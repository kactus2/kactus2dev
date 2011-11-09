/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentinstance.cpp
 *		Project: Kactus 2
 */

#include "vhdlcomponentinstance.h"

#include "vhdlgenerator2.h"
#include "vhdlcomponentdeclaration.h"
#include "vhdlgeneral.h"
#include <models/component.h>
#include <models/view.h>

#include <QSharedPointer>

// typedef to be used for container containing the generic map
typedef QMap<QString, QString> generics;

// typedef to be used for container containing the port map
typedef QMap<VhdlPortMap, VhdlPortMap> portMaps;

VhdlComponentInstance::VhdlComponentInstance( VhdlGenerator2* parent,
											 VhdlComponentDeclaration* compDeclaration,
											 const QString& instanceName,
											 const QString& viewName,
											 const QString& description):
QObject(parent), 
compDeclaration_(compDeclaration),
instanceName_(instanceName),
typeName_(compDeclaration->typeName()),
architecture_(),
description_(description),
defaultPortConnections_(),
genericMap_(),
portMap_() {

	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString)), Qt::UniqueConnection);

	// if view name is not specified
	if (viewName.isEmpty()) {
		return;
	}

	QSharedPointer<Component> component = compDeclaration_->componentModel();
	Q_ASSERT(component);

	View* view = component->findView(viewName);
	
	// the component did not contain the given view
	if (!view) {
		return;
	}

	// search for a flat view that implements this component
	while (view->isHierarchical()) {
		// find a reference to a flat view
		QString viewRef = view->getTopLevelView();
		
		// if theres no top level flat view referenced of it does not exist
		if (viewRef.isEmpty() || !component->hasView(viewRef)) {
			return;
		}

		view = component->findView(viewRef);
	}
	Q_ASSERT(view);

	// get the architecture specified for the view
	architecture_ = view->getModelName();
}

VhdlComponentInstance::~VhdlComponentInstance() {
}

void VhdlComponentInstance::write( QTextStream& stream ) const {
	// if instance has description
	if (!description_.isEmpty()) {
		stream << "\t-- " << description_ << endl;
	}

	// write the instance name and type
	stream << "\t" << instanceName_ << " : " << typeName_;

	// if architecture has been defined
	if (!architecture_.isEmpty()) {
		stream << architecture_;
	}
	stream << endl;

	// print the generic map

	stream << "\t\tgeneric map (" << endl;
	for (generics::iterator i = genericMap_.begin(); i != genericMap_.end(); ++i) {
		stream << "\t\t\t" << i.key() << " => " << i.value();

		// if this is not the last generic to print
		if (i + 1 != genericMap_.end()) {
			stream << "," << endl;
		}
	}
	stream << ")" << endl;

	// print the port map

	stream << "\t\tport map (" << endl;
	for (portMaps::iterator i = portMap_.begin(); i != portMap_.end(); ++i) {
		stream << "\t\t\t";
		i.key().write(stream);
		stream << " => ";
		i.value().write(stream);

		// if this is not the last port map to print
		if (i + 1 != portMap_.end()) {
			stream << "," << endl;
		}
	}
	stream << ");" << endl;
}

QString VhdlComponentInstance::name() const {
	return instanceName_;
}

void VhdlComponentInstance::addPortMap( const VhdlConnectionEndPoint& endPoint, 
									   const QString& signalName ) {

	// create a map for the port of this instance
	VhdlPortMap instancePort(endPoint.portName(), endPoint.portLeft(), endPoint.portRight());

	// create a map for the defaultValue/top port that is connected
	VhdlPortMap signalMapping(signalName, endPoint.signalLeft(), endPoint.signalRight());

	addMapping(instancePort, signalMapping);
}

void VhdlComponentInstance::addPortMap( const QString& portName,
									   int portLeft, 
									   int portRight,
									   const QString& signalName,
									   int signalLeft, 
									   int signalRight ) {
	
	// create a map for the port of this instance
	VhdlPortMap instancePort(portName, portLeft, portRight);

	// create a map for the defaultValue/top port that is connected
	VhdlPortMap signalMapping(signalName, signalLeft, signalRight);

	addMapping(instancePort, signalMapping);
}

void VhdlComponentInstance::addMapping(const VhdlPortMap &instancePort,
									   const VhdlPortMap &signalMapping ) {
	// if the same bits of port are already connected
	if (portMap_.contains(instancePort)) {
		VhdlPortMap previousValue = portMap_.value(instancePort);

		// inform user that the mapping for those bits already existed.
		emit noticeMessage(tr("The instance %1:%2 already contains mapping "
			"\"%3 => %4\"").arg(
			typeName_).arg(
			instanceName_).arg(
			instancePort.toString()).arg(
			previousValue.toString()));

		// inform user that the new mapping is also added
		emit noticeMessage(tr("Instance %1:%2 now has also port mapping "
			"\"%3 => %4\"").arg(
			typeName_).arg(
			instanceName_).arg(
			instancePort.toString()).arg(
			signalMapping.toString()));
	}

	portMap_.insertMulti(instancePort, signalMapping);
}

void VhdlComponentInstance::addGenericMap( const QString& genericName,
										  const QString& genericValue ) {
											  
	// if the generics already contains a mapping for the generic
	if (genericMap_.contains(genericName)) {
		QString oldValue = genericMap_.value(genericName);
		
		// print a notification to user that the previous value for the generic
		// is overwritten with new value
		emit noticeMessage(tr("The instance %1:%2 already contained generic mapping"
			" \"%3 => %4\" but \"%3 => %5\" replaced it.").arg(
			typeName_).arg(
			instanceName_).arg(
			genericName).arg(
			oldValue).arg(
			genericValue));
	}

	genericMap_.insert(genericName, genericValue);
}

bool VhdlComponentInstance::hasConnection( const QString& portName ) {
	for (portMaps::iterator i = portMap_.begin();
		i != portMap_.end(); ++i) {
			
			// if the mapping is for port with same name
			if (i.key().name().compare(portName, Qt::CaseInsensitive) == 0) {
				return true;
			}
	}

	// not port mapping for named port was found
	return false;
}

void VhdlComponentInstance::useDefaultsForOtherPorts() {
	// check all ports that have a connection
	for (portMaps::iterator i = portMap_.begin();
		i != portMap_.end(); ++i) {

			defaultPortConnections_.remove(i.key().name());
	}

	// now default values remain only for ports with no connections at all

	// go through each default port value
	for (QMap<QString, QString>::iterator i = defaultPortConnections_.begin();
		i != defaultPortConnections_.end(); ++i) {
		
		VhdlPortMap port(i.key());
		VhdlPortMap defaultValue(i.value());
		addMapping(port, defaultValue);
	}
}

BusInterface* VhdlComponentInstance::interface( const QString& interfaceName ) const {
	return compDeclaration_->componentModel()->getBusInterface(interfaceName);
}

VLNV VhdlComponentInstance::vlnv() const {
	Q_ASSERT(compDeclaration_);
	Q_ASSERT(compDeclaration_->componentModel());
	return *compDeclaration_->componentModel()->getVlnv();
}

QSharedPointer<Component> VhdlComponentInstance::componentModel() const {
	Q_ASSERT(compDeclaration_);
	return compDeclaration_->componentModel();
}

QString VhdlComponentInstance::portType( const QString& portName ) const {
	Q_ASSERT(compDeclaration_);
	return compDeclaration_->portType(portName);
}

bool VhdlComponentInstance::hasPort( const QString& portName ) const {
	Q_ASSERT(compDeclaration_);
	Q_ASSERT(compDeclaration_->componentModel());
	return compDeclaration_->componentModel()->hasPort(portName);
}

QString VhdlComponentInstance::typeName() const {
	return typeName_;
}
