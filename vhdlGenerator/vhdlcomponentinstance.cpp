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
#include <models/librarycomponent.h>
#include <models/abstractiondefinition.h>
#include <models/businterface.h>

#include <LibraryManager/libraryinterface.h>

#include <QSharedPointer>
#include <QMultiMap>
#include <QChar>

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

	LibraryInterface* handler = parent->handler();
	Q_ASSERT(handler);

	QSharedPointer<Component> component = compDeclaration_->componentModel();
	Q_ASSERT(component);
 	
	// set the entity name that is used
	typeName_ = component->getEntityName(viewName);
	compDeclaration_->setEntityName(typeName_);

	// get the architecture name for this instance
	architecture_ = component->getArchitectureName(viewName);

	// get the default values of the in and inout ports
	QMap<QString, QString> defaultValues = component->getPortDefaultValues();
	for (QMap<QString, QString>::iterator i = defaultValues.begin(); 
		i != defaultValues.end(); ++i) {

			if (!i.value().isEmpty()) {
				defaultPortConnections_.insert(i.key(), i.value());
			}
	}

	QMap<QString, QString> tempDefaults;
	for (QMap<QString, QString>::iterator i = defaultPortConnections_.begin();
		i != defaultPortConnections_.end(); ++i) {

		// get the VLNVs of the abstraction definitions for the port
		QMultiMap<QString, VLNV> absDefs = component->getInterfaceAbsDefForPort(i.key());

		// if the port is not in any interface then use the ports own default value.
		if (absDefs.isEmpty()) {
			tempDefaults.insert(i.key(), i.value());
		}

		// try to find the abstraction definition and use it's default value if possible
		for (QMultiMap<QString, VLNV>::Iterator j = absDefs.begin();
			j != absDefs.end(); ++j) {

			// if the abs def does not exist in the library
			if (handler->getDocumentType(j.value()) != VLNV::ABSTRACTIONDEFINITION) {
				if (!tempDefaults.contains(i.key()))
					tempDefaults.insert(i.key(), i.value());
				continue;
			}

			QSharedPointer<LibraryComponent> libComp = handler->getModel(j.value());
			QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

			// if the abstraction definition has defined a default value for the port
			// then use it before the port's own default value
			if (absDef->hasDefaultValue(j.key())) {
				tempDefaults.insert(i.key(), QString::number(absDef->getDefaultValue(j.key())));
				break;
			}

			// if abstraction definition did not contain 
			else {
				if (!tempDefaults.contains(i.key()))
					tempDefaults.insert(i.key(), i.value());
				continue;
			}
		}
	}
	defaultPortConnections_ = tempDefaults;
}

VhdlComponentInstance::~VhdlComponentInstance() {
}

void VhdlComponentInstance::write( QTextStream& stream ) const {
	// if instance has description
	if (!description_.isEmpty()) {
		stream << "\t";
		VhdlGeneral::writeDescription(description_, stream, QString("\t"));
	}

	// write the instance name and type
	stream << "\t" << instanceName_ << " : " << typeName_;

	// if architecture has been defined
	if (!architecture_.isEmpty()) {
		stream << "(" << architecture_ << ")";
	}
	stream << endl;

	// print the generic map

	if (!genericMap_.isEmpty()) {
		stream << "\t\tgeneric map (" << endl;
		for (QMap<QString, QString>::const_iterator i = genericMap_.begin(); i != genericMap_.end(); ++i) {
			stream << "\t\t\t" << i.key() << " => " << i.value();

			// if this is not the last generic to print
			if (i + 1 != genericMap_.end()) {
				stream << "," << endl;
			}
		}
		stream << endl << "\t\t)" << endl;
	}

	// print the port map

	if (!portMap_.isEmpty()) {
		stream << "\t\tport map (" << endl;
                for (QMap<VhdlPortMap, VhdlPortMap>::const_iterator i = portMap_.begin(); i != portMap_.end(); ++i) {
			stream << "\t\t\t";
			i.key().write(stream);
			stream << " => ";
			i.value().write(stream);

			// if this is not the last port map to print
			if (i + 1 != portMap_.end()) {
				stream << "," << endl;
			}
		}
		stream << endl << "\t\t);" << endl;
	}
}

QString VhdlComponentInstance::name() const {
	return instanceName_;
}

void VhdlComponentInstance::addPortMap( const VhdlConnectionEndPoint& endpoint, 
									   const QString& signalName ) {

	// create a map for the port of this instance
	VhdlPortMap instancePort(endpoint.portName(), endpoint.portLeft(), endpoint.portRight());

	// create a map for the defaultValue/top port that is connected
	VhdlPortMap signalMapping(signalName, endpoint.signalLeft(), endpoint.signalRight());

	addMapping(instancePort, signalMapping);
}

void VhdlComponentInstance::addPortMap( const QString& portName,
									   int portLeft, 
									   int portRight,
									   const QString& portType,
									   const QString& signalName,
									   int signalLeft, 
									   int signalRight,
									   const QString& signalType) {
	
	// create a map for the port of this instance
	VhdlPortMap instancePort(portName, portLeft, portRight, portType);

	// create a map for the defaultValue/top port that is connected
	VhdlPortMap signalMapping(signalName, signalLeft, signalRight, signalType);

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
	for (QMap<VhdlPortMap, VhdlPortMap>::iterator i = portMap_.begin();
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
	for (QMap<VhdlPortMap, VhdlPortMap>::iterator i = portMap_.begin();
		i != portMap_.end(); ++i) {
			defaultPortConnections_.remove(i.key().name());
	}

	// now default values remain only for ports with no connections at all

	// go through each default port value
	for (QMap<QString, QString>::iterator i = defaultPortConnections_.begin();
		i != defaultPortConnections_.end(); ++i) {
		
			// if the default value is not set
			if (i.value().isEmpty()) {
				continue;
			}

		VhdlPortMap port(i.key());

		// get the type of the port
		QString portTypeStr(portType(i.key()));

		// make sure the default value is in correct form
		QString defaultStr = VhdlGeneral::convertDefaultValue(i.value(), portTypeStr);

		VhdlPortMap defaultValue(defaultStr);
		addMapping(port, defaultValue);
	}
}

QSharedPointer<BusInterface> VhdlComponentInstance::interface( const QString& interfaceName ) const {
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

bool VhdlComponentInstance::isScalarPort( const QString& portName ) const {
	Q_ASSERT(compDeclaration_);
	return compDeclaration_->isScalarPort(portName);
}

General::Direction VhdlComponentInstance::portDirection( const QString& portName ) const {
	return compDeclaration_->portDirection(portName);
}

int VhdlComponentInstance::getPortPhysLeftBound( const QString& portName ) const {
	return compDeclaration_->getPortPhysLeftBound(portName);
}

int VhdlComponentInstance::getPortPhysRightBound( const QString& portName ) const {
	return compDeclaration_->getPortPhysRightBound(portName);
}
