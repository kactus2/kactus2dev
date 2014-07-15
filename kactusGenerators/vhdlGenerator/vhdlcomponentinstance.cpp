/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentinstance.cpp
 *		Project: Kactus 2
 */

#include "vhdlcomponentinstance.h"

#include "vhdlcomponentdeclaration.h"
#include "vhdlgeneral.h"

#include <kactusGenerators/HDLGenerator/HDLUtils.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/view.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/businterface.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QSharedPointer>
#include <QMultiMap>
#include <QChar>

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::VhdlComponentInstance()
//-----------------------------------------------------------------------------
VhdlComponentInstance::VhdlComponentInstance(QObject* parent,
                                             LibraryInterface* handler,
											 QSharedPointer<Component> component,
											 QString const& instanceName,
											 QString const& viewName,
											 QString const& description):
HDLComponentInstance(parent, component, instanceName, viewName, description), 
defaultPortConnections_(),
modelParameterValues_(),
portMap_() 
{
	Q_ASSERT(handler);
	Q_ASSERT(component);

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

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::~VhdlComponentInstance()
//-----------------------------------------------------------------------------
VhdlComponentInstance::~VhdlComponentInstance()
{

}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::write()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::write( QTextStream& stream ) const {
	// if instance has description
	if (!description().isEmpty()) {
		stream << "  ";
		HDLUtils::writeDescription(description(), stream, "--", QString("  "));
	}

	// write the instance name and type
	stream << "  " << name() << " : " << type();

	// if architecture has been defined
	if (!getArchitecture().isEmpty()) {
		stream << "(" << getArchitecture() << ")";
	}
	stream << endl;

    writeGenericAssignments(stream);
    writePortMaps(stream);

}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::mapToPort()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::mapToPort(const VhdlConnectionEndPoint& endpoint, QString const& signalName)
{
	// create a map for the port of this instance
	VhdlPortMap instancePort(endpoint.portName(), endpoint.portLeft(), endpoint.portRight());

	// create a map for the defaultValue/top port that is connected
	VhdlPortMap signalMapping(signalName, endpoint.signalLeft(), endpoint.signalRight());

	addMapping(instancePort, signalMapping);
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::mapToSignal()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::mapToSignal(QString const& portName, int portLeft, int portRight, 
    QString const& portType, QString const& signalName, int signalLeft, int signalRight, QString const& signalType)
{	
	// create a map for the port of this instance
	VhdlPortMap instancePort(portName, portLeft, portRight, portType);

	// create a map for the defaultValue/top port that is connected
	VhdlPortMap signalMapping(signalName, signalLeft, signalRight, signalType);

	addMapping(instancePort, signalMapping);
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::hasConnection()
//-----------------------------------------------------------------------------
bool VhdlComponentInstance::hasConnection( QString const& portName )
{
	for (QMap<VhdlPortMap, VhdlPortMap>::iterator i = portMap_.begin();	i != portMap_.end(); ++i)
    {		
        // if the mapping is for port with same name
        if (i.key().name().compare(portName, Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }

	// not port mapping for named port was found
	return false;
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::useDefaultsForOtherPorts()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::useDefaultsForOtherPorts()
{
	// check all ports that have a connection
	for (QMap<VhdlPortMap, VhdlPortMap>::iterator i = portMap_.begin();	i != portMap_.end(); ++i)
    {
        defaultPortConnections_.remove(i.key().name());
	}

	// now default values remain only for ports with no connections at all

	// go through each default port value
	for (QMap<QString, QString>::iterator i = defaultPortConnections_.begin();
        i != defaultPortConnections_.end(); ++i)
    {	
        // if the default value is set
        if (!i.value().isEmpty())
        {
            VhdlPortMap port(i.key());

            // get the type of the port
            QString portTypeStr(portType(i.key()));

            // make sure the default value is in correct form
            QString defaultStr = VhdlGeneral::convertDefaultValue(i.value(), portTypeStr);

            VhdlPortMap defaultValue(defaultStr);
            addMapping(port, defaultValue);
        }
	}
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::isScalarPort()
//-----------------------------------------------------------------------------
bool VhdlComponentInstance::isScalarPort(QString const& portName) const
{
    QString type = portType(portName);
    return VhdlGeneral::isScalarType(type);
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::portType()
//-----------------------------------------------------------------------------
QString VhdlComponentInstance::portType(QString const& portName) const
{
    foreach(QSharedPointer<Port> port, componentModel()->getPorts())
    {
        if (port->getName() == portName)
        {
            QString type = port->getTypeName();

            if (type.isEmpty())
            {
                type = VhdlGeneral::useDefaultType(port->getLeftBound(), port->getRightBound());
            }

            return type;
        }
    }
    return QString("undefined port");
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::writeGenericAssignments()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::writeGenericAssignments(QTextStream& stream) const
{
    if (modelParameterValues_.isEmpty())
    {
        return;
    }

    // print the generic map
    stream << "  " << "  " << "generic map (" << endl;
    for (QMap<QString, QString>::const_iterator i = modelParameterValues_.begin(); 
        i != modelParameterValues_.end(); ++i)
    {
        stream << "  " << "  " << "  ";
        stream << i.key().leftJustified(16, ' '); //align colons (:) at least roughly
        stream << " => " << i.value();

        // if this is not the last generic to print
        if (i + 1 != modelParameterValues_.end())
        {
            stream << "," << endl;
        }
    }
    stream << endl << "  " << "  " << ")" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentInstance::writePortMaps()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::writePortMaps(QTextStream& stream) const
{
    if (portMap_.isEmpty())
    {
        return;
    }

    // print the port map
    stream << "  " << "  " << "port map (" << endl;
    for (QMap<VhdlPortMap, VhdlPortMap>::const_iterator i = portMap_.begin(); i != portMap_.end(); ++i) {

        stream << "  " << "  " << "  " ;
        i.key().write(stream);
        stream << " => ";
        stream << i.value().toString();
        //i.value().write(stream);

        // if this is not the last port map to print, add comma (,)
        if (i + 1 != portMap_.end())
        {
            stream << "," << endl;
        }
    }
    stream << endl << "  " << "  " << ");" << endl;

}

void VhdlComponentInstance::addMapping(const VhdlPortMap &instancePort,
    const VhdlPortMap &signalMapping ) {
        // if the same bits of port are already connected
        if (portMap_.contains(instancePort)) {
            VhdlPortMap previousValue = portMap_.value(instancePort);

            // inform user that the mapping for those bits already existed.
            emit noticeMessage(tr("The instance %1:%2 already contains mapping "
                "\"%3 => %4\"").arg(
                type()).arg(
                name()).arg(
                instancePort.toString()).arg(
                previousValue.toString()));

            // inform user that the new mapping is also added
            emit noticeMessage(tr("Instance %1:%2 now has also port mapping "
                "\"%3 => %4\"").arg(
                type()).arg(
                name()).arg(
                instancePort.toString()).arg(
                signalMapping.toString()));
        }

        portMap_.insertMulti(instancePort, signalMapping);
}