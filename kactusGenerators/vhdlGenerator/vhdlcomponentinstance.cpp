//-----------------------------------------------------------------------------
// File: vhdlcomponentinstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 26.10.2011
//
// Description:
// Represents one vhdl component instantiation.
//-----------------------------------------------------------------------------

#include "vhdlcomponentinstance.h"

#include "vhdlcomponentdeclaration.h"
#include "vhdlgeneral.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/Port.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QSharedPointer>
#include <QMultiMap>
#include <QChar>

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::VhdlComponentInstance()
//-----------------------------------------------------------------------------
VhdlComponentInstance::VhdlComponentInstance(QObject* parent, LibraryInterface* handler,
											 VhdlComponentDeclaration* compDeclaration,
											 const QString& instanceName, const QString& viewName,
											 const QString& description):
QObject(parent),
VhdlObject(instanceName, description),
compDeclaration_(compDeclaration),
instanceName_(instanceName),
componentModuleName_(compDeclaration->typeName()),
architecture_(),
description_(description),
defaultPortConnections_(),
genericMap_(),
portMap_() 
{
	Q_ASSERT(handler);

	QSharedPointer<Component> component = compDeclaration_->componentModel();
	Q_ASSERT(component);

	// Look up the view instantiation.
	QSharedPointer<View> view;

	foreach ( QSharedPointer<View> currentView, *component->getViews() )
	{
		if ( currentView->name() == viewName )
		{
			view = currentView;
			break;
		}
	}

	// Look up the component instantiation.
	QSharedPointer<ComponentInstantiation> cimp;

	if ( view )
	{
		foreach ( QSharedPointer<ComponentInstantiation> currentInsta, *component->getComponentInstantiations() )
		{
			if ( currentInsta->name() == view->getComponentInstantiationRef() )
			{
				cimp = currentInsta;
				break;
			}
		}
	}
 	
	if ( cimp )
	{
		// set the module name that is used
		componentModuleName_ = cimp->getModuleName();
		compDeclaration_->setEntityName(componentModuleName_);

		// get the architecture name for this instance
		architecture_ = cimp->getArchitectureName();
	}

	// get the default values of the in and inout ports
	foreach ( QSharedPointer<Port> port, *component->getPorts() )
	{
		defaultPortConnections_.insert( port->name(), port->getWire()->getDefaultDriverValue() );
	}

	QMap<QString, QString> tempDefaults;

	for (QMap<QString, QString>::iterator i = defaultPortConnections_.begin();
		i != defaultPortConnections_.end(); ++i)
	{
		// get the VLNVs of the abstraction definitions for the port
		QSharedPointer<QList<QSharedPointer<AbstractionType> > > absTypes;
        QSharedPointer<BusInterface> portInterface(component->getInterfaceForPort(i.key()));
        if (!portInterface.isNull())
        {
            absTypes = portInterface->getAbstractionTypes();
        }        

		// if the port is not in any interface then use the ports own default value.
		if (!absTypes || absTypes->empty())
		{
			tempDefaults.insert(i.key(), i.value());
		}
        else
        {
            // try to find the abstraction definition and use it's default value if possible
            foreach (QSharedPointer<AbstractionType> j, *absTypes)
            {
                // if the abs def does not exist in the library
                if (handler->getDocumentType(*j->getAbstractionRef()) != VLNV::ABSTRACTIONDEFINITION)
                {
                    if (!tempDefaults.contains(i.key()))
                    {
                        tempDefaults.insert(i.key(), i.value());
                    }

                    continue;
                }

                QSharedPointer<Document> libComp = handler->getModel(*j->getAbstractionRef());
                QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();

                if (!tempDefaults.contains(i.key()))
                {
                    tempDefaults.insert(i.key(), i.value());
                }
            }
        }
	}
	defaultPortConnections_ = tempDefaults;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::~VhdlComponentInstance()
//-----------------------------------------------------------------------------
VhdlComponentInstance::~VhdlComponentInstance()
{

}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::write()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::write( QTextStream& stream ) const
{
	// if instance has description
	if (!description_.isEmpty())
    {
		stream << "  ";
		VhdlGeneral::writeDescription(description_, stream, QString("  "));
	}

	// write the instance name and type
    stream << " " << getVhdlLegalName() << " : " << compDeclaration_->getVhdlLegalName();

	// if architecture has been defined
	if (!architecture_.isEmpty())
    {
		stream << "(" << architecture_ << ")";
	}
	stream << endl;

	// print the generic map
	if (!genericMap_.isEmpty())
    {
		stream << "  " << "  " << "generic map (" << endl;
		for (QMap<QString, QString>::const_iterator i = genericMap_.begin(); i != genericMap_.end(); ++i)
        {
			stream << "  " << "  " << "  ";
			stream << i.key().leftJustified(16, ' '); //align colons (:) at least roughly
			stream << " => " << i.value();

			// if this is not the last generic to print
			if (i + 1 != genericMap_.end())
            {
				stream << "," << endl;
			}
		}
		stream << endl << "  " << "  " << ")" << endl;
	}

	// print the port map
	if (!portMap_.isEmpty())
    {
		stream << "  " << "  " << "port map (" << endl;

        for (QMap<VhdlPortMap, VhdlPortMap>::const_iterator i = portMap_.begin(); i != portMap_.end(); ++i)
        {
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
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::name()
//-----------------------------------------------------------------------------
QString VhdlComponentInstance::name() const
{
	return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::addPortMap()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::addPortMap( const VhdlConnectionEndPoint& endpoint, const QString& signalName )
{
	// create a map for the port of this instance
	VhdlPortMap instancePort(endpoint.portName(), endpoint.portLeft(), endpoint.portRight());

	// create a map for the defaultValue/top port that is connected
	VhdlPortMap signalMapping(signalName, endpoint.signalLeft(), endpoint.signalRight());

	addMapping(instancePort, signalMapping);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::addPortMap()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::addPortMap( const QString& portName, const QString& portLeft, const QString& portRight,
    const QString& portType, const QString& signalName, const QString& signalLeft, const QString& signalRight,
    const QString& signalType)
{
	// create a map for the port of this instance
	VhdlPortMap instancePort(portName, portLeft, portRight, portType);

	// create a map for the defaultValue/top port that is connected
	VhdlPortMap signalMapping(signalName, signalLeft, signalRight, signalType);

	addMapping(instancePort, signalMapping);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::addMapping()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::addMapping(const VhdlPortMap &instancePort, const VhdlPortMap &signalMapping )
{
	// if the same bits of port are already connected
	if (portMap_.contains(instancePort))
    {
		VhdlPortMap previousValue = portMap_.value(instancePort);

		// inform user that the mapping for those bits already existed.
		emit noticeMessage(tr("The instance %1:%2 already contains mapping \"%3 => %4\"").
            arg(componentModuleName_).arg(instanceName_).arg(instancePort.toString()).
            arg(previousValue.toString()));

		// inform user that the new mapping is also added
		emit noticeMessage(tr("Instance %1:%2 now has also port mapping \"%3 => %4\"").
            arg(componentModuleName_).arg(instanceName_).arg(instancePort.toString()).
            arg(signalMapping.toString()));
	}

	portMap_.insertMulti(instancePort, signalMapping);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::addGenericMap()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::addGenericMap( const QString& genericName, const QString& genericValue )
{
	// if the generics already contains a mapping for the generic
	if (genericMap_.contains(genericName))
    {
		QString oldValue = genericMap_.value(genericName);
		
		// print a notification to user that the previous value for the generic
		// is overwritten with new value
		emit noticeMessage(tr("The instance %1:%2 already contained generic mapping \"%3 => %4\" but "
            "\"%3 => %5\" replaced it.").
            arg(componentModuleName_).arg(instanceName_).arg(genericName).arg(oldValue).arg(genericValue));
	}

	genericMap_.insert(genericName, genericValue);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::hasConnection()
//-----------------------------------------------------------------------------
bool VhdlComponentInstance::hasConnection( const QString& portName )
{
	for (QMap<VhdlPortMap, VhdlPortMap>::iterator i = portMap_.begin(); i != portMap_.end(); ++i)
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
// Function: vhdlcomponentinstance::useDefaultsForOtherPorts()
//-----------------------------------------------------------------------------
void VhdlComponentInstance::useDefaultsForOtherPorts()
{
	// check all ports that have a connection
	for (QMap<VhdlPortMap, VhdlPortMap>::iterator i = portMap_.begin(); i != portMap_.end(); ++i)
    {
        defaultPortConnections_.remove(i.key().name());
	}

	// now default values remain only for ports with no connections at all

	// go through each default port value
	for (QMap<QString, QString>::iterator i = defaultPortConnections_.begin(); i != defaultPortConnections_.end();
        ++i)
    {
        // if the default value is not set
        if (i.value().isEmpty())
        {
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

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::interface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> VhdlComponentInstance::interface( const QString& interfaceName ) const
{
	return compDeclaration_->componentModel()->getBusInterface(interfaceName);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::vlnv()
//-----------------------------------------------------------------------------
VLNV VhdlComponentInstance::vlnv() const
{
	Q_ASSERT(compDeclaration_);
	Q_ASSERT(compDeclaration_->componentModel());
	return compDeclaration_->componentModel()->getVlnv();
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component> VhdlComponentInstance::componentModel() const
{
	Q_ASSERT(compDeclaration_);
	return compDeclaration_->componentModel();
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::portType()
//-----------------------------------------------------------------------------
QString VhdlComponentInstance::portType( const QString& portName ) const
{
	Q_ASSERT(compDeclaration_);
	return compDeclaration_->portType(portName);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::hasPort()
//-----------------------------------------------------------------------------
bool VhdlComponentInstance::hasPort( const QString& portName ) const
{
	Q_ASSERT(compDeclaration_);
	Q_ASSERT(compDeclaration_->componentModel());
	return compDeclaration_->componentModel()->hasPort(portName);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::typeName()
//-----------------------------------------------------------------------------
QString VhdlComponentInstance::typeName() const
{
	return componentModuleName_;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::isScalarPort()
//-----------------------------------------------------------------------------
bool VhdlComponentInstance::isScalarPort( const QString& portName ) const
{
	Q_ASSERT(compDeclaration_);
	return compDeclaration_->isScalarPort(portName);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::portDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction VhdlComponentInstance::portDirection( const QString& portName ) const
{
	return compDeclaration_->portDirection(portName);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::getPortPhysLeftBound()
//-----------------------------------------------------------------------------
QString VhdlComponentInstance::getPortPhysLeftBound( const QString& portName ) const
{
	return compDeclaration_->getPortPhysLeftBound(portName);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentinstance::getPortPhysRightBound()
//-----------------------------------------------------------------------------
QString VhdlComponentInstance::getPortPhysRightBound( const QString& portName ) const
{
	return compDeclaration_->getPortPhysRightBound(portName);
}
