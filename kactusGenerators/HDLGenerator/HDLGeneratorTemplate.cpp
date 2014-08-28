//-----------------------------------------------------------------------------
// File: HDLGeneratorTemplate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.7.2014
//
// Description:
// Creates a structural HDL file with following structure
//    *      1. File header 
//    *      2. Libraries
//    *      3. Entity declaration
//    *      4. Architecture
//    *         - Signals (+ user's own code)
//    *         - Component declarations
//    *         - Component instantiations (+ user's own code)
//-----------------------------------------------------------------------------

#include "HDLGeneratorTemplate.h"

#include "HDLUtils.h"
#include "vhdlconnectionendpoint.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/view.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/PortRef.h>
#include <IPXACTmodels/Interconnection.h>
#include <IPXACTmodels/HierConnection.h>
#include <IPXACTmodels/PortMap.h>

#include <common/utils.h>

#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QDate>
#include <QSettings>
#include <QObject>

static const QString BLACK_BOX_DECL_START = "##KACTUS2_BLACK_BOX_DECLARATIONS_BEGIN##";
static const QString BLACK_BOX_DECL_END = "##KACTUS2_BLACK_BOX_DECLARATIONS_END##";
static const QString BLACK_BOX_ASSIGN_START = "##KACTUS2_BLACK_BOX_ASSIGNMENTS_BEGIN##";
static const QString BLACK_BOX_ASSIGN_END = "##KACTUS2_BLACK_BOX_ASSIGNMENTS_END##";

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::HDLGeneratorTemplate()
//-----------------------------------------------------------------------------
HDLGeneratorTemplate::HDLGeneratorTemplate(LibraryInterface* handler, QObject* parent ):
QObject(parent),
component_(),
viewName_(),
topLevelEntity_(),
libraries_(),
typeDefinitions_(),
handler_(handler),
design_(),
desConf_(),
userModifiedDeclarations_(),
userModifiedAssignments_(),
topModelParameters_(),
topPorts_(),
signals_(),
componentDeclarations_(),
instances_() 
{
	Q_ASSERT(handler);
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::~HDLGeneratorTemplate()
//-----------------------------------------------------------------------------
HDLGeneratorTemplate::~HDLGeneratorTemplate()
{

}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parse()
//-----------------------------------------------------------------------------
bool HDLGeneratorTemplate::parse( QSharedPointer<Component> topLevelComponent, QString const& viewName ) {
	
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
	parseTopModelParameters();
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
		foreach (QSharedPointer<HDLComponentInstance> instance, instances_)
        {
			instance->useDefaultsForOtherPorts();
		}

		// tell each component declaration to check for it's ports and 
		// uncomment those that are needed
		foreach (QSharedPointer<HDLComponentDeclaration> comp, componentDeclarations_)
        {
			comp->checkPortConnections();
		}
	}
	// if design is not used then set all ports uncommented.
	else {

		for (QMap<VhdlPortSorter, QSharedPointer<HDLPort> >::iterator i = topPorts_.begin();
			i != topPorts_.end(); ++i)
        {
			i.value()->setConnected(true);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::generate()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::generate(QString const& outputFileName)
{			
	if (outputFileName.isEmpty())
    {
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

		QString message(tr("File: %1 couldn't be opened for writing.").arg(outputFileName));
		emit errorMessage(message);
		return;
	}
	// now file has been opened for writing

	// open file stream to write to
	QTextStream fileStream(&file);

	QFileInfo fileInfo(outputFileName);

	emit noticeMessage(tr("Writing the HDL file..."));
	
	// write header comments of the file
	writeFileHeader(fileStream, fileInfo.fileName());

    writeLibraryIncludes(fileStream);
	fileStream << endl;

	// write the top-level entity
    writeEntityStartTag(fileStream);
	fileStream << endl;

	// write the top-level generics
	writeModelParameters(fileStream);

	// write the top-level ports
	writePorts(fileStream);

	// end top-level entity definition
    writeEntityEndTag(fileStream);

	// write the architecture of the entity
    writeStructuralStartTag(fileStream);
    fileStream << endl;

	// write declarations for signals connecting the ports
	writeSignalDeclarations(fileStream);

	// write all component declarations
	writeComponentDeclarations(fileStream);

	// write the user modifiable code 
	writeUserModifiedDeclarations(fileStream);
    fileStream << endl;

	// start writing architecture component instances
    writeComponentInstantiationStartTag(fileStream);
    fileStream << endl;

	// write the user modifiable code
	writeUserModifiedAssignments(fileStream);

	// write the component instances
	writeComponentInstances(fileStream);

    writeStructuralEndTag(fileStream);
    fileStream << endl;

	file.close();

	emit noticeMessage(tr("Done writing the HDL file."));
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::handler()
//-----------------------------------------------------------------------------
LibraryInterface* HDLGeneratorTemplate::handler() const
{
    return handler_;
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseDesignAndConfiguration()
//-----------------------------------------------------------------------------
bool HDLGeneratorTemplate::parseDesignAndConfiguration()
{
	Q_ASSERT(component_);

	// if view is not specified it is not error it just means that only
	// the top entity should be created
	if (viewName_.isEmpty())
    {
		return true;
	}

	// if view is specified but it does not exist
	View* view = component_->findView(viewName_);
	
	// if view is not found
	if (!view || !view->isHierarchical())
    {
		return false;
	}

    // get the type of document for the hierarchical reference.
	VLNV hierarchyRef = view->getHierarchyRef();	
    
    return parseDesign(hierarchyRef) && parseDesignConfiguration(hierarchyRef);
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseDesign()
//-----------------------------------------------------------------------------
bool HDLGeneratorTemplate::parseDesign(VLNV const& hierarchyRef)
{
    design_ = handler_->getDesign(hierarchyRef);

    // if design was not found
    if (!design_)
    {
        VLNV designVLNV = handler_->getDesignVLNV(hierarchyRef);
        emit errorMessage(tr("The design %1 referenced in component %2 was not found in library.").arg(
            designVLNV.toString()).arg(component_->getVlnv()->toString()));

        return false;
    }

    // if design is found then make sure it is valid
    else
    {
        QStringList errorList;
        if (!design_->isValid(errorList))
        {
            emit noticeMessage(tr("The Design contained following errors:"));

            foreach (QString error, errorList)
            {
                emit errorMessage(error);
            }
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseDesignConfiguration()
//-----------------------------------------------------------------------------
bool HDLGeneratorTemplate::parseDesignConfiguration(VLNV const& hierarchyRef)
{
    VLNV::IPXactType documentType = handler_->getDocumentType(hierarchyRef);

    // if configuration is used
    if (documentType == VLNV::DESIGNCONFIGURATION)
    {
        QSharedPointer<LibraryComponent> libComp = handler_->getModel(hierarchyRef);
        desConf_ = libComp.staticCast<DesignConfiguration>();

        QStringList errorList;
        if (!desConf_->isValid(errorList))
        {
            emit noticeMessage(tr("The configuration contained following errors:"));

            foreach (QString error, errorList)
            {
                emit errorMessage(error);
            }
            return false;
        }                
    }
    else if (documentType == VLNV::INVALID)    
    {
        emit errorMessage(tr("The Hierarchical reference %1 within component %2's"
            " view %3 was not found in library.").arg(
            hierarchyRef.toString()).arg(
            component_->getVlnv()->toString()).arg(
            viewName_));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseTopModelParameters()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::parseTopModelParameters()
{
	QList<QSharedPointer<ModelParameter> > modelParams = component_->getModelParameters();
	foreach (QSharedPointer<ModelParameter> modelParam, modelParams) {
		QString name = modelParam->getName();
        QSharedPointer<HDLModelParameter> parameterObject(createModelParameterObject(modelParam));

		topModelParameters_.insert(name, parameterObject);
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseTopPorts()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::parseTopPorts()
{	
	QList<QSharedPointer<Port> > ports = component_->getPorts();
	foreach (QSharedPointer<Port> port, ports)
    {
		// do not add ports with invalid direction or phantom direction
		if (port->getDirection() == General::DIRECTION_INVALID ||
			port->getDirection() == General::DIRECTION_PHANTOM)
        {
		    continue;
		}

		// create the actual port
        QSharedPointer<HDLPort> portObject(createPortObject(port));

		// create the sorter instance
		VhdlPortSorter sorter(component_->getInterfaceNameForPort(port->getName()),	port->getName(), 
            port->getDirection());
		
		Q_ASSERT(!topPorts_.contains(sorter));

		topPorts_.insert(sorter, portObject);
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseInstances()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::parseInstances()
{
	Q_ASSERT(design_);

	QList<ComponentInstance> const& instances = design_->getComponentInstances();

	foreach (ComponentInstance const& instance, instances)
    {
		VLNV::IPXactType instanceType = handler_->getDocumentType(instance.getComponentRef());

		// if vlnv is not found in library
		if (instanceType == VLNV::INVALID)
        {
			emit errorMessage(tr("Component %1 referenced in design %2 was not"
				" found in library.").arg(instance.getComponentRef().toString()).arg(
				component_->getVlnv()->toString()));
			continue;
		}
		// if vlnv does not reference a component
		else if (instanceType != VLNV::COMPONENT)
        {
			emit errorMessage(tr("VLNV %1 does not belong to a component.").arg(
                instance.getComponentRef().toString()));
			continue;
		}

		// pointer to the matching component declaration
		QSharedPointer<HDLComponentDeclaration> compDeclaration;
		
		// if component declaration is already created
		if (componentDeclarations_.contains(instance.getComponentRef()))
        {
			compDeclaration = componentDeclarations_.value(instance.getComponentRef());
		}

		// if component declaration is not yet created then create it
		else
        {
			QSharedPointer<LibraryComponent> libComp = handler_->getModel(instance.getComponentRef());
			QSharedPointer<Component> component = libComp.staticCast<Component>();
			Q_ASSERT(component);
            compDeclaration = QSharedPointer<HDLComponentDeclaration>(createComponentDeclarationObject(component));

			componentDeclarations_.insert(instance.getComponentRef(), compDeclaration);
		}
		Q_ASSERT(compDeclaration);

		QString instanceActiveView;
		// if configuration is used then get the active view for the instance
		if (desConf_) {
			instanceActiveView = desConf_->getActiveView(instance.getInstanceName());
		}

		// create the instance
        QSharedPointer<HDLComponentInstance> compInstance(createComponentInstanceObject(compDeclaration->componentModel(), 
            instance, instanceActiveView));

        connect(compInstance.data(), SIGNAL(noticeMessage(QString const&)),
            this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);
        connect(compInstance.data(), SIGNAL(errorMessage(QString const&)),
            this, SIGNAL(errorMessage(const QString)), Qt::UniqueConnection);

		// add the libraries of the instantiated component to the library list.
        QStringList componentLibraries = getLibrariesForComponent(compInstance, instanceActiveView);
        libraries_.append(componentLibraries);

		// add each generic value to instances generic map
        for (QMap<QString, QString>::const_iterator i = instance.getConfigurableElementValues().begin();
			i != instance.getConfigurableElementValues().end(); ++i) {
			compInstance->assignModelParameter(i.key(), i.value());
		}

		// register the instance to the component declaration
		compDeclaration->addInstantiation(compInstance);

		instances_.insert(instance.getInstanceName(), compInstance);
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseInterconnections()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::parseInterconnections()
{
	Q_ASSERT(design_);

	QList<Interconnection> interconnections = design_->getInterconnections();

	QStringList connectionNames;
	foreach (Interconnection interconnection, interconnections) 
    {
		bool invalidInterconnection = false;
		QSharedPointer<HDLComponentInstance> instance1;
		QSharedPointer<BusInterface> interface1;
		QSharedPointer<HDLComponentInstance> instance2;
		QSharedPointer<BusInterface> interface2;
		
		// if there are several interconnections with same name
		if (connectionNames.contains(interconnection.name()))
        {
			emit errorMessage(tr("Design %1 contained more than one interconnection named %2").arg(
				component_->getVlnv()->toString()).arg(interconnection.name()));
		}
		else
        {
			connectionNames.append(interconnection.name());
		}

        QPair<Interface, Interface> interfaces = interconnection.getInterfaces();

		// if the instance reference is incorrect
		if (!instances_.contains(interfaces.first.getComponentRef())) {
			invalidInterconnection = true;
			emit errorMessage(tr("Instance %1 was not found in component %2.").arg(
				interfaces.first.getComponentRef()).arg(
				component_->getVlnv()->toString()));
		}
		else { 
			instance1 = instances_.value(interfaces.first.getComponentRef());
			interface1 = instance1->interface(interfaces.first.getBusRef());
		}
		// if the interface is not found
		if (!interface1) {
			emit errorMessage(tr("Bus interface %1 was not found in component %2.").arg(
				interfaces.first.getBusRef()).arg(
				instance1->vlnv().toString()));
			invalidInterconnection = true;
		}

		// if the instance reference is incorrect
		if (!instances_.contains(interfaces.second.getComponentRef())) {
			invalidInterconnection = true;
			emit errorMessage(tr("Instance %1 was not found in component %2.").arg(
				interfaces.second.getComponentRef()).arg(
				component_->getVlnv()->toString()));
		}
		else {
			instance2 = instances_.value(interfaces.second.getComponentRef());
			interface2 = instance2->interface(interfaces.second.getBusRef());
		}
		// if the interface is not found
		if (!interface2) {
			emit errorMessage(tr("Bus interface %1 was not found in component %2.").arg(
				interfaces.second.getBusRef()).arg(
				instance2->vlnv().toString()));
			invalidInterconnection = true;
		}

		// if the interconnection is invalid then move on to next interconnection
		if (invalidInterconnection) {
			continue;
		}
		else {
			connectInterfaces(interconnection.name(), interconnection.description(),
				instance1, interface1, instance2, interface2);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::connectInterfaces()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::connectInterfaces(QString const& connectionName, 
									   QString const& description,
									   QSharedPointer<HDLComponentInstance> instance1,
									   QSharedPointer<BusInterface> interface1, 
									   QSharedPointer<HDLComponentInstance> instance2, 
									   QSharedPointer<BusInterface> interface2 ) {
	Q_ASSERT(instance1);
	Q_ASSERT(interface1);
	Q_ASSERT(instance2);
	Q_ASSERT(interface2);

	// get the port maps of both interfaces
	QList<QSharedPointer<PortMap> > portMaps1 = interface1->getPortMaps();
	QList<QSharedPointer<PortMap> > portMaps2 = interface2->getPortMaps();

	// get the IP-Xact models of both instances
	QSharedPointer<Component> component1 = instance1->componentModel();
	QSharedPointer<Component> component2 = instance2->componentModel();

	foreach (QSharedPointer<PortMap> portMap1, portMaps1) {
		foreach(QSharedPointer<PortMap> portMap2, portMaps2) {

			const QString port1Name = portMap1->physicalPort();
			const QString port2Name = portMap2->physicalPort();

			// if the port maps are not for same logical signal
			if (portMap1->logicalPort() != portMap2->logicalPort()) {
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
			const QString signalName = connectionName + "_" + portMap1->logicalPort();
			
			HDLGeneratorTemplate::PortConnection port1(instance1, port1Name, 
				alignment.port1Left_, alignment.port1Right_);
			HDLGeneratorTemplate::PortConnection port2(instance2, port2Name,
				alignment.port2Left_, alignment.port2Right_);
			QList<HDLGeneratorTemplate::PortConnection> ports;
			ports.append(port1);
			ports.append(port2);

			// connect the port from both port maps
			connectPorts(signalName, description, ports);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::connectPorts()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::connectPorts(QString const& connectionName, QString const& description, 
								  const QList<HDLGeneratorTemplate::PortConnection>& ports) {

	// at least 2 ports must be found
	Q_ASSERT(ports.size() > 1);

	// the type of the signal
	QString type = ports.first().instance_->portType(ports.first().portName_);

	// the minSize for the signal
	int minSize = ports.first().leftBound_ - ports.first().rightBound_ + 1;
	int maxSize = minSize;

	// first find out the smallest possible minSize for the signal
	foreach (HDLGeneratorTemplate::PortConnection connection, ports)
    {
		// the smallest minSize needed is used
		minSize = qMin(minSize, (connection.leftBound_ - connection.rightBound_ + 1));
	}

	// calculate the bounds for the signal
	int left = minSize - 1;
	int right = 0;

	// create the endpoints
	QList<VhdlConnectionEndPoint> endpoints;
	foreach (HDLGeneratorTemplate::PortConnection connection, ports) {

		QString connectionType = connection.instance_->portType(connection.portName_);

		// make sure all ports are for compatible type
        if (!checkPortTypeMatch(type, connectionType)) {
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

        VhdlConnectionEndPoint endpoint = VhdlConnectionEndPoint(connection.instance_->name(), connection.portName_,
            connection.leftBound_, connection.rightBound_, maxSize-1, right);
    
		endpoints.append(endpoint);
	}

	// if vectored but minSize is only one then convert to scalar signal type
    if (type == vectorSignalType() && minSize == 1) {
        type = scalarSignalType();        
	}

	// if scalar signal type but minSize is larger than 1 then convert to vectored
	else if (type == scalarSignalType() && minSize > 1) {
		type == vectorSignalType();
	}

	// the signal that connects the end points
	QSharedPointer<HDLSignal> signal;

	// find a signal to use for connecting the end points
	foreach (VhdlConnectionEndPoint endpoint, endpoints) {
		if (signals_.contains(endpoint))
        {
			signal = signals_.value(endpoint);
			signal->setBounds(qMax(left, signal->left()), qMin(right, signal->right()));
			break;
		}
	}
	// if signal was not found then create a new one 
	if (!signal) {
        signal = QSharedPointer<HDLSignal>(createSignalObject(connectionName, type, left, right, description));
		signal->setBounds(left, right);
	}

	// connect each end point to given signal
	foreach (VhdlConnectionEndPoint endpoint, endpoints)
    {
		connectEndPoint(endpoint, signal);
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::connectEndPoint()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::connectEndPoint( const VhdlConnectionEndPoint& endpoint,
									 const QSharedPointer<HDLSignal> signal )
{

	Q_ASSERT(signal);

	// if the end point already has a signal associated with it
	if (signals_.contains(endpoint)) {

		QSharedPointer<HDLSignal> oldSignal = signals_.value(endpoint);
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

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseAdHocConnections()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::parseAdHocConnections() {
	// all adHoc connections are processed
	QList<AdHocConnection> adHocs = design_->getAdHocConnections();
	foreach (AdHocConnection adHoc, adHocs) {

		// the data structure to store the ports to connect
		QList<HDLGeneratorTemplate::PortConnection> ports;

		foreach (PortRef portRef, adHoc.internalPortReferences()) {

			// if the instance is not found
			if (!instances_.contains(portRef.getComponentRef())) {
				emit errorMessage(tr("Instance %1 was not found in design.").arg(
					portRef.getComponentRef()));
				continue;
			}
			QSharedPointer<HDLComponentInstance> instance =
				instances_.value(portRef.getComponentRef());
			Q_ASSERT(instance);

			// if the specified port is not found
			if (!instance->hasPort(portRef.getPortRef())) {
				emit errorMessage(tr("Port %1 was not found in instance %2 of"
					" type %3").arg(portRef.getPortRef()).arg(
					portRef.getComponentRef()).arg(
					instance->vlnv().toString()));
				continue;
			}

			int left = portRef.getLeft();
			// if left bound is not defined then use the port physical bound
			if (left < 0) {
				left = instance->getPortPhysLeftBound(portRef.getPortRef());
			}

			int right = portRef.getRight();
			// if right bound is not defined then use the port physical bound
			if (right < 0) {
				right = instance->getPortPhysRightBound(portRef.getPortRef());
			}

			// create the port specification
			HDLGeneratorTemplate::PortConnection port(instance, portRef.getPortRef(),
				left, right);

			// add port to the list
			ports.append(port);
		}

		// if the connection contains at least one port of the top component
		if (!adHoc.externalPortReferences().isEmpty()) {

			// connect each external port to instance port
			foreach (PortRef portRef, adHoc.externalPortReferences()) {

				// check that the port is found 
				VhdlPortSorter sorter(component_->getInterfaceNameForPort(portRef.getPortRef()),
					portRef.getPortRef(), 
					component_->getPortDirection(portRef.getPortRef()));
				if (!topPorts_.contains(sorter)) {
					emit errorMessage(tr("The port %1 is not found in top component %2").arg(
						portRef.getPortRef()).arg(component_->getVlnv()->toString()));
					continue;
				}

				QSharedPointer<HDLPort> port = topPorts_.value(sorter);
				Q_ASSERT(port);

				int left = portRef.getLeft();
				// if left bound is not defined then use the port physical bound
				if (left < 0) {
					left = port->left();
				}

				int right = portRef.getRight();
				// if right bound is not defined then use the port physical bound
				if (right < 0) {
					right = port->right();
				}

				// connect each instance port to the top port
				connectHierPort(portRef.getPortRef(), left, right, ports);
			}
		}
		// otherwise the connection is just between the ports of the instances
		else {

			// connect the ports 
			connectPorts(adHoc.name(), adHoc.description(), ports);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::parseHierConnections()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::parseHierConnections() {
    // search all hierConnections within design
    QList<HierConnection> hierConnections =
        design_->getHierarchicalConnections();

    foreach (HierConnection hierConnection, hierConnections) {

        // find the top-level bus interface
        QSharedPointer<BusInterface> topInterface =
            component_->getBusInterface(hierConnection.getInterfaceRef());

        // if the top level interface couldn't be found
        if (!topInterface) {
            emit errorMessage(tr("Interface %1 was not found in top component %2.").arg(
                hierConnection.getInterfaceRef()).arg(
                component_->getVlnv()->toString()));
            continue;
        }

        // find the component instance for this hierConnection
        QSharedPointer<HDLComponentInstance> instance = instances_.value(hierConnection.getInterface().getComponentRef());

        // if the component instance couldn't be found
        if (!instance) {
            emit errorMessage(tr("Instance %1 was not found in design.").arg(
                hierConnection.getInterface().getComponentRef()));
            continue;
        }

        // find the bus interface
        QSharedPointer<BusInterface> instanceInterface = instance->interface(hierConnection.getInterface().getBusRef());
        // if bus interface couldn't be found
        if (!instanceInterface) {
            emit errorMessage(tr("Interface %1 was not found in instance %2 of type %3").arg(
                hierConnection.getInterface().getBusRef()).arg(
                instance->name()).arg(
                instance->type()));
            continue;
        }

        connectHierInterface(instance, instanceInterface, topInterface);
    }
}

void HDLGeneratorTemplate::connectHierPort( QString const& topPort,
									 int leftBound, 
									 int rightBound, 
									 const QList<HDLGeneratorTemplate::PortConnection>& ports ) {

	foreach (HDLGeneratorTemplate::PortConnection port, ports) {
		
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
			QSharedPointer<HDLPort> HDLPort = topPorts_.value(sorter);
			HDLPort->setConnected(true);

			// if the top port is scalar then don't use the bit boundaries
            if (isScalarType(HDLPort->type())) {
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
		port.instance_->mapToSignal(port.portName_, portLeft, portRight,
			port.instance_->portType(port.portName_),
			topPort, leftBound, rightBound, topPorts_.value(sorter)->type());
	}
}

void HDLGeneratorTemplate::connectHierInterface(QSharedPointer<HDLComponentInstance> instance,
    QSharedPointer<BusInterface> instanceInterface, QSharedPointer<BusInterface> topInterface )
{
	Q_ASSERT(instance);
	Q_ASSERT(instanceInterface);
	Q_ASSERT(topInterface);

	// get the port maps of both interfaces
	QList<QSharedPointer<PortMap> > portMaps = instanceInterface->getPortMaps();
	QList<QSharedPointer<PortMap> > hierPortMaps = topInterface->getPortMaps();

	// get the IP-Xact model of the instance
	QSharedPointer<Component> component1 = instance->componentModel();

	foreach (QSharedPointer<PortMap> portMap, portMaps) {
		foreach(QSharedPointer<PortMap> hierPortMap, hierPortMaps) {

			const QString portName = portMap->physicalPort();
			const QString hierPortName = hierPortMap->physicalPort();

			// if the port maps are not for same logical signal
			if (portMap->logicalPort() != hierPortMap->logicalPort()) {
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

			HDLGeneratorTemplate::PortConnection port1(instance, portName, 
				alignment.port1Left_, alignment.port1Right_);
			QList<HDLGeneratorTemplate::PortConnection> ports;
			ports.append(port1);

			// connect the port from instance to the top port
			connectHierPort(hierPortName, alignment.port2Left_, alignment.port2Right_, ports);
		}
	}
}

void HDLGeneratorTemplate::mapPorts2Signals()
{
	// search each endpoint
	for (QMap<VhdlConnectionEndPoint, QSharedPointer<HDLSignal> >::iterator i = signals_.begin(); 
		i != signals_.end(); ++i) {

		// if the component instance can't be found
		if (!instances_.contains(i.key().instanceName()))
        {
			emit errorMessage(tr("Instance %1 was not found in design.").arg(i.key().instanceName()));
			continue;
		}

		// find the instance for the end point
		QSharedPointer<HDLComponentInstance> instance = instances_.value(i.key().instanceName());

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
		if (isScalarType(i.value()->type())) {
			signalLeft = -1;
			signalRight = -1;
		}

		// add a port map for the instance to connect port to signal
		instance->mapToSignal(i.key().portName(), portLeft, portRight, 
			instance->portType(i.key().portName()),
			i.value()->name(), signalLeft, signalRight, i.value()->type());	
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::writeModelParameters()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::writeModelParameters( QTextStream& outputStream ) {
	// if generics exist
	if (!topModelParameters_.isEmpty()) {

		// the start tag
        writeModelParametersStartTag(outputStream);

		for (QMap<QString, QSharedPointer<HDLModelParameter> >::iterator i = topModelParameters_.begin();
			i != topModelParameters_.end(); ++i) {

			outputStream << "  " << "  ";
			i.value()->write(outputStream);

			// if this is not the last generic to write
			if (i + 1 != topModelParameters_.end()) {
				outputStream << ";";
			}

			if (!i.value()->description().isEmpty()) {
				outputStream << " ";
				HDLUtils::writeDescription(i.value()->description(), outputStream, commentTag());
			}
			else {
				outputStream << endl;
			}
		}
        writeModelParameterEndTag(outputStream);

		outputStream << endl;
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::writePorts()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::writePorts( QTextStream& outputStream ) {
	// if ports exist
	if (!topPorts_.isEmpty() && HDLPort::hasConnectedPorts(topPorts_.values())) {

        writePortsStartTag(outputStream);

		QString previousInterface;
		// print each port
		for (QMap<VhdlPortSorter, QSharedPointer<HDLPort> >::iterator i = topPorts_.begin();
			i != topPorts_.end(); ++i) {

				// if the port is first in the interface then introduce it
				if (i.key().interface() != previousInterface) {
					const QString interfaceName = i.key().interface();

					outputStream << endl << "  " << "  " << commentTag();

					if (interfaceName == QString("none")) {
						outputStream << "These ports are not in any interface" << endl;
					}
					else if (interfaceName == QString("several")) {
						outputStream << "There ports are contained in many interfaces" << endl;
					}
					else {	
						outputStream << "Interface: " << interfaceName << endl;
						const QString description = component_->getInterfaceDescription(
							interfaceName);
						if (!description.isEmpty()) {							
							HDLUtils::writeDescription(description, outputStream, commentTag(), QString("    "));
						}
					}
					previousInterface = interfaceName;
				}

				// write the port name and direction
				outputStream << "  " << "  ";
				i.value()->write(outputStream);

				// if this is not the last port to write
				if (i + 1 != topPorts_.end()) {
					outputStream << ";";
				}
				
				if (!i.value()->description().isEmpty()) {
					outputStream << " ";
                    HDLUtils::writeDescription(i.value()->description(), outputStream, commentTag());					
				}
				else {
					outputStream << endl;
				}
		}
        writePortsEndTag(outputStream);

		// write extra empty line to make code readable
		outputStream << endl;
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::writeSignalDeclarations()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::writeSignalDeclarations( QTextStream& outputStream ) {
	
	QList<QSharedPointer<HDLSignal> > signalList;

	// get the unique signals that exist
	foreach (QSharedPointer<HDLSignal> signal, signals_.values()) {
		if (!signalList.contains(signal)) {
			signalList.append(signal);
		}
	}
	// when the list contains only the unique signals then write them
	foreach (QSharedPointer<HDLSignal> signal, signalList) {
		signal->write(outputStream);
	}
	outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::writeComponentDeclarations()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::writeComponentDeclarations( QTextStream& outputStream ) {
	foreach (QSharedPointer<HDLComponentDeclaration> comp, componentDeclarations_) {
		comp->write(outputStream);
		outputStream << endl;
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::writeComponentInstances()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::writeComponentInstances( QTextStream& outputStream ) {
	foreach (QSharedPointer<HDLComponentInstance> instance, instances_) {
		instance->write(outputStream);
		outputStream << endl;
	}
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::readUserModifiablePart()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::readUserModifiablePart( QFile& previousFile ) {
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
		if (lineCompare == (commentTag() + BLACK_BOX_DECL_START)) {
			readingDeclarations = true;
		}
		// if previous line was the last one of the declarations
		else if (lineCompare == (commentTag() + BLACK_BOX_DECL_END)) {
			readingDeclarations = false;
		}
		// if the next line starts the assignments
		else if (lineCompare == (commentTag() + BLACK_BOX_ASSIGN_START)) {
			readingAssignments = true;
		}
		// if previous line was the last of the assignments
		else if (lineCompare == (commentTag() + BLACK_BOX_ASSIGN_END)) {
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

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::writeUserModifiedDeclarations()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::writeUserModifiedDeclarations( QTextStream& stream )
{
	stream << "  " << commentTag() << "You can write code after this tag and it is saved through the generator." << endl;
	stream << "  " << commentTag() << BLACK_BOX_DECL_START << endl;
	stream << userModifiedDeclarations_;
	stream << "  " << commentTag() << BLACK_BOX_DECL_END << endl;
	stream << "  " << commentTag() << "Do not write your code after this tag." << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::writeUserModifiedAssignments()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::writeUserModifiedAssignments( QTextStream& stream )
{
	stream << "  " << commentTag() << "You can write code after this tag and it is saved through the generator." << endl;
	stream << "  " << commentTag() << BLACK_BOX_ASSIGN_START << endl;
	stream << userModifiedAssignments_;
	stream << "  " << commentTag() << BLACK_BOX_ASSIGN_END << endl;
	stream << "  " << commentTag() << "Do not write your code after this tag." << endl << endl;
}

//-----------------------------------------------------------------------------
// Function: HDLGeneratorTemplate::writeLibraryIncludes()
//-----------------------------------------------------------------------------
void HDLGeneratorTemplate::writeLibraryIncludes(QTextStream& fileStream)
{
    // always add IEEE library
    fileStream << "library IEEE;" << endl;

    // write the libraries needed 
    libraries_.append("work");
    libraries_.removeDuplicates();

    // declare the libraries used.
    foreach (QString library, libraries_) {
        if (!library.isEmpty()) {
            fileStream << "library " << library <<";" << endl;

            typeDefinitions_.append(QString("%1.all").arg(library));
        }
    }

    // always add the library for std_logic and std_logic_vector
    typeDefinitions_.append("IEEE.std_logic_1164.all");
    typeDefinitions_.removeDuplicates();

    // write all type defs needed
    foreach (QString portTypeDef, typeDefinitions_) {
        if (!portTypeDef.isEmpty()) {
            fileStream << "use " << portTypeDef << ";" << endl;
        }
    }
}

HDLGeneratorTemplate::PortConnection::PortConnection(QSharedPointer<HDLComponentInstance> instance,
	QString const& portName, int left /*= -1*/, int right /*= -1*/ ):
instance_(instance),
portName_(portName),
leftBound_(left),
rightBound_(right)
{

}

HDLGeneratorTemplate::PortConnection::PortConnection( const HDLGeneratorTemplate::PortConnection& other ):
instance_(other.instance_),
portName_(other.portName_),
leftBound_(other.leftBound_),
rightBound_(other.rightBound_)
{

}

HDLGeneratorTemplate::PortConnection& HDLGeneratorTemplate::PortConnection::operator=(
	const HDLGeneratorTemplate::PortConnection& other ) {
		if (this != &other) {
			instance_ = other.instance_;
			portName_ = other.portName_;
			leftBound_ = other.leftBound_;
			rightBound_ = other.rightBound_;
		}
		return *this;
}
