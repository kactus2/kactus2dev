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

	QStringList logicalPorts = interface1->getLogicalPortNames();
	foreach (QString logicalPort, logicalPorts) {

		// if the logical port is not contained in both interfaces
		if (!interface2->hasLogicalPort(logicalPort)) {
			continue;
		}


	}
}
