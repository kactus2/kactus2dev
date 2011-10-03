/* 
 *  	Created on: 30.12.2010
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator.cpp
 *		Project: Kactus 2
 */

#include "vhdlgenerator.h"
#include "componenttype.h"
#include "componentinstance.h"
#include "connection.h"

#include <exceptions/vhdl_error.h>
#include <exceptions/invalid_file.h>

#include <models/generaldeclarations.h>
#include <models/businterface.h>
#include <models/component.h>
#include <models/librarycomponent.h>
#include <models/port.h>
#include <models/fileset.h>
#include <models/file.h>
#include <models/view.h>
#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QDate>
#include <QTime>

#include <QDebug>

VhdlGenerator::VhdlGenerator(QSharedPointer<Component> topLevelComponent, 
							 const QString& viewName,
							 LibraryInterface* libraryHandler,
							 QWidget* parent): 
QObject(parent),
libraryHandler_(libraryHandler),
parent_(parent),
design_(),
component_(topLevelComponent),
desConf_(),
viewName_(viewName),
topLevelEntity_(),
libraries_(),
generics_(), 
signals_(),
components_(),
instances_(),
componentModels_(),
ports_() {

	Q_ASSERT_X(topLevelComponent, "VhdlGenerator constructor",
		"Null top level component pointer given to constructor");

	Q_ASSERT_X(libraryHandler, "VhdlGenerator constructor",
			"Null LibraryHandler pointer given to constructor");

	Q_ASSERT_X(!viewName.isEmpty(), "VhdlGenerator constructor",
			"Empty view name given to constructor");

	View* view = component_->findView(viewName);
	
	// make sure view exists
	if (!view) {
		emit errorMessage(tr("View %1 was not found within top component.").arg(
			viewName));
		return;
	}
	// view must be hierarchical
	else if (!view->isHierarchical()) {
		emit errorMessage(tr("View %1 was found but does not contain a "
			"hierarchical reference").arg(viewName_));
		return;
	}

	VLNV hierRef = view->getHierarchyRef();
	
	// if the referenced document is not included in library
	if (!libraryHandler_->contains(hierRef)) {
		emit errorMessage(tr("Referenced document %1:%2:%3:%4 was not found "
			"within library.").arg(
			hierRef.getVendor()).arg(
			hierRef.getLibrary()).arg(
			hierRef.getName()).arg(
			hierRef.getVersion()));
		return;
	}

	// check the type from library
	hierRef.setType(libraryHandler_->getDocumentType(hierRef));

	if (hierRef.getType() == VLNV::DESIGN) {
	
		// parse the design and take ownership of design
		QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(hierRef);
		design_ = libComp.staticCast<Design>();
	}
	else if (hierRef.getType() == VLNV::DESIGNCONFIGURATION) {

		// parse the design configuration 
		QSharedPointer<LibraryComponent> libComp = libraryHandler_->getModel(hierRef);
		desConf_ = libComp.staticCast<DesignConfiguration>();

		// get reference to the design
		VLNV designRef = desConf_->getDesignRef();

		// if the design is not found within libary
		if (!libraryHandler_->contains(designRef)) {
			emit errorMessage(tr("Referenced view %1:%2:%3:%4 was not found "
				"within library.").arg(
				designRef.getVendor()).arg(
				designRef.getLibrary()).arg(
				designRef.getName()).arg(
				designRef.getVersion()));
			return;
		}
		// make sure the document is a design
		designRef.setType(libraryHandler_->getDocumentType(designRef));
		if (designRef.getType() != VLNV::DESIGN) {
			emit errorMessage(tr("Referenced document %1:%2:%3:%4 is not a "
				"design.").arg(
				designRef.getVendor()).arg(
				designRef.getLibrary()).arg(
				designRef.getName()).arg(
				designRef.getVersion()));
			return;
		}

		// parse the design and save the pointer
		QSharedPointer<LibraryComponent> libDes = libraryHandler_->getModel(designRef);
		design_ = libDes.staticCast<Design>();
	}
	// document was completely wrong type
	else {
		emit errorMessage(tr("Referenced document %1:%2:%3:%4 is neither "
			"design or design configuration.").arg(
			hierRef.getVendor()).arg(
			hierRef.getLibrary()).arg(
			hierRef.getName()).arg(
			hierRef.getVersion()));
		return;
	}
}

VhdlGenerator::~VhdlGenerator() {
	generics_.clear();
	//ports_.clear();
	signals_.clear();
	//signalMaps_.clear();
	components_.clear();
	instances_.clear();
	componentModels_.clear();
}


void VhdlGenerator::parse() {
	// the name of the top-level entity is the name of the top-level component
	topLevelEntity_ = component_->getVlnv()->getName().remove(QString(".comp"));

	// parse the generics of a top entity
	parseTopGenerics();

	// get the component instances from the design
	parseComponentInstances();

	// parse the interconnections of a design
	parseInterConnections();

	// parse the ad hoc connections of a design
	parseAdHocConnections();

	// parse the hier connections of a design
	parseHierConnections();

	// create mappings between ports and signals
	mapPorts2Signals();
}

void VhdlGenerator::generateVhdl(const QString& outputFileName) {

	if (!libraryHandler_ || !component_ || !design_ || outputFileName.isEmpty()) {
		return;
	}

	QFile file(outputFileName);

	// if previous file exists then remove it.
	if (file.exists()) {
		file.remove();
	}

	// open the file and erase all old contents if any exists
	// if file could not be opened
	if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {

		// throw an exception that file couldn't be opened
		QString message(tr("File: "));
		message += outputFileName;
		message += tr(" couldn't be opened for writing");
		emit errorMessage(message);
		return;
	}
	// now file has been opened for writing

	// open file stream to write to
	QTextStream vhdlStream(&file);

	// write header comments of the file
	writeVhdlHeader(vhdlStream, outputFileName);

	vhdlStream << endl;

	// always add IEEE library
	vhdlStream << "library IEEE;" << endl;
	vhdlStream << "use IEEE.std_logic_1164.all;" << endl;
	vhdlStream << "use ieee.numeric_std.all;" << endl << endl;

	// declare the libraries used.
	foreach (QString library, libraries_) {
		vhdlStream << "library " << library <<";" << endl;
		vhdlStream << "use " << library << ".all;" << endl;
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

	// write the architecture of the entity
	vhdlStream << "architecture structural of " << topLevelEntity_ << " is"
			<< endl << endl;

	// write declarations for signals connecting the ports
	writeSignalDeclarations(vhdlStream);

	// write all component declarations
	writeComponentDeclarations(vhdlStream);

	// start writing architecture component instances
	vhdlStream << endl << "begin" << endl << endl;

	// write signal mappings between signals and top-level ports
	writeSignalMaps(vhdlStream);
	vhdlStream << endl;

	// write the component instances
	writeComponentInstances(vhdlStream);

	vhdlStream << "end structural;" << endl << endl;
	return;
}

bool VhdlGenerator::addRtlView(const QString vhdlFilePath) {

	// ipDir represents the directory where the IP-Xact file is located in.
	QString ipDir(libraryHandler_->getPath(*component_->getVlnv()));
	if (ipDir.isEmpty()) {
		emit errorMessage(tr("Path to top-component was not found."));
		return false;
	}

	// get the relative path to add to file set
	QString relativePath = General::getRelativePath(ipDir, vhdlFilePath);
	if (relativePath.isEmpty()) {
		emit errorMessage(tr("Could not create relative path to vhdl file."));
		return false;
	}

	QString fileSetName("vhdlSource");
	FileSet* topFileSet = component_->getFileSet(fileSetName);

	// if the top vhdl file set was not found. Create one
	if (!topFileSet) {
		topFileSet = new FileSet(fileSetName, QString("sourceFiles"));
		component_->addFileSet(topFileSet);
	}

	// create a new file
	File* topVhdlFile = new File(relativePath, topFileSet);
	topVhdlFile->addFileType(QString("vhdlSource"));
	topVhdlFile->setIncludeFile(true);
	topVhdlFile->setLogicalName(component_->getVlnv()->getLibrary());
	topVhdlFile->setCommand(QString("vcom"));
	topVhdlFile->setBuildFlags(QString("-quiet -check_synthesis -work work"), true);

	// add the new file to the file set
	topFileSet->addFile(topVhdlFile);

	// find a rtl view from the component
	QString viewName("rtl");
	View* rtlView = new View(viewName);

	// add the spirit:envIdentifier. Only language is defined, not tool
	// specific settings
	rtlView->addEnvIdentifier(QString("VHDL::"));

	// set the language of the view.
	rtlView->setLanguage(QString("vhdl"));

	// set the model name to be the top_level architecture of the top-level
	// entity
	QString topEntity(topLevelEntity_);
	topEntity += QString("(top_level)");
	rtlView->setModelName(topEntity);

	// set a reference to a file set
	rtlView->addFileSetRef(fileSetName);

	// add the view to the component's model
	component_->addView(rtlView);

	// find the active view used to generate the vhdl
	View* activeView = component_->findView(viewName_);
	if (!activeView) {
		emit errorMessage(tr("The active view %1 was not found in top component").arg(
			viewName_));
		return false;
	}

	activeView->setTopLevelView(rtlView->getName());

	return true;
}

void VhdlGenerator::writeComponentInstances(QTextStream& stream) {
	for (QMap<QString, QSharedPointer<ComponentInstance> >::iterator i = instances_.begin();
			i != instances_.end(); ++i) {
		i.value()->generateInstanceVhdl(stream);
		stream << endl;
	}
}

void VhdlGenerator::writeSignalDeclarations(QTextStream& stream) {
	
	// Each signal is found at least twice to list of signals that have already
	// been declared is needed
	QStringList writtenSignals;

	// write all signals
	for (QMap<Connection::EndPoint, QSharedPointer<Signal> >::iterator i =
			signals_.begin(); i != signals_.end(); ++i) {

		// if signal has already been declared once
		if (writtenSignals.contains(i.value()->getName()))
			continue;
		else {
			// tell signal to write itself
			i.value()->writeSignalVhdl(stream);
			writtenSignals.append(i.value()->getName());
		}
	}

	stream << endl;
}

void VhdlGenerator::writeComponentDeclarations(QTextStream& stream) {
	for (QMap<QString, QSharedPointer<ComponentType> >::iterator i = components_.begin();
			i != components_.end(); ++i) {
		i.value()->generateComponentVhdl(stream);
		stream << endl;
	}
	stream << endl;
}

void VhdlGenerator::writeGenerics(QTextStream& vhdlStream) {

	// if generics exist
	if (!generics_.isEmpty()) {
        
		// the start tag
		vhdlStream << "\tgeneric (" << endl;

		// write each generic
		for (QMap<QString, QSharedPointer<ComponentType::VhdlGeneric> >::iterator i =
				generics_.begin(); i != generics_.end(); ++i) {
			vhdlStream << "\t\t" << i.key() << " : " << i.value()->type_;

			// if default value is defined
			if (!i.value()->defaultValue_.isEmpty()) {
                QString type = i.value()->type_;
                QString value = i.value()->defaultValue_;
                vhdlStream << " := " << quoteGenericValue(type, value);
			}

			// if the last generic
			if (i == --generics_.end()) {
				vhdlStream << ");" << endl;
			}
			// if theres more generics left
			else {
				vhdlStream << ";" << endl;
			}
		}
	}
}

void VhdlGenerator::writePorts(QTextStream& vhdlStream) {
	
	// if ports exist
	if (!ports_.isEmpty()) {

		vhdlStream << "\tport (" << endl;

		// print each port
		for (QMap<QString, QSharedPointer<Connection> >::iterator i = ports_.begin();
				i != ports_.end(); ++i) {

			// write the port name and direction
			vhdlStream << "\t\t" << i.key() << " : " <<
					General::direction2Str(i.value()->getDirection());

			// write the port type
			vhdlStream << " ";
			i.value()->writeType(vhdlStream);

			// if the last port is written
			if ( i == --ports_.end()) {
				vhdlStream << ");" << endl;
			}
			// if theres more ports left
			else {
				vhdlStream << ";" << endl;
			}
		}
		// write extra empty line to make code readable
		vhdlStream << endl;
		return;

	}
	
}

QMap<QString, General::PortMap*> VhdlGenerator::getInterfacePortMaps(
		BusInterface* interface) {

	// the map to return
	QMap<QString, General::PortMap*> portMaps;

	// list of all portMaps on interface 1
	QList<QSharedPointer<General::PortMap> > interfacePorts =
			interface->getPortMaps();

	// take each portmap in interface 1
	for (int i = 0; i < interfacePorts.size(); ++i) {

		// add the port map to map with logical name as key
		portMaps.insert(interfacePorts.at(i)->logicalPort_,
				interfacePorts.at(i).data());
	}
	return portMaps;
}

void VhdlGenerator::writeSignalMaps(QTextStream& stream) {
	
	for (QMap<Connection::EndPoint, QSharedPointer<Signal> >::iterator i = signals_.begin();
		i != signals_.end(); ++i) {

		// if endpoint is some other than top level port
		if (i.key().instanceName_ != topLevelEntity_)
			continue;

		stream << "\t";

		// if signal is connected to ouput port
		if (component_->getPortDirection(i.key().portName_) == General::OUT) {

			stream << i.key().portName_;
			if (i.key().portLeft_ != 0 || i.key().portRight_ != 0) {
				stream << "(" << i.key().portLeft_ << " downto "
				<< i.key().portRight_ << ")";
			}
			stream << " <= ";
			stream << i.value()->getName();
			if (i.key().signalLeft_ != 0 || i.key().signalRight_ != 0) {
				stream << "(" << i.key().signalLeft_ << 
				" downto " << i.key().signalRight_ << ")";
			}
		}
		// if signal is connected to input or inout port
		else {
			stream << i.value()->getName();
			if (i.key().signalLeft_ != 0 || i.key().signalRight_) {
				stream << "(" << i.key().signalLeft_ << " downto " <<
					i.key().signalRight_ << ")";
			}
			stream << " <= ";
			stream << i.key().portName_;
			if (i.key().portLeft_ != 0 || i.key().portRight_ != 0) {
				stream << "(" << i.key().portLeft_ << " downto " <<
					i.key().portRight_ << ")";
			}
		}
		stream << ";" << endl;
	}	
}

void VhdlGenerator::writeVhdlHeader(QTextStream& stream,
		const QString fileName) {
	stream << "-- ****************************************************" << endl;
	stream << tr("-- ** File: ") << fileName << endl;
	stream << tr("-- ** Date: ") <<
			QDate::currentDate().toString(QString("dd.MM.yyyy")) << " " <<
			QTime::currentTime().toString(QString("hh:mm:ss")) << endl;
	stream << tr("-- ** Description: ") << design_->getDescription() << endl;
	stream << "-- **" << endl;
	stream << tr("-- ** This file was generated by Kactus2 vhdl generator")
					<< endl;
	stream << "-- **" << endl;
	stream << "-- ****************************************************" << endl;
	return;
}

void VhdlGenerator::parseComponentInstances() {
	QList<Design::ComponentInstance> instances =
			design_->getComponentInstances();

	for (int i = 0; i < instances.size(); ++i) {

		// get pointer to the component that is references by component
		// instance
		QSharedPointer<LibraryComponent> libcomponent = libraryHandler_->getModel(
				instances.at(i).componentRef);

		// if component was not found
		if (!libcomponent) {
			emit errorMessage(tr("Component %1:%2:%3:%4 was not found within "
				"library").arg(
				instances.at(i).componentRef.getVendor()).arg(
				instances.at(i).componentRef.getLibrary()).arg(
				instances.at(i).componentRef.getName()).arg(
				instances.at(i).componentRef.getVersion()));
			continue;
		}

		QSharedPointer<Component> component = libcomponent.staticCast<Component>();

		// if the vlnv didn't belong to a component
		if (!component) {
			emit errorMessage(tr("The VLNV %1:%2:%3:%4 did not belong to a "
				"component").arg(
				instances.at(i).componentRef.getVendor()).arg(
				instances.at(i).componentRef.getLibrary()).arg(
				instances.at(i).componentRef.getName()).arg(
				instances.at(i).componentRef.getVersion()));
			continue;
		}

		// create a Component declaration matching the component
		ComponentType* compDeclaration = new ComponentType(component, this);

		// if the component name is already taken
		if (components_.contains(compDeclaration->getType())) {

			// if component name is taken but there is no component with the
			// same VLNV it means that there is more than one different
			// component with same name
			if (!componentModels_.contains(*component->getVlnv())) {
				emit errorMessage(tr("Several different entities with same name"));
				continue;
			}

		}
		else {
			// if component declaration doesn't exist, insert it
			components_.insert(compDeclaration->getType(),
					QSharedPointer<ComponentType>(compDeclaration));

			// add the component to the list of component models
			componentModels_.insert(*component->getVlnv(),
				QSharedPointer<Component>(component));
		}

		VhdlGenerator::createInstance(instances.at(i).instanceName,	compDeclaration, 
			instances.at(i), component);

		// if design configuration is used and it has active view for given 
		// component instance
		if (desConf_ && desConf_->hasActiveView(instances.at(i).instanceName)) {
			parseLibraries(component, desConf_->getActiveView(instances.at(i).instanceName));
		}
		// if no active view is specified
		else {
			parseLibraries(component, QString());
		}

	}
}

void VhdlGenerator::createInstance(const QString& instanceName,
		ComponentType* compDeclaration,
		const Design::ComponentInstance& instance,
		QSharedPointer<Component> component) {

	// if component instance with the same name already exists
	if (instances_.contains(instanceName)) {
		emit errorMessage(tr("Two component instances with the same name"));
		return;
	}

	// create component instance to be used for generating vhdl
	ComponentInstance* vhdlInstance = new ComponentInstance(instanceName,
			compDeclaration->getInstanceType(), component, compDeclaration, this);
	connect(vhdlInstance, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(vhdlInstance, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	// map generics that are defined in design and are defined in this
	// component instance
	QMap<QString, QString> confElements = instance.configurableElementValues;
	for (QMap<QString, QString>::iterator i = confElements.begin();
			i != confElements.end(); ++i) {

		// if the generic is found in the component type
		if (compDeclaration->hasGeneric(i.key())) {

            QString type = compDeclaration->genericType(i.key());
            QString value = i.value();
			// add the mapping of the generic to the instance
			vhdlInstance->setGenericMap(
                i.key(), quoteGenericValue(type, value));
		}
	}

	// insert the component instance to the map
	instances_.insert(instanceName,
			QSharedPointer<ComponentInstance>(vhdlInstance));
}

void VhdlGenerator::parseInterConnections() {
	QList<Design::Interconnection> interconnections =
			design_->getInterconnections();

	// go through all interConnections
	for (int i = 0; i < interconnections.size(); ++i) {

		// get pointer to the first component instance
		ComponentInstance* comp1 = instances_.value(
				interconnections.at(i).interface1.componentRef).data();

		// get pointer to the busInterface of first component instance
		BusInterface* interface1 = comp1->getComponent()->getBusInterface(
				interconnections.at(i).interface1.busRef);
		
		// get pointer to the second component instance
		ComponentInstance* comp2 = instances_.value(
				interconnections.at(i).interface2.componentRef).data();

		// get pointer to the busInterface of second component instance
		BusInterface* interface2 = comp2->getComponent()->getBusInterface(
				interconnections.at(i).interface2.busRef);

		// connect the two interfaces together
		connectInterfaces(QString(""), comp1, interface1,
				interface2, comp2);
	}
}

void VhdlGenerator::parseAdHocConnections() {

	// all adHoc connections are processed
	QList<Design::AdHocConnection> adHocs = design_->getAdHocConnections();
	foreach (Design::AdHocConnection adHoc, adHocs) {

		// find the boundaries for the signal
		int left = adHoc.internalPortReferences.first().left;
		int right = 0;
		foreach (Design::PortRef portRef, adHoc.internalPortReferences) {
			left = qMax(left, portRef.left);
		}
		foreach (Design::PortRef portRef, adHoc.externalPortReferences) {
			left = qMax(left, portRef.left);
		}
		// if no bound was specified
		if (left < 0)
			left = 0;

		// create the endPoints
		QList<Connection::EndPoint> endPoints;
		foreach (Design::PortRef portRef, adHoc.internalPortReferences) {
			
			// create the end point
			Connection::EndPoint endPoint(portRef.componentRef, portRef.portRef);
			endPoint.portLeft_ = portRef.left;
			endPoint.signalLeft_ = portRef.left;
			endPoint.portRight_ = portRef.right;
			endPoint.signalRight_ = portRef.right;
			
			// add end point to list
			endPoints.append(endPoint);
		}
		foreach (Design::PortRef portRef, adHoc.externalPortReferences) {
			// create the end point
			Connection::EndPoint endPoint(portRef.componentRef, portRef.portRef);
			endPoint.portLeft_ = portRef.left;
			endPoint.signalLeft_ = portRef.left;
			endPoint.portRight_ = portRef.right;
			endPoint.signalRight_ = portRef.right;

			// add end point to list
			endPoints.append(endPoint);
		}

		// create the signal that connects the end points
		QSharedPointer<Signal> signal;

		// find a signal to use for connecting the end points
		foreach (Connection::EndPoint endPoint, endPoints) {
			if (signals_.contains(endPoint)) {
				signal = signals_.value(endPoint);
				signal->setBounds(qMax(left, int(signal->getLeft())),
					qMin(right, int(signal->getRight())));
				break;
			}
		}
		// if signal was not found then create a new one 
		if (!signal) {
			signal = QSharedPointer<Signal>(new Connection(adHoc.name, this));
			signal->setBounds(left, right);
		}

		// connect each end point to given signal
		foreach (Connection::EndPoint endPoint, endPoints) {
			connectEndPoint(endPoint, signal);
		}
	}
}

void VhdlGenerator::parseHierConnections() {

	// search all hierConnections within design
	QList<Design::HierConnection> hierConnections =
			design_->getHierarchicalConnections();
	for (int i = 0; i < hierConnections.size(); ++i) {

		// find the top-level bus interface
		BusInterface* topInterface =
				component_->getBusInterface(hierConnections.at(i).interfaceRef);

		// if the top level interface couldn't be found
		if (!topInterface) {
			throw Vhdl_error(tr("Top level interface defined in hierConnections"
					" couldn't be found in top-level component"));
		}

		// find the component instance for this hierConnection
		ComponentInstance* instance =
				instances_.value(
						hierConnections.at(i).interface_.componentRef).data();

		// if the component instance couldn't be found
		if (!instance) {
			throw Vhdl_error(tr("Component instance defined in hierConenction"
					" couldn't be found within this design"));
		}

		// find the bus interface
		BusInterface* instanceInterface =
				instance->getComponent()->getBusInterface(
						hierConnections.at(i).interface_.busRef);
		// if bus interface couldn't be found
		if (!instanceInterface) {
			throw Vhdl_error(tr("Bus interface defined in hierConnection "
					"couldn't be found within defined component description"));
		}

		connectInterfaces(QString("out"), instance, instanceInterface,
				topInterface);
	}
}

void VhdlGenerator::connectInterfaces(const QString connectionName,
		ComponentInstance* instance1, BusInterface* interface1,
		BusInterface* interface2, ComponentInstance* instance2) {

	// make sure valid pointers are always given
	Q_ASSERT_X(instance1, "VhdlGenerator::connectInterfaces()",
			"Null ComponentInstance pointer given for instance 1");
	Q_ASSERT_X(interface1, "VhdlGenerator::connectInterfaces()",
			"Null BusInterface pointer given for interface 1");
	Q_ASSERT_X(interface2, "VhdlGenerator::connectInterfaces()",
			"Null BusInterface pointer given for interface 2");

	// first fetch all portMaps of interface 1
	QMap<QString, General::PortMap*> interface1Maps =
			getInterfacePortMaps(interface1);

	// then fetch all portMaps of interface 2
	QMap<QString, General::PortMap*> interface2Maps =
			getInterfacePortMaps(interface2);

	// go through all port maps in interface 1
	for (QMap<QString, General::PortMap*>::iterator i = interface1Maps.begin();
			i != interface1Maps.end(); ++i) {

		// if the port in instance 1 is phantom then don't add this to vhdl
		// or if the port does not exist
		if (!instance1->getComponent()->getPort(i.value()->physicalPort_) ||
			instance1->getComponent()->getPort(
				i.value()->physicalPort_)->getDirection() ==
						General::DIRECTION_PHANTOM) {
			emit noticeMessage(tr("Port %1 was not found or was phantom, skipping...").arg(
				i.value()->physicalPort_));
			continue;
		}

		// if the same logical port is mapped in both interfaces
		if (interface2Maps.contains(i.key())) {

			// pointer to the port map in interface 2
			General::PortMap* int2PortMap = interface2Maps.value(i.key());

			// if this is not hierarchical connection
			if (instance2) {

				// find the port in instance 2
				Port* port = instance2->getComponent()->getPort(
						int2PortMap->physicalPort_);

				// if the port direction is phantom
				if (port && port->getDirection() ==
						General::DIRECTION_PHANTOM) {
					emit noticeMessage(tr("Port %1 was phantom, skipping...").arg(
						port->getName()));
					continue;
				}
			}

			// if instance2 was not found then this is hierarchical connection

			// if the physical port is not defined in the containing component
			else if (!component_->hasPort(int2PortMap->physicalPort_)) {
				emit errorMessage(tr("Port %1 is not defined in top-component").arg(
					int2PortMap->physicalPort_));
				continue;
			}
			// if this is hierarchical connection and the port direction in
			// top component is type phantom
			else if (component_->getPort(
					int2PortMap->physicalPort_)->getDirection() ==
							General::DIRECTION_PHANTOM) {
				emit noticeMessage(
					tr("Port %1 was defined as phantom, skipping...").arg(
					int2PortMap->physicalPort_));
				continue;
			}


			// create a descriptive name for the new signal
			QString signalName = createSignalName(connectionName,
				instance1->getName(),
				i.value()->physicalPort_, instance2,
				int2PortMap->physicalPort_);

			// the width of the signal to be created to connect the ports
			unsigned int width = 0;

			// if both instances have been identified
			if (instance2) {

				// if either instance has port which is not defined 
				if (!instance1->getComponent()->getPort(i.value()->physicalPort_)) {
					QString msg = tr("Port %1 not found within component %2").arg(
						i.value()->physicalPort_).arg(
						instance1->getComponent()->getVlnv()->getName());
					emit errorMessage(msg);
					continue;
				}
				else if (!instance2->getComponent()->getPort(int2PortMap->physicalPort_)) {
					QString msg = tr("Port %1 not found within component %2").arg(
						int2PortMap->physicalPort_).arg(
						instance2->getComponent()->getVlnv()->getName());
					emit errorMessage(msg);
					continue;
				}

				// the smaller port defines the width
				width = qMin(instance1->getComponent()->getPort(i.value()->physicalPort_)->getPortSize(),
						instance2->getComponent()->getPort(int2PortMap->physicalPort_)->getPortSize());
			}
			// if the instance2 is not defined then this is hierarchical
			// connection and the signal width is defined by the other
			// subcomponent and the top-level port
			else {
					
				// the larger port defines the width of the port
				width = qMax(instance1->getComponent()->getPort(
						i.value()->physicalPort_)->getPortSize(),
						component_->getPort(
								int2PortMap->physicalPort_)->getPortSize());
			}

			// check if there is already a signal that can be used to connect the
			// ports to each other.

			// create end points for both ports
			Connection::EndPoint endPoint1(instance1->getName(), i.value()->physicalPort_);
			// set the slice for signal
			if (i.value()->logicalVector_) {
				endPoint1.signalLeft_ = i.value()->logicalVector_->getLeft();
				endPoint1.signalRight_ = i.value()->logicalVector_->getRight();
			}
			// set the slice for port
			if (i.value()->physicalVector_) {
				endPoint1.portLeft_ = i.value()->physicalVector_->getLeft();
				endPoint1.portRight_ = i.value()->physicalVector_->getRight();
			}

			instance1->createPortDeclaration(endPoint1);

			Connection::EndPoint endPoint2;
			endPoint2.portName_ = int2PortMap->physicalPort_;
			// set slice for signal
			if (int2PortMap->logicalVector_) {
				endPoint2.signalLeft_ = int2PortMap->logicalVector_->getLeft();
				endPoint2.signalRight_ = int2PortMap->logicalVector_->getRight();
			}
			// set slice for port
			if (int2PortMap->physicalVector_) {
				endPoint2.portLeft_ = int2PortMap->physicalVector_->getLeft();
				endPoint2.portRight_ = int2PortMap->physicalVector_->getRight();
			}

			// if normal connection
			if (instance2) {
				endPoint2.instanceName_ = instance2->getName();
				instance2->createPortDeclaration(endPoint2);
			}
			// if hierarchical connection
			else {
				endPoint2.instanceName_ = topLevelEntity_;
				
				// find the port from the component
				Port* portModel = component_->getPort(endPoint2.portName_);
				if (!portModel) {
					emit errorMessage(tr("Port %1 not found within top component").arg(
						endPoint2.portName_));
					continue;
				}
				// get the left and right bound of the port
				int left = portModel->getLeftBound();
				int right = portModel->getRightBound();

				// create a port for the top component
				Connection* port = new Connection(endPoint2.portName_, 
					component_->getPortDirection(endPoint2.portName_),
					left, right);
				connect(port, SIGNAL(errorMessage(const QString&)),
					this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
				connect(port, SIGNAL(noticeMessage(const QString&)),
					this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
				ports_.insert(endPoint2.portName_, QSharedPointer<Connection>(port));
			}

			// if both endpoints are already associated with a signal
			if (signals_.contains(endPoint1) && signals_.contains(endPoint2)) {

				// adjust the signal width if necessary
				int width = qMax(signals_.value(endPoint1)->getSize(),
					signals_.value(endPoint2)->getSize());
				signals_.value(endPoint1)->setBounds(width-1, 0);

				QSharedPointer<Signal> oldSignal = signals_.value(endPoint1);
				// get all end points associated with signal for endpoint 2
				QList<Connection::EndPoint> endPoints = signals_.keys(oldSignal);
				// replace each end point with association to signal for endpoint 1
				foreach (Connection::EndPoint temp, endPoints) {
					signals_.insert(temp, signals_.value(endPoint1));
				}

				// remove the signal that no longer has any end points associated
				// with it.
				oldSignal.clear();
			}
			// if neither port has a signal associated with it
			else if (!signals_.contains(endPoint1) && !signals_.contains(endPoint2)) {

				// create a signal and associate it with the ports
				QSharedPointer<Signal> signal = QSharedPointer<Signal>(new Signal(signalName, this));
				signal->setBounds(width-1, 0);
				signals_.insert(endPoint1, signal);
				signals_.insert(endPoint2, signal);
			}
			// if one port has signal associated with it
			else {
				
				// if endPoint1 has association
				if (signals_.contains(endPoint1)) {

					// adjust the signal width if necessary
					int newWidth = qMax(signals_.value(endPoint1)->getSize(), width);
					signals_.value(endPoint1)->setBounds(newWidth-1, 0);

					// add association also for endpoint 2
					signals_.insert(endPoint2, signals_.value(endPoint1));
				}
				// if endPoint2 has association
				else {
					// adjust the signal width if necessary
					int newWidth = qMax(signals_.value(endPoint2)->getSize(), width);
					signals_.value(endPoint2)->setBounds(newWidth-1, 0);

					// add association also for endpoint 1
					signals_.insert(endPoint1, signals_.value(endPoint2));
				}
			}
		}
	}
	// all port logical signals found in both interfaces are connected.
}

void VhdlGenerator::getLibraries() {

	// list of vlnvs that are needed by the design
	QList<VLNV> neededVLNVs;

	// if a design was specified
	if (design_) {

		// ask library handler for the list of VLNVs needed by the design and
		// its subcomponents
		libraryHandler_->getNeededVLNVs(*design_->getVlnv(), neededVLNVs);
	}

	// go through each vlnv
	for (int i = 0; i < neededVLNVs.size(); ++i) {

		// if the VLNV is for a component
		if (neededVLNVs.at(i).getType() == VLNV::COMPONENT) {

			// parse a model that matches the vlnv
			QSharedPointer<LibraryComponent> libComponent =
					libraryHandler_->getModel(neededVLNVs.at(i));
			if (!libComponent) {
				continue;
			}

			// convert the pointer to component
			QSharedPointer<Component> component = libComponent.staticCast<Component>();
			if (!component) {
				continue;
			}

			// ask the component for it's vhdl libraries
			QList<General::LibraryFilePair> componentFiles =
					component->getVhdlLibraries();

			// search all component files
			for (int i = 0; i < componentFiles.size(); ++i) {

				// if the library is not yet added to the list of libraries
				if (!libraries_.contains(componentFiles.at(i).libraryName_)) {

					// add the library name to the list of libraries
					libraries_.append(componentFiles.at(i).libraryName_);
				}
			}
		}
	}
	// the files needed by the design's subcomponents are now added

	// ask the top component for its vhdl files and their libraries
	QList<General::LibraryFilePair> tempFiles = component_->getVhdlLibraries();

	// search all component files
	for (int i = 0; i < tempFiles.size(); ++i) {

		// if the library is not yet added to the list of libraries
		if (!libraries_.contains(tempFiles.at(i).libraryName_)) {

			// add the library name to the list of libraries
			libraries_.append(tempFiles.at(i).libraryName_);
		}
	}
}

QString VhdlGenerator::createSignalName(const QString connectionName,
		const QString instance1Name,
		const QString instance1Port, ComponentInstance* instance2,
		const QString instance2Port) {

	// generate a signal name for signal connecting the physical ports
	// in interface 1 and interface 2
	QString signalName(connectionName);

	if (!signalName.isEmpty())
		signalName += QString("_");

	signalName += instance1Name;
	signalName += QString("_");
	signalName += instance1Port;
	signalName += QString("_");

	// if instance 2 is defined
	if (instance2) {
		signalName += instance2->getName();
		signalName += QString("_");
		signalName += instance2Port;
	}
	// if instance 2 was not defined (interface2 is top-level interface)
	else {
		signalName += QString("top_");
		signalName += instance2Port;
	}

	return signalName;
}

void VhdlGenerator::mapPorts2Signals() {

	// search each endpoint
	for (QMap<Connection::EndPoint, QSharedPointer<Signal> >::iterator i = signals_.begin(); 
		i != signals_.end(); ++i) {

		// if the component instance can't be found and endpoint is not for top
		// component
		if (!instances_.contains(i.key().instanceName_) && 
			topLevelEntity_ != i.key().instanceName_) {
			emit errorMessage(tr("%1 was not found within component instances "
				"or top-level entity").arg(i.key().instanceName_));
			continue;
		}
		// if end point is for top component.
		else if (i.key().instanceName_ == topLevelEntity_) {
			// dont do mapping for top ports yet, it is done later
			continue;
		}

		// if mapping between instance port and signal
		ComponentInstance* instance = instances_.value(i.key().instanceName_).data();
		if (!instance) {
			emit errorMessage(tr("Component instance %1 was not found").arg(
				i.key().instanceName_));
			continue;
		}

		instance->createPortMap(i.value()->getName(), i.key().signalLeft_, i.key().signalRight_,
			i.key().portName_, i.key().portLeft_, i.key().portRight_);		
	}

}

void VhdlGenerator::parseTopGenerics() {
	const QMap<QString, QSharedPointer<ModelParameter> >& modelParameters = component_->getModelParameters();

	// go through all generics
	for (QMap<QString, QSharedPointer<ModelParameter> >::const_iterator i = 
		modelParameters.begin(); i != modelParameters.end(); ++i) {

        QString value = i.value()->getValue();
        QString type = i.value()->getDataType();
        QString genericValue = quoteGenericValue(type, value);
        // create generic and add it to the map
        ComponentType::VhdlGeneric* vhdlGeneric = new ComponentType::VhdlGeneric(
            i.value()->getName(), i.value()->getDataType(), genericValue);
        generics_.insert(vhdlGeneric->name_, QSharedPointer<ComponentType::VhdlGeneric>(vhdlGeneric));
	}
}

void VhdlGenerator::parseLibraries( QSharedPointer<Component> component, const QString& viewName ) {

	Q_ASSERT_X(component, "VhdlGenerator::parseLibraries",
		"Null component pointer given as parameter.");

	QList<General::LibraryFilePair> libraries;

	// if view is specified
	if (!viewName.isEmpty()) {
		libraries += component->getLibraries(viewName);
	}
	// no view is specified, use all libraries of component
	else {
		libraries += component->getVhdlLibraries();
		libraries += component->getVerilogLibraries();
	}

	foreach (General::LibraryFilePair pair, libraries) {

		// if library is not yet found on the list
		if (!libraries_.contains(pair.libraryName_)) {
			libraries_.append(pair.libraryName_);
		}
	}
}

void VhdlGenerator::connectEndPoint( const Connection::EndPoint& endPoint, QSharedPointer<Signal> signal ) {

	if (signals_.contains(endPoint)) {

		QSharedPointer<Signal> oldSignal = signals_.value(endPoint);
		// get all end points associated with signal for endpoint 2
		QList<Connection::EndPoint> endPoints = signals_.keys(oldSignal);
		// replace each end point with association to signal for endpoint
		foreach (Connection::EndPoint temp, endPoints) {
			signals_.insert(temp, signal);
		}
	}
	// if end point was not yet specified
	else {
		signals_.insert(endPoint, signal);
	}
}

QString VhdlGenerator::quoteGenericValue(
    const QString& type, const QString& value) const {

    if (type.toLower() != "string") {
        return value;
    }
    
    QString quoted;
    if (!value.startsWith("\"")) {
        quoted += "\"";
    }
    
    quoted += value;
    
    if (!value.endsWith("\"")) {
        quoted += "\"";
    }
    return quoted;
}
