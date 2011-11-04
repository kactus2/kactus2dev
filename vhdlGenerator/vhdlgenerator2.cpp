/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlgenerator2.cpp
 *		Project: Kactus 2
 */

#include "vhdlgenerator2.h"

#include <LibraryManager/libraryinterface.h>

#include <models/view.h>
#include <models/librarycomponent.h>
#include <models/modelparameter.h>
#include <models/port.h>
#include <models/businterface.h>
#include <models/generaldeclarations.h>

#include <mainwindow/mainwindow.h>

VhdlGenerator2::VhdlGenerator2(LibraryInterface* handler, MainWindow* parent):
QObject(parent),
handler_(handler),
component_(),
design_(),
desConf_(),
viewName_(),
architectureName_(),
topLevelEntity_(),
libraries_(),
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

void VhdlGenerator2::setArchitectureName( const QString& architectureName ) {
	architectureName_ = architectureName;
}

bool VhdlGenerator2::parse( QSharedPointer<Component> topLevelComponent, 
						   const QString& viewName ) {
	
	Q_ASSERT(topLevelComponent);
	component_ = topLevelComponent;

	// the name of the top-level entity is the name of the top-level component
	topLevelEntity_ = component_->getVlnv()->getName().remove(QString(".comp"));
	viewName_ = viewName;
	
	// if design can not be parsed
	if (!parseDesignAndConfiguration()) {
		return false;
	}

	// parse the info for the top entity
	parseTopGenerics();
	parseTopPorts();

	// if design is used then these can be parsed also
	if (design_) {
		parseInstances();
		parseInterconnections();
	}

	return true;
}

void VhdlGenerator2::generateVhdl( const QString& outputFileName, 
								  const QString& architectureName ) {

}

bool VhdlGenerator2::addRTLView( const QString& vhdlFileName ) {
	return false;
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
		emit errorMessage(tr("Component %1 did not contain view %2.").arg(
			component_->getVlnv()->toString()).arg(
			viewName_));
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

	// the design and possibly the configuration are now parsed
	return true;
}

bool VhdlGenerator2::containsArchitecture() const {
	// if design exists then architecture can be created
	return design_;
}

void VhdlGenerator2::parseTopGenerics() {

	QMap<QString, QSharedPointer<ModelParameter> > modelParams = component_->getModelParameters();
	foreach (QSharedPointer<ModelParameter> modelParam, modelParams) {
		QString name = modelParam->getName();
		QSharedPointer<VhdlGeneric> generic(new VhdlGeneric(this, modelParam.data()));

		topGenerics_.insert(name, generic);
	}
}

void VhdlGenerator2::parseTopPorts() {
	
	QMap<QString, QSharedPointer<Port> > ports = component_->getPorts();
	foreach (QSharedPointer<Port> port, ports) {

		// do not add ports with invalid direction or phantom direction
		if (port->getDirection() == General::DIRECTION_INVALID ||
			port->getDirection() == General::DIRECTION_PHANTOM) {
				continue;
		}

		// create the actual port
		QSharedPointer<VhdlPort> vhdlPort(new VhdlPort(this, port.data()));

		// create the sorter instance
		VhdlPortSorter sorter(vhdlPort->name(), port->getDirection());

		// this port can not be created yet
		Q_ASSERT(!topPorts_.contains(sorter));

		topPorts_.insert(sorter, vhdlPort);
	}
}

void VhdlGenerator2::parseInstances() {
	Q_ASSERT(design_);

	QList<Design::ComponentInstance> instances =
		design_->getComponentInstances();

	foreach (Design::ComponentInstance instance, instances) {
		VLNV::IPXactType instanceType = handler_->getDocumentType(instance.componentRef);

		// if vlnv is not found in library
		if (instanceType == VLNV::INVALID) {
			emit errorMessage(tr("Component %1 referenced in desgin %2 was not"
				" found in library.").arg(
				instance.componentRef.toString()).arg(
				component_->getVlnv()->toString()));
			continue;
		}
		// if vlnv does not reference a component
		else if (instanceType != VLNV::COMPONENT) {
			emit errorMessage(tr("VLNV %1 does not belong to a component.").arg(
				instance.componentRef.toString()));
			continue;
		}

		// pointer to the matching component declaration
		QSharedPointer<VhdlComponentDeclaration> compDeclaration;
		
		// if component declaration is already created
		if (components_.contains(instance.componentRef)) {
			compDeclaration = components_.value(instance.componentRef);
		}

		// if component declaration is not yet created then create it
		else {
			QSharedPointer<LibraryComponent> libComp = handler_->getModel(instance.componentRef);
			QSharedPointer<Component> component = libComp.staticCast<Component>();
			Q_ASSERT(component);
			compDeclaration = QSharedPointer<VhdlComponentDeclaration>(
				new VhdlComponentDeclaration(component, this));

			components_.insert(instance.componentRef, compDeclaration);
		}
		Q_ASSERT(compDeclaration);

		QString instanceActiveView;
		// if configuration is used then get the active view for the instance
		if (desConf_) {
			instanceActiveView = desConf_->getActiveView(instance.instanceName);
		}

		// create the instance
		QSharedPointer<VhdlComponentInstance> compInstance(new VhdlComponentInstance(
			this, compDeclaration.data(), instance.instanceName, instanceActiveView,
			instance.instanceName));
		instances_.insert(instance.instanceName, compInstance);
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
		BusInterface* interface1 = 0;
		QSharedPointer<VhdlComponentInstance> instance2;
		BusInterface* interface2 = 0;
		
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
									   BusInterface* interface1,
									   QSharedPointer<VhdlComponentInstance> instance2,
									   BusInterface* interface2 ) {
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
			const QString port2Name = portMap1->physicalPort_;

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

			// create the name for the signal to be created
			const QString signalName = connectionName + portMap1->logicalPort_;

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
	Q_ASSERT(ports.size() < 1);

	// calculate the bounds for the signal
	int left = ports.first().leftBound_;
	int right = ports.first().rightBound_;
	int signalSize = left - right + 1;
	QString type = ports.first().instance_->portType(ports.first().portName_);

	// create the endPoints
	QList<VhdlConnectionEndPoint> endPoints;
	foreach (VhdlGenerator2::PortConnection connection, ports) {

		// make sure all ports are for same type
		if (type != connection.instance_->portType(connection.portName_)) {
			emit errorMessage(tr("Type mismatch for instance %1 port %2.").arg(
				connection.instance_->name()).arg(connection.portName_));
		}

		VhdlConnectionEndPoint endPoint(connection.instance_->name(), connection.portName_,
			connection.leftBound_, connection.rightBound_, left, right);
		endPoints.append(endPoint);
	}

	// the signal that connects the end points
	QSharedPointer<VhdlSignal> signal;

	// find a signal to use for connecting the end points
	foreach (VhdlConnectionEndPoint endPoint, endPoints) {
		if (signals_.contains(endPoint)) {
			signal = signals_.value(endPoint);
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
	foreach (VhdlConnectionEndPoint endPoint, endPoints) {
		connectEndPoint(endPoint, signal);
	}
}

void VhdlGenerator2::connectEndPoint( const VhdlConnectionEndPoint& endPoint,
									 const QSharedPointer<VhdlSignal> signal ) {

	Q_ASSERT(signal);

	// if the end point already has a signal associated with it
	if (signals_.contains(endPoint)) {

		QSharedPointer<VhdlSignal> oldSignal = signals_.value(endPoint);
		// get all end points associated with signal for endpoint 2
		QList<VhdlConnectionEndPoint> endPoints = signals_.keys(oldSignal);
		// replace each end point with association to signal for endpoint
		foreach (VhdlConnectionEndPoint temp, endPoints) {
			signals_.insert(temp, signal);
		}
	}
	// if end point was not yet specified
	else {
		signals_.insert(endPoint, signal);
	}
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
