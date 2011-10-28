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

VhdlComponentInstance::VhdlComponentInstance( VhdlGenerator2* parent,
											 VhdlComponentDeclaration* compDeclaration,
											 const QString& instanceName,
											 const QString& description):
QObject(parent), 
compDeclaration_(compDeclaration),
instanceName_(instanceName),
typeName_(compDeclaration->typeName()),
description_(description),
defaultPortConnections_(),
genericMap_(),
portMap_() {

	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString)), Qt::UniqueConnection);
}

VhdlComponentInstance::~VhdlComponentInstance() {
}

void VhdlComponentInstance::write( QTextStream& stream ) {

}

QString VhdlComponentInstance::name() const {
	return instanceName_;
}

void VhdlComponentInstance::addPortMap( const VhdlConnectionEndPoint& endPoint, 
									   const QString& signalName ) {

	// create a map for the port of this instance
	VhdlPortMap instancePort(endPoint.portName(), endPoint.portLeft(), endPoint.portRight());

	// create a map for the signal/top port that is connected
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

	// create a map for the signal/top port that is connected
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
		emit noticeMessage(tr("The instance %1:%2 already contains generic mapping"
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
	return false;
}

void VhdlComponentInstance::useDefaultsForOtherPorts() {

}
