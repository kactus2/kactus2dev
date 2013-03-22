/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator2.cpp
 *		Project: Kactus 2
 *
 *      Creates a structural VHDL file with following structure
 *      1. File header 
 *      2. Libraries
 *      3. Entity declaration
 *      4. Architecture
 *         - Signals (+ user's own code)
 *         - Component declarations
 *         - Component instantiations (+ user's own code)
 */

#include "vhdlgenerator2.h"

#include "vhdlgeneral.h"

#include <LibraryManager/libraryinterface.h>

#include <models/view.h>
#include <models/librarycomponent.h>
#include <models/modelparameter.h>
#include <models/port.h>
#include <models/businterface.h>
#include <models/generaldeclarations.h>
#include <models/fileset.h>
#include <models/file.h>

#include <designwidget/HWDesignWidget.h>
#include <IPXactWrapper/ComponentEditor/treeStructure/componenteditor.h>

#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QDate>
#include <QSettings>

static const QString BLACK_BOX_DECL_START = "-- ##KACTUS2_BLACK_BOX_DECLARATIONS_BEGIN##";
static const QString BLACK_BOX_DECL_END = "-- ##KACTUS2_BLACK_BOX_DECLARATIONS_END##";
static const QString BLACK_BOX_ASSIGN_START = "-- ##KACTUS2_BLACK_BOX_ASSIGNMENTS_BEGIN##";
static const QString BLACK_BOX_ASSIGN_END = "-- ##KACTUS2_BLACK_BOX_ASSIGNMENTS_END##";

VhdlGenerator2::VhdlGenerator2(LibraryInterface* handler, HWDesignWidget* parent):
QObject(parent),
handler_(handler),
component_(),
design_(),
desConf_(),
viewName_(),
topLevelEntity_(),
libraries_(),
typeDefinitions_(),
userModifiedDeclarations_(),
userModifiedAssignments_(),
topGenerics_(),
topPorts_(),
signals_(),
components_(),
instances_() {

	Q_ASSERT(handler);
	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
}

VhdlGenerator2::VhdlGenerator2( LibraryInterface* handler, 
							   ComponentEditor* parent ):
QObject(parent),
handler_(handler),
component_(),
design_(),
desConf_(),
viewName_(),
topLevelEntity_(),
libraries_(),
typeDefinitions_(),
userModifiedDeclarations_(),
userModifiedAssignments_(),
topGenerics_(),
topPorts_(),
signals_(),
components_(),
instances_() {

	Q_ASSERT(handler);
	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
}

VhdlGenerator2::~VhdlGenerator2() {
}

LibraryInterface* VhdlGenerator2::handler() const {
	return handler_;
}

bool VhdlGenerator2::parse( QSharedPointer<Component> topLevelComponent, 
						   const QString& viewName ) {
	
	Q_ASSERT(topLevelComponent);
	component_ = topLevelComponent;

	emit noticeMessage(tr("Parsing the IP-Xact models..."));

	// the name of the top-level entity is the name of the top-level component
	topLevelEntity_ = component_->getEntityName(viewName);
	viewName_ = viewName;
	
	// if the parsing fails
	if (!parseDesignAndConfiguration()) {
		return false;
	}

	// get the types that are used for the ports.
	typeDefinitions_ = component_->getPortTypeDefinitions();

	// parse the info for the top entity
	parseTopGenerics();
	parseTopPorts();

	// if design is used then these can be parsed also
	if (design_) {
		parseInstances();
		parseInterconnections();
		parseAdHocConnections();
		parseHierConnections();
		mapPorts2Signals();

		// tell each instance to use the default port value for the unconnected
		// ports.
		foreach (QSharedPointer<VhdlComponentInstance> instance, instances_) {
			instance->useDefaultsForOtherPorts();
		}

		// tell each component declaration to check for it's ports and 
		// uncomment those that are needed
		foreach (QSharedPointer<VhdlComponentDeclaration> comp, components_) {
			comp->checkPortConnections();
		}
	}
	// if design is not used then set all ports uncommented.
	else {

		for (QMap<VhdlPortSorter, QSharedPointer<VhdlPort> >::iterator i = topPorts_.begin();
			i != topPorts_.end(); ++i) {

			i.value()->setCommented(false);
		}
	}

	return true;
}

void VhdlGenerator2::generateVhdl( const QString& outputFileName) {
			
	if (outputFileName.isEmpty()) {
		return;
	}

	QFile file(outputFileName);

	// if previous file exists then remove it.
	if (file.exists()) {

		// read the existing user-modifiable code from the file
		readUserModifiablePart(file);

		file.remove();
	}

	// open the file and erase all old contents if any exists
	// if file could not be opened
	if (!file.open(QFile::Truncate | QFile::WriteOnly)) {

		QString message(tr("File: "));
		message += outputFileName;
		message += tr(" couldn't be opened for writing");
		emit errorMessage(message);
		return;
	}
	// now file has been opened for writing

	// open file stream to write to
	QTextStream vhdlStream(&file);

	QFileInfo fileInfo(outputFileName);

	emit noticeMessage(tr("Writing the vhdl file..."));

	
	// write header comments of the file
	writeVhdlHeader(vhdlStream, fileInfo.fileName());

	// always add IEEE library
	vhdlStream << "library IEEE;" << endl;

	// write the libraries needed 
	libraries_.append("work");
	libraries_.removeDuplicates();

	// declare the libraries used.
	foreach (QString library, libraries_) {
		if (!library.isEmpty()) {
			vhdlStream << "library " << library <<";" << endl;

			typeDefinitions_.append(QString("%1.all").arg(library));
		}
	}

	// always add the library for std_logic and std_logic_vector
	typeDefinitions_.append("IEEE.std_logic_1164.all");
	typeDefinitions_.removeDuplicates();

	// write all type defs needed
	foreach (QString portTypeDef, typeDefinitions_) {
		if (!portTypeDef.isEmpty()) {
			vhdlStream << "use " << portTypeDef << ";" << endl;
		}
	}
	vhdlStream << endl;




	// write the top-level entity
	vhdlStream << "entity " << topLevelEntity_ << " is" << endl;
	vhdlStream << endl;

	// write the top-level generics
	writeGenerics(vhdlStream);

	// write the top-level ports
	writePorts(vhdlStream);

	// end top-level entity definition
	vhdlStream << "end " << topLevelEntity_ << ";" << endl << endl;


	// if view has description
	QString viewDescription = component_->getViewDescription(viewName_);
	VhdlGeneral::writeDescription(viewDescription, vhdlStream);

	QString archName;
	// if view name is not specified then "structural" is used
	if (viewName_.isEmpty()) {
		archName = "structural";
	}
	else {
		archName = viewName_;
	}
	// write the architecture of the entity
	vhdlStream << "architecture " << archName << " of " << topLevelEntity_ << " is"
		<< endl << endl;

	// write declarations for signals connecting the ports
	writeSignalDeclarations(vhdlStream);

	// write all component declarations
	writeComponentDeclarations(vhdlStream);

	// write the user modifiable code 
	writeUserModifiedDeclarations(vhdlStream);

	// start writing architecture component instances
	vhdlStream << endl << "begin" << endl << endl;

	// write the user modifiable code
	writeUserModifiedAssignments(vhdlStream);

	// write the component instances
	writeComponentInstances(vhdlStream);

	vhdlStream << "end " << archName << ";" << endl << endl;

	file.close();

	emit noticeMessage(tr("Done writing the vhdl file."));

	return;
}

bool VhdlGenerator2::addRTLView( const QString& vhdlFileName ) {
	// ipDir represents the directory where the IP-Xact file is located in.
	QString ipDir(handler_->getPath(*component_->getVlnv()));
	if (ipDir.isEmpty()) {
		emit errorMessage(tr("Path to top-component was not found."));
		return false;
	}

	// get the relative path to add to file set
	QString relativePath = General::getRelativePath(ipDir, vhdlFileName);
	if (relativePath.isEmpty()) {
		emit errorMessage(tr("Could not create relative path to vhdl file."));
		return false;
	}

	QString fileSetName;
	if (!viewName_.isEmpty()) {
		fileSetName = QString("%1_vhdlSource").arg(viewName_);
	}
	else {
		fileSetName = QString("vhdlSource");
	}
	QSharedPointer<FileSet> topFileSet = component_->getFileSet(fileSetName);

	// if the top vhdl file set was not found. Create one
	if (!topFileSet) {
		topFileSet = QSharedPointer<FileSet>(new FileSet(fileSetName, QString("sourceFiles")));
		topFileSet->useDefaultVhdlBuilders();
		component_->addFileSet(topFileSet);
	}

	topFileSet->clearFiles();

	// create a new file
	QSharedPointer<File> topVhdlFile(new File(relativePath, topFileSet.data()));
	topVhdlFile->addFileType(QString("vhdlSource"));
	topVhdlFile->setIncludeFile(true);
	topVhdlFile->setLogicalName("work");
	topVhdlFile->setCommand(QString("vcom"));
	topVhdlFile->setBuildFlags(QString("-quiet -check_synthesis -work work"), true);

	// add the new file to the file set
	topFileSet->addFile(topVhdlFile);

	QString viewName;
	if (!viewName_.isEmpty()) {
		viewName = QString("%1_vhd").arg(viewName_);
	}
	else {
		viewName = "rtl";
	}
	View* rtlView = new View(viewName);

	// add the spirit:envIdentifier. Only language is defined, not tool
	// specific settings
	rtlView->addEnvIdentifier(QString("VHDL:Kactus2:"));

	// set the language of the view.
	rtlView->setLanguage(QString("vhdl"));

	// set the model name to be the top_level architecture of the top-level
	// entity
	QString archName;
	if (viewName_.isEmpty()) {
		archName = "rtl";
	}
	else {
		archName = viewName_;
	}
	QString topEntity(QString("%1(%2)").arg(topLevelEntity_).arg(archName));
	rtlView->setModelName(topEntity);

	// set a reference to a file set
	rtlView->addFileSetRef(fileSetName);

	// add the view to the component's model
	component_->addView(rtlView);

	// find the active view used to generate the vhdl
	View* activeView = component_->findView(viewName_);
	
	// if the view does not exist or it is not hierarchical
	if (!activeView || !activeView->isHierarchical()) {
		return true;
	}

	activeView->setTopLevelView(rtlView->getName());

	return true;
}

bool VhdlGenerator2::parseDesignAndConfiguration() {
	Q_ASSERT(component_);

	// if view is not specified it is not error it just means that only
	// the top entity should be created
	if (viewName_.isEmpty()) {
		return true;
	}

	// if view is specified but it does not exist
	View* view = component_->findView(viewName_);
	
	// if view is not found
	if (!view) {
		return false;
	}
	else if (!view->isHierarchical()) {
		return false;
	}

	VLNV hierarchyRef = view->getHierarchyRef();
	
	// get the type of document for the hierarchical reference
	VLNV::IPXactType documentType = handler_->getDocumentType(hierarchyRef);

	// if configuration is used
	if (documentType == VLNV::DESIGNCONFIGURATION) {
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(hierarchyRef);
		desConf_ = libComp.staticCast<DesignConfiguration>();
	}
	// if the referenced document was not found
	else if (documentType == VLNV::INVALID) {
		emit errorMessage(tr("The Hierarchical reference %1 within component %2's"
			" view %3 was not found in library.").arg(
			hierarchyRef.toString()).arg(
			component_->getVlnv()->toString()).arg(
			viewName_));
		return false;
	}
	
	design_ = handler_->getDesign(hierarchyRef);
	// if design was not found
	if (!design_) {
		VLNV designVLNV = handler_->getDesignVLNV(hierarchyRef);
		emit errorMessage(tr("The design %1 referenced in component %2 was not "
			"found in library.").arg(
			designVLNV.toString()).arg(component_->getVlnv()->toString()));
		return false;
	}

	// if design is found then make sure it is valid
	else {

		QStringList errorList;
		if (!design_->isValid(errorList)) {

			emit noticeMessage(tr("The Design contained following errors:"));

			foreach (QString error, errorList) {
				emit errorMessage(error);
			}
			return false;
		}
	}

	// if design configuration is found the make sure it is also valid
	if (desConf_) {
		QStringList errorList;

		if (!desConf_->isValid(errorList)) {
			emit noticeMessage(tr("The configuration contained following errors:"));

			foreach (QString error, errorList) {
				emit errorMessage(error);
			}
			return false;
		}
	}
	

	// the design and possibly the configuration are now parsed
	return true;
}

bool VhdlGenerator2::containsArchitecture() const {
	// if design exists then architecture can be created
	return design_;
}

void VhdlGenerator2::parseTopGenerics() {

	QList<QSharedPointer<ModelParameter> > modelParams = component_->getModelParameters();
	foreach (QSharedPointer<ModelParameter> modelParam, modelParams) {
		QString name = modelParam->getName();
		QSharedPointer<VhdlGeneric> generic(new VhdlGeneric(this, modelParam.data()));

		topGenerics_.insert(name, generic);
	}
}

void VhdlGenerator2::parseTopPorts() {
	
	QList<QSharedPointer<Port> > ports = component_->getPorts();
	foreach (QSharedPointer<Port> port, ports) {

		// do not add ports with invalid direction or phantom direction
		if (port->getDirection() == General::DIRECTION_INVALID ||
			port->getDirection() == General::DIRECTION_PHANTOM) {
				continue;
		}

		// create the actual port
		QSharedPointer<VhdlPort> vhdlPort(new VhdlPort(this, port.data()));

		// create the sorter instance
		VhdlPortSorter sorter(component_->getInterfaceNameForPort(vhdlPort->name()),
			vhdlPort->name(), port->getDirection());

		// this port can not be created yet
		Q_ASSERT(!topPorts_.contains(sorter));

		topPorts_.insert(sorter, vhdlPort);
	}
}

void VhdlGenerator2::parseInstances() {
	Q_ASSERT(design_);

	QList<ComponentInstance> const& instances = design_->getComponentInstances();

	foreach (ComponentInstance const& instance, instances) {
		VLNV::IPXactType instanceType = handler_->getDocumentType(instance.getComponentRef());

		// if vlnv is not found in library
		if (instanceType == VLNV::INVALID) {
			emit errorMessage(tr("Component %1 referenced in design %2 was not"
				" found in library.").arg(
				instance.getComponentRef().toString()).arg(
				component_->getVlnv()->toString()));
			continue;
		}
		// if vlnv does not reference a component
		else if (instanceType != VLNV::COMPONENT) {
			emit errorMessage(tr("VLNV %1 does not belong to a component.").arg(
				instance.getComponentRef().toString()));
			continue;
		}

		// pointer to the matching component declaration
		QSharedPointer<VhdlComponentDeclaration> compDeclaration;
		
		// if component declaration is already created
		if (components_.contains(instance.getComponentRef())) {
			compDeclaration = components_.value(instance.getComponentRef());
		}

		// if component declaration is not yet created then create it
		else {
			QSharedPointer<LibraryComponent> libComp = handler_->getModel(instance.getComponentRef());
			QSharedPointer<Component> component = libComp.staticCast<Component>();
			Q_ASSERT(component);
			compDeclaration = QSharedPointer<VhdlComponentDeclaration>(
				new VhdlComponentDeclaration(component, this));

			components_.insert(instance.getComponentRef(), compDeclaration);
		}
		Q_ASSERT(compDeclaration);

		QString instanceActiveView;
		// if configuration is used then get the active view for the instance
		if (desConf_) {
			instanceActiveView = desConf_->getActiveView(instance.getInstanceName());
		}

		// create the instance
		QSharedPointer<VhdlComponentInstance> compInstance(new VhdlComponentInstance(
			this, compDeclaration.data(), instance.getInstanceName(), instanceActiveView,
			instance.getDescription()));

		// add the libraries of the instantiated component to the library list.
		libraries_.append(compInstance->componentModel()->getVhdlLibraries(instanceActiveView));

		// add each generic value to instances generic map
        for (QMap<QString, QString>::const_iterator i = instance.getConfigurableElementValues().begin();
			i != instance.getConfigurableElementValues().end(); ++i) {
			compInstance->addGenericMap(i.key(), i.value());
		}

		// register the instance to the component declaration
		compDeclaration->addInstantiation(compInstance);

		instances_.insert(instance.getInstanceName(), compInstance);
	}
}

void VhdlGenerator2::parseInterconnections() {
	Q_ASSERT(design_);

	QList<Design::Interconnection> interconnections =
		design_->getInterconnections();

	QStringList connectionNames;
	foreach (Design::Interconnection interconnection, interconnections) {

		bool invalidInterconnection = false;
		QSharedPointer<VhdlComponentInstance> instance1;
		QSharedPointer<BusInterface> interface1;
		QSharedPointer<VhdlComponentInstance> instance2;
		QSharedPointer<BusInterface> interface2;
		
		// if there are several interconnections with same name
		if (connectionNames.contains(interconnection.name)) {
			emit errorMessage(tr("Design %1 contained more than one interconnection"
				" named %2").arg(
				component_->getVlnv()->toString()).arg(
				interconnection.name));
		}
		else {
			connectionNames.append(interconnection.name);
		}

		// if the instance reference is incorrect
		if (!instances_.contains(interconnection.interface1.componentRef)) {
			invalidInterconnection = true;
			emit errorMessage(tr("Instance %1 was not found in component %2.").arg(
				interconnection.interface1.componentRef).arg(
				component_->getVlnv()->toString()));
		}
		else { 
			instance1 = instances_.value(interconnection.interface1.componentRef);
			interface1 = instance1->interface(interconnection.interface1.busRef);
		}
		// if the interface is not found
		if (!interface1) {
			emit errorMessage(tr("Bus interface %1 was not found in component %2.").arg(
				interconnection.interface1.busRef).arg(
				instance1->vlnv().toString()));
			invalidInterconnection = true;
		}

		// if the instance reference is incorrect
		if (!instances_.contains(interconnection.interface2.componentRef)) {
			invalidInterconnection = true;
			emit errorMessage(tr("Instance %1 was not found in component %2.").arg(
				interconnection.interface2.componentRef).arg(
				component_->getVlnv()->toString()));
		}
		else {
			instance2 = instances_.value(interconnection.interface2.componentRef);
			interface2 = instance2->interface(interconnection.interface2.busRef);
		}
		// if the interface is not found
		if (!interface2) {
			emit errorMessage(tr("Bus interface %1 was not found in component %2.").arg(
				interconnection.interface2.busRef).arg(
				instance2->vlnv().toString()));
			invalidInterconnection = true;
		}

		// if the interconnection is invalid then move on to next interconnection
		if (invalidInterconnection) {
			continue;
		}
		else {
			connectInterfaces(interconnection.name, interconnection.description,
				instance1, interface1, instance2, interface2);
		}
	}
}

void VhdlGenerator2::connectInterfaces( const QString& connectionName, 
									   const QString& description,
									   QSharedPointer<VhdlComponentInstance> instance1,
									   QSharedPointer<BusInterface> interface1, 
									   QSharedPointer<VhdlComponentInstance> instance2, 
									   QSharedPointer<BusInterface> interface2 ) {
	Q_ASSERT(instance1);
	Q_ASSERT(interface1);
	Q_ASSERT(instance2);
	Q_ASSERT(interface2);

	// get the port maps of both interfaces
	QList<QSharedPointer<General::PortMap> > portMaps1 = interface1->getPortMaps();
	QList<QSharedPointer<General::PortMap> > portMaps2 = interface2->getPortMaps();

	// get the IP-Xact models of both instances
	QSharedPointer<Component> component1 = instance1->componentModel();
	QSharedPointer<Component> component2 = instance2->componentModel();

	foreach (QSharedPointer<General::PortMap> portMap1, portMaps1) {
		foreach(QSharedPointer<General::PortMap> portMap2, portMaps2) {

			const QString port1Name = portMap1->physicalPort_;
			const QString port2Name = portMap2->physicalPort_;

			// if the port maps are not for same logical signal
			if (portMap1->logicalPort_ != portMap2->logicalPort_) {
				continue;
			}
			// if port does not exist in instance 1
			else if (!component1->hasPort(port1Name)) {
				emit errorMessage(tr("Port %1 was not defined in component %2.").arg(
					port1Name).arg(
					component1->getVlnv()->toString()));
				continue;
			}
			// if port does not exist in instance 2
			else if (!component2->hasPort(port2Name)) {
				emit errorMessage(tr("Port %1 was not defined in component %2.").arg(
					port2Name).arg(
					component2->getVlnv()->toString()));
				continue;
			}
			// if the port on instance 1 is not valid physical port
			else if (!component1->isPhysicalPort(port1Name)) {
				continue;
			}
			// if the port on instance 2 is not valid physical port
			else if (!component2->isPhysicalPort(port2Name)) {
				continue;
			}

			// get the alignments of the ports
			General::PortAlignment alignment = 
				General::calculatePortAlignment(portMap1.data(), 
				component1->getPortLeftBound(port1Name),
				component1->getPortRightBound(port1Name),
				portMap2.data(),
				component2->getPortLeftBound(port2Name),
				component2->getPortRightBound(port2Name));

			// if the alignment is not valid (port sizes do not match or 
			// they do not have any common bits)
			if (alignment.invalidAlignment_) {
				continue;
			}

			// create the name for the new signal
			const QString signalName = connectionName + "_" + portMap1->logicalPort_;
			
			VhdlGenerator2::PortConnection port1(instance1, port1Name, 
				alignment.port1Left_, alignment.port1Right_);
			VhdlGenerator2::PortConnection port2(instance2, port2Name,
				alignment.port2Left_, alignment.port2Right_);
			QList<VhdlGenerator2::PortConnection> ports;
			ports.append(port1);
			ports.append(port2);

			// connect the port from both port maps
			connectPorts(signalName, description, ports);
		}
	}
}

void VhdlGenerator2::connectPorts(const QString& connectionName, 
								  const QString& description, 
								  const QList<VhdlGenerator2::PortConnection>& ports) {

	// at least 2 ports must be found
	Q_ASSERT(ports.size() > 1);

	// the type of the signal
	QString type = ports.first().instance_->portType(ports.first().portName_);

	// the minSize for the signal
	int minSize = ports.first().leftBound_ - ports.first().rightBound_ + 1;
	int maxSize = minSize;

	// first find out the smallest possible minSize for the signal
	foreach (VhdlGenerator2::PortConnection connection, ports) {
		// the smallest minSize needed is used
		minSize = qMin(minSize, (connection.leftBound_ - connection.rightBound_ + 1));
	}

	// calculate the bounds for the signal
	int left = minSize - 1;
	int right = 0;

	// create the endpoints
	QList<VhdlConnectionEndPoint> endpoints;
	foreach (VhdlGenerator2::PortConnection connection, ports) {

		QString connectionType = connection.instance_->portType(connection.portName_);

		// make sure all ports are for compatible type
		if (!VhdlGeneral::checkVhdlTypeMatch(type, connectionType)) {
			QString instance1(QString("instance %1 port %2 type %3").arg(
				ports.first().instance_->name()).arg(
				ports.first().portName_).arg(
				type));
			QString instance2(QString("instance %1 port %2 type %3").arg(
				connection.instance_->name()).arg(
				connection.portName_).arg(
				connectionType));
			emit errorMessage(tr("Type mismatch: %1 - %2.").arg(
				instance1).arg(
				instance2));
		}

		VhdlConnectionEndPoint endpoint(connection.instance_->name(), connection.portName_,
			connection.leftBound_, connection.rightBound_, maxSize-1, right);
		endpoints.append(endpoint);
	}

	// if vectored but minSize is only one then convert to scalar signal type
	if (type == "std_logic_vector" && minSize == 1) {
		type = "std_logic";
	}

	// if scalar signal type but minSize is larger than 1 then convert to vectored
	else if (type == "std_logic" && minSize > 1) {
		type == "std_logic_vector";
	}

	// the signal that connects the end points
	QSharedPointer<VhdlSignal> signal;

	// find a signal to use for connecting the end points
	foreach (VhdlConnectionEndPoint endpoint, endpoints) {
		if (signals_.contains(endpoint)) {
			signal = signals_.value(endpoint);
			signal->setBounds(qMax(left, int(signal->left())),
				qMin(right, int(signal->right())));
			break;
		}
	}
	// if signal was not found then create a new one 
	if (!signal) {
		signal = QSharedPointer<VhdlSignal>(new VhdlSignal(this, connectionName,
			type, left, right, description));
		signal->setBounds(left, right);
	}

	// connect each end point to given signal
	foreach (VhdlConnectionEndPoint endpoint, endpoints) {
		connectEndPoint(endpoint, signal);
	}
}

void VhdlGenerator2::connectEndPoint( const VhdlConnectionEndPoint& endpoint,
									 const QSharedPointer<VhdlSignal> signal ) {

	Q_ASSERT(signal);

	// if the end point already has a signal associated with it
	if (signals_.contains(endpoint)) {

		QSharedPointer<VhdlSignal> oldSignal = signals_.value(endpoint);
		// get all end points associated with signal for endpoint 2
		QList<VhdlConnectionEndPoint> endpoints = signals_.keys(oldSignal);
		// replace each end point with association to signal for endpoint
		foreach (VhdlConnectionEndPoint temp, endpoints) {
			signals_.insert(temp, signal);
		}
	}
	// if end point was not yet specified
	else {
		signals_.insert(endpoint, signal);
	}
}

void VhdlGenerator2::parseAdHocConnections() {
	// all adHoc connections are processed
	QList<Design::AdHocConnection> adHocs = design_->getAdHocConnections();
	foreach (Design::AdHocConnection adHoc, adHocs) {

		// the data structure to store the ports to connect
		QList<VhdlGenerator2::PortConnection> ports;

		foreach (Design::PortRef portRef, adHoc.internalPortReferences) {

			// if the instance is not found
			if (!instances_.contains(portRef.componentRef)) {
				emit errorMessage(tr("Instance %1 was not found in design.").arg(
					portRef.componentRef));
				continue;
			}
			QSharedPointer<VhdlComponentInstance> instance =
				instances_.value(portRef.componentRef);
			Q_ASSERT(instance);

			// if the specified port is not found
			if (!instance->hasPort(portRef.portRef)) {
				emit errorMessage(tr("Port %1 was not found in instance %2 of"
					" type %3").arg(portRef.portRef).arg(
					portRef.componentRef).arg(
					instance->vlnv().toString()));
				continue;
			}

			int left = portRef.left;
			// if left bound is not defined then use the port physical bound
			if (left < 0) {
				left = instance->getPortPhysLeftBound(portRef.portRef);
			}

			int right = portRef.right;
			// if right bound is not defined then use the port physical bound
			if (right < 0) {
				right = instance->getPortPhysRightBound(portRef.portRef);
			}

			// create the port specification
			VhdlGenerator2::PortConnection port(instance, portRef.portRef,
				left, right);

			// add port to the list
			ports.append(port);
		}

		// if the connection contains at least one port of the top component
		if (!adHoc.externalPortReferences.isEmpty()) {

			// connect each external port to instance port
			foreach (Design::PortRef portRef, adHoc.externalPortReferences) {

				// check that the port is found 
				VhdlPortSorter sorter(component_->getInterfaceNameForPort(portRef.portRef),
					portRef.portRef, 
					component_->getPortDirection(portRef.portRef));
				if (!topPorts_.contains(sorter)) {
					emit errorMessage(tr("The port %1 is not found in top component %2").arg(
						portRef.portRef).arg(component_->getVlnv()->toString()));
					continue;
				}

				QSharedPointer<VhdlPort> port = topPorts_.value(sorter);
				Q_ASSERT(port);

				int left = portRef.left;
				// if left bound is not defined then use the port physical bound
				if (left < 0) {
					left = port->left();
				}

				int right = portRef.right;
				// if right bound is not defined then use the port physical bound
				if (right < 0) {
					right = port->right();
				}

				// connect each instance port to the top port
				connectHierPort(portRef.portRef, left, right, ports);
			}
		}
		// otherwise the connection is just between the ports of the instances
		else {

			// connect the ports 
			connectPorts(adHoc.name, adHoc.description, ports);
		}
	}
}

void VhdlGenerator2::connectHierPort( const QString& topPort,
									 int leftBound, 
									 int rightBound, 
									 const QList<VhdlGenerator2::PortConnection>& ports ) {

	foreach (VhdlGenerator2::PortConnection port, ports) {
		
		// if port is scalar then don't add the bit boundaries
		int portLeft = port.leftBound_;
		int portRight = port.rightBound_;
		if (port.instance_->isScalarPort(port.portName_)) {
			portLeft = -1;
			portRight = -1;
		}

		VhdlPortSorter sorter(component_->getInterfaceNameForPort(topPort), 
			topPort, component_->getPortDirection(topPort));
		// if the port was found in top component then set it as uncommented 
		// because it is needed
		if (topPorts_.contains(sorter)) {
			QSharedPointer<VhdlPort> vhdlPort = topPorts_.value(sorter);
			vhdlPort->setCommented(false);

			// if the top port is scalar then don't use the bit boundaries
			if (VhdlGeneral::isScalarType(vhdlPort->type())) {
				leftBound = -1;
				rightBound = -1;
			}
		}
		else {
			emit errorMessage(tr("Port %1 was not found in top component %2.").arg(
				topPort).arg(component_->getVlnv()->toString()));
			continue;
		}

		// tell each instance to create a port map between the ports
		port.instance_->addPortMap(port.portName_, portLeft, portRight,
			port.instance_->portType(port.portName_),
			topPort, leftBound, rightBound, topPorts_.value(sorter)->type());
	}
}

void VhdlGenerator2::parseHierConnections() {
	// search all hierConnections within design
	QList<Design::HierConnection> hierConnections =
		design_->getHierarchicalConnections();

	foreach (Design::HierConnection hierConnection, hierConnections) {
	
		// find the top-level bus interface
		QSharedPointer<BusInterface> topInterface =
			component_->getBusInterface(hierConnection.interfaceRef);

		// if the top level interface couldn't be found
		if (!topInterface) {
			emit errorMessage(tr("Interface %1 was not found in top component %2.").arg(
				hierConnection.interfaceRef).arg(
				component_->getVlnv()->toString()));
			continue;
		}

		// find the component instance for this hierConnection
		QSharedPointer<VhdlComponentInstance> instance = instances_.value(hierConnection.interface_.componentRef);

		// if the component instance couldn't be found
		if (!instance) {
			emit errorMessage(tr("Instance %1 was not found in design.").arg(
				hierConnection.interface_.componentRef));
			continue;
		}

		// find the bus interface
		QSharedPointer<BusInterface> instanceInterface = instance->interface(hierConnection.interface_.busRef);
		// if bus interface couldn't be found
		if (!instanceInterface) {
			emit errorMessage(tr("Interface %1 was not found in instance %2 of type %3").arg(
				hierConnection.interface_.busRef).arg(
				instance->name()).arg(
				instance->typeName()));
			continue;
		}

		connectHierInterface(instance, instanceInterface, topInterface);
	}
}

void VhdlGenerator2::connectHierInterface( QSharedPointer<VhdlComponentInstance> instance, QSharedPointer<BusInterface> instanceInterface, QSharedPointer<BusInterface> topInterface ) {
	Q_ASSERT(instance);
	Q_ASSERT(instanceInterface);
	Q_ASSERT(topInterface);

	// get the port maps of both interfaces
	QList<QSharedPointer<General::PortMap> > portMaps = instanceInterface->getPortMaps();
	QList<QSharedPointer<General::PortMap> > hierPortMaps = topInterface->getPortMaps();

	// get the IP-Xact model of the instance
	QSharedPointer<Component> component1 = instance->componentModel();

	foreach (QSharedPointer<General::PortMap> portMap, portMaps) {
		foreach(QSharedPointer<General::PortMap> hierPortMap, hierPortMaps) {

			const QString portName = portMap->physicalPort_;
			const QString hierPortName = hierPortMap->physicalPort_;

			// if the port maps are not for same logical signal
			if (portMap->logicalPort_ != hierPortMap->logicalPort_) {
				continue;
			}
			// if port does not exist in instance 1
			else if (!component1->hasPort(portName)) {
				emit errorMessage(tr("Port %1 was not defined in component %2.").arg(
					portName).arg(
					component1->getVlnv()->toString()));
				continue;
			}
			// if port does not exist in instance 2
			else if (!component_->hasPort(hierPortName)) {
				emit errorMessage(tr("Port %1 was not defined in component %2.").arg(
					hierPortName).arg(
					component_->getVlnv()->toString()));
				continue;
			}
			// if the port on instance 1 is not valid physical port
			else if (!component1->isPhysicalPort(portName)) {
				continue;
			}
			// if the port on instance 2 is not valid physical port
			else if (!component_->isPhysicalPort(hierPortName)) {
				continue;
			}

			// get the alignments of the ports
			General::PortAlignment alignment = 
				General::calculatePortAlignment(portMap.data(), 
				component1->getPortLeftBound(portName),
				component1->getPortRightBound(portName),
				hierPortMap.data(),
				component_->getPortLeftBound(hierPortName),
				component_->getPortRightBound(hierPortName));

			// if the alignment is not valid (port sizes do not match or 
			// they do not have any common bits)
			if (alignment.invalidAlignment_) {
				continue;
			}

			VhdlGenerator2::PortConnection port1(instance, portName, 
				alignment.port1Left_, alignment.port1Right_);
			QList<VhdlGenerator2::PortConnection> ports;
			ports.append(port1);

			// connect the port from instance to the top port
			connectHierPort(hierPortName, alignment.port2Left_, alignment.port2Right_, ports);
		}
	}
}

void VhdlGenerator2::mapPorts2Signals() {
	// search each endpoint
	for (QMap<VhdlConnectionEndPoint, QSharedPointer<VhdlSignal> >::iterator i = signals_.begin(); 
		i != signals_.end(); ++i) {

		// if the component instance can't be found
		if (!instances_.contains(i.key().instanceName())) {
			emit errorMessage(tr("Instance %1 was not found in design.").arg(
				i.key().instanceName()));
			continue;
		}

		// find the instance for the end point
		QSharedPointer<VhdlComponentInstance> instance = instances_.value(i.key().instanceName());

		// if port is scalar then don't add the bit boundaries
		int portLeft = i.key().portLeft();
		int portRight = i.key().portRight();
		if (instance->isScalarPort(i.key().portName())) {
			portLeft = -1;
			portRight = -1;
		}

		// if signal is scalar then don't add the bit boundaries
		int signalLeft = i.key().signalLeft();
		int signalRight = i.key().signalRight();
		if (VhdlGeneral::isScalarType(i.value()->type())) {
			signalLeft = -1;
			signalRight = -1;
		}

		// add a port map for the instance to connect port to signal
		instance->addPortMap(i.key().portName(), portLeft, portRight, 
			instance->portType(i.key().portName()),
			i.value()->name(), signalLeft, signalRight, i.value()->type());	
	}
}

void VhdlGenerator2::writeVhdlHeader( QTextStream& vhdlStream, const QString& fileName ) {
	
	vhdlStream << "-- ***************************************************" << endl;
	vhdlStream << "-- File         : " << fileName << endl;
	vhdlStream << "-- Creation date: " << QDate::currentDate().toString(QString("dd.MM.yyyy")) << endl;
	vhdlStream << "-- Creation time: " << QTime::currentTime().toString(QString("hh:mm:ss")) << endl;
	vhdlStream << "-- Description  : " << endl;
	VhdlGeneral::writeDescription(component_->getDescription(), vhdlStream, QString(""));
	vhdlStream << "-- " << endl;

	QSettings settings;
	QString userName = settings.value("General/Username", getenv("USERNAME")).toString();
	vhdlStream << "-- Created by   : " << userName << endl; 
	vhdlStream << "-- This file was generated with Kactus2 vhdl generator" << endl;
	VLNV* vlnv = component_->getVlnv();
	vhdlStream << "-- based on IP-XACT component " << vlnv->toString() << endl;
	vhdlStream << "-- whose XML file is " << handler_->getPath(*vlnv) << endl;
	vhdlStream << "-- ***************************************************" << endl;
}

void VhdlGenerator2::writeGenerics( QTextStream& vhdlStream ) {
	// if generics exist
	if (!topGenerics_.isEmpty()) {

		// the start tag
		vhdlStream << "  " << "generic (" << endl;

		for (QMap<QString, QSharedPointer<VhdlGeneric> >::iterator i = topGenerics_.begin();
			i != topGenerics_.end(); ++i) {

			vhdlStream << "  " << "  ";
			i.value()->write(vhdlStream);

			// if this is not the last generic to write
			if (i + 1 != topGenerics_.end()) {
				vhdlStream << ";";
			}

			if (!i.value()->description().isEmpty()) {
				vhdlStream << " ";
				VhdlGeneral::writeDescription(i.value()->description(), vhdlStream);
			}
			else {
				vhdlStream << endl;
			}
		}
		vhdlStream << "  " << ");" << endl;
		vhdlStream << endl;
	}
}

void VhdlGenerator2::writePorts( QTextStream& vhdlStream ) {
	// if ports exist
	if (!topPorts_.isEmpty() && VhdlPort::hasRealPorts(topPorts_)) {

		vhdlStream << "  " << "port (" << endl;

		QString previousInterface;
		// print each port
		for (QMap<VhdlPortSorter, QSharedPointer<VhdlPort> >::iterator i = topPorts_.begin();
			i != topPorts_.end(); ++i) {

				// if the port is first in the interface then introduce it
				if (i.key().interface() != previousInterface) {
					const QString interfaceName = i.key().interface();

					vhdlStream << endl << "  " << "  " << "-- ";

					if (interfaceName == QString("none")) {
						vhdlStream << "These ports are not in any interface" << endl;
					}
					else if (interfaceName == QString("several")) {
						vhdlStream << "There ports are contained in many interfaces" << endl;
					}
					else {	
						vhdlStream << "Interface: " << interfaceName << endl;
						const QString description = component_->getInterfaceDescription(
							interfaceName);
						if (!description.isEmpty()) {							
							VhdlGeneral::writeDescription(description, vhdlStream, QString("    "));
						}
					}
					previousInterface = interfaceName;
				}

				// write the port name and direction
				vhdlStream << "  " << "  ";
				i.value()->write(vhdlStream);

				// if this is not the last port to write
				if (i + 1 != topPorts_.end()) {
					vhdlStream << ";";
				}
				
				if (!i.value()->description().isEmpty()) {
					vhdlStream << " ";
					VhdlGeneral::writeDescription(i.value()->description(), vhdlStream);
				}
				else {
					vhdlStream << endl;
				}
		}
		vhdlStream << "  " << ");" << endl;
		// write extra empty line to make code readable
		vhdlStream << endl;
	}
}

void VhdlGenerator2::writeSignalDeclarations( QTextStream& vhdlStream ) {
	
	QList<QSharedPointer<VhdlSignal> > signalList;

	// get the unique signals that exist
	foreach (QSharedPointer<VhdlSignal> signal, signals_.values()) {
		if (!signalList.contains(signal)) {
			signalList.append(signal);
		}
	}
	// when the list contains only the unique signals then write them
	foreach (QSharedPointer<VhdlSignal> signal, signalList) {
		signal->write(vhdlStream);
	}
	vhdlStream << endl;
}

void VhdlGenerator2::writeComponentDeclarations( QTextStream& vhdlStream ) {
	foreach (QSharedPointer<VhdlComponentDeclaration> comp, components_) {
		comp->write(vhdlStream);
		vhdlStream << endl;
	}
}

void VhdlGenerator2::writeComponentInstances( QTextStream& vhdlStream ) {
	foreach (QSharedPointer<VhdlComponentInstance> instance, instances_) {
		instance->write(vhdlStream);
		vhdlStream << endl;
	}
}

void VhdlGenerator2::readUserModifiablePart( QFile& previousFile ) {
	Q_ASSERT(previousFile.exists());

	// clear the previous stuff
	userModifiedAssignments_.clear();
	userModifiedDeclarations_.clear();

	// open the file
	// if file could not be opened
	if (!previousFile.open(QFile::ReadOnly)) {

		QString message(tr("File: "));
		message += previousFile.fileName();
		message += tr(" couldn't be opened for reading");
		emit errorMessage(message);
		return;
	}

	QTextStream stream(&previousFile);

	bool readingDeclarations = false;
	bool readingAssignments = false;

	// read as long as theres stuff in the stream to read.
	while (!stream.atEnd()) {
		QString line = stream.readLine();

		QString lineCompare = line.trimmed();

		// if next line starts the declarations
		if (lineCompare == BLACK_BOX_DECL_START) {
			readingDeclarations = true;
		}
		// if previous line was the last one of the declarations
		else if (lineCompare == BLACK_BOX_DECL_END) {
			readingDeclarations = false;
		}
		// if the next line starts the assignments
		else if (lineCompare == BLACK_BOX_ASSIGN_START) {
			readingAssignments = true;
		}
		// if previous line was the last of the assignments
		else if (lineCompare == BLACK_BOX_ASSIGN_END) {
			readingAssignments = false;
		}
		// if the line is part of the declarations
		else if (readingDeclarations) {
			// add the line to the declarations
			userModifiedDeclarations_ += line;
			userModifiedDeclarations_ += QString("\n");
		}
		// if the line is part of the assignments
		else if (readingAssignments) {
			// add the line to the assignments
			userModifiedAssignments_ += line;
			userModifiedAssignments_ += QString("\n");
		}
	}

	previousFile.close();
}

void VhdlGenerator2::writeUserModifiedDeclarations( QTextStream& stream ) {
	stream << "  " <<"-- You can write vhdl code after this tag and it is saved through the generator." << endl;
	stream << "  " << BLACK_BOX_DECL_START << endl;
	stream << userModifiedDeclarations_;
	stream << "  " << BLACK_BOX_DECL_END << endl;
	stream << "  " << "-- Do not write your code after this tag." << endl << endl;
}

void VhdlGenerator2::writeUserModifiedAssignments( QTextStream& stream ) {
	stream << "  " << "-- You can write vhdl code after this tag and it is saved through the generator." << endl;
	stream << "  " << BLACK_BOX_ASSIGN_START << endl;
	stream << userModifiedAssignments_;
	stream << "  " << BLACK_BOX_ASSIGN_END << endl;
	stream << "  " << "-- Do not write your code after this tag." << endl << endl;
}

VhdlGenerator2::PortConnection::PortConnection( 
	QSharedPointer<VhdlComponentInstance> instance,
	const QString& portName,
	int left /*= -1*/, 
	int right /*= -1*/ ):
instance_(instance),
portName_(portName),
leftBound_(left),
rightBound_(right) {
}

VhdlGenerator2::PortConnection::PortConnection( const VhdlGenerator2::PortConnection& other ):
instance_(other.instance_),
portName_(other.portName_),
leftBound_(other.leftBound_),
rightBound_(other.rightBound_) {
}

VhdlGenerator2::PortConnection& VhdlGenerator2::PortConnection::operator=(
	const VhdlGenerator2::PortConnection& other ) {
		if (this != &other) {
			instance_ = other.instance_;
			portName_ = other.portName_;
			leftBound_ = other.leftBound_;
			rightBound_ = other.rightBound_;
		}
		return *this;
}
