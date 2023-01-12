//-----------------------------------------------------------------------------
// File: vhdlcomponentdeclaration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 26.10.2011
//
// Description:
// VHDL component declaration.
//-----------------------------------------------------------------------------


#include "vhdlcomponentdeclaration.h"

#include "vhdlgeneral.h"

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/PortMap.h>

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::VhdlComponentDeclaration()
//-----------------------------------------------------------------------------
VhdlComponentDeclaration::VhdlComponentDeclaration(QSharedPointer<Component> component, 
    QSharedPointer<ExpressionParser> parser) :
VhdlObject(component->getVlnv().getName(), QString()),
component_(component),
typeName_(component->getVlnv().getName()),
generics_(),
ports_(),
instantations_() 
{
	Q_ASSERT(component);


	// parse the ports for the component declaration
	foreach (QSharedPointer<Port> port, *component_->getPorts())
    {
		// do not add ports with invalid direction or phantom direction
		if (port->getDirection() == DirectionTypes::DIRECTION_INVALID ||
			port->getDirection() == DirectionTypes::DIRECTION_PHANTOM)
        {
			continue;
		}

		// create the actual port
		QSharedPointer<VhdlPort> vhdlPort(new VhdlPort(port, parser));

		// create the sorter instance
        QString busName = getContainingBusInterfaceName(port->name());

		VhdlPortSorter sorter(busName, vhdlPort->name(), port->getDirection());

		// this port can not be created yet
		Q_ASSERT(!ports_.contains(sorter));

		ports_.insert(sorter, vhdlPort);
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::write()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::write( QTextStream& stream ) const
{
	// if component contains a description
	if (!description().isEmpty())
    {
		VhdlGeneral::writeDescription(description(), stream, QString("  "));
	}
	stream << "  " << "-- IP-XACT VLNV: " << component_->getVlnv().toString() << Qt::endl;
    stream << "  " << "component " << getVhdlLegalName() << Qt::endl;

	// write the generic declarations
	if (!generics_.isEmpty())
    {
        writeGenerics(stream);
	}

	// write the port declarations
	if (!ports_.isEmpty() && VhdlPort::hasRealPorts(ports_))
    {
        writePorts(stream);
	}

	stream << "  end component;" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::writeGenerics()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::writeGenerics(QTextStream& stream) const
{
    stream << "  " << "  " << "generic (" << Qt::endl;
    for (QMap<QString, QSharedPointer<VhdlGeneric> >::const_iterator i = generics_.begin();
        i != generics_.end(); ++i)
    {
        stream << "  " << "  " << "  ";
        i.value()->write(stream);

        // if this is not the last generic to write
        if (i + 1 != generics_.end())
        {
            stream << ";";
        }

        if (!i.value()->description().isEmpty())
        {
            stream << " ";
            VhdlGeneral::writeDescription(i.value()->description(), stream);
        }
        else
        {
            stream << Qt::endl;
        }
    }
    stream << "  " << "  " << ");" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::writeGenerics()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::writePorts(QTextStream& stream) const
{
    stream << "  " << "  " << "port (" << Qt::endl;
    QString previousInterface;
    for (QMap<VhdlPortSorter, QSharedPointer<VhdlPort> >::const_iterator i = ports_.begin();
        i != ports_.end(); ++i)
    {
        // if the port is first in the interface, then introduce the interface
        if (i.key().interface() != previousInterface)
        {
            const QString interfaceName = i.key().interface();

            stream << Qt::endl << "  " << "  " << "  " << "-- ";

            if (interfaceName == QString("none"))
            {
                stream << "These ports are not in any interface" << Qt::endl;
            }
            else if (interfaceName == QString("several"))
            {
                stream << "There ports are contained in many interfaces" << Qt::endl;
            }
            else
            {
                stream << "Interface: " << interfaceName << Qt::endl;
                const QString description = component_->getBusInterface(
                    interfaceName)->description();
                if (!description.isEmpty())
                {
                    VhdlGeneral::writeDescription(description, stream, QString("      "));
                }
            }
            previousInterface = interfaceName;
        }

        stream << "  " << "  " << "  "; //3 indents
        i.value()->write(stream); // print the actual port definition


                                  // if this is not the last port to write
        if (i + 1 != ports_.end())
        {
            stream << ";";
        }

        if (!i.value()->description().isEmpty())
        {
            stream << " ";
            VhdlGeneral::writeDescription(i.value()->description(), stream);
        }
        else
        {
            stream << Qt::endl;
        }
    }
    stream << "    );" << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::typeName()
//-----------------------------------------------------------------------------
QString VhdlComponentDeclaration::typeName() const
{
	return typeName_;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::description()
//-----------------------------------------------------------------------------
QString VhdlComponentDeclaration::description() const
{
	return component_->getDescription();
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component> VhdlComponentDeclaration::componentModel() const
{
	return component_;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::addInstantiation()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::addInstantiation( QSharedPointer<VhdlComponentInstance> instance )
{
	Q_ASSERT(!instantations_.contains(instance));

	instantations_.append(instance);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::hasGeneric()
//-----------------------------------------------------------------------------
bool VhdlComponentDeclaration::hasGeneric( const QString& genericName ) const
{
	return generics_.contains(genericName);
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::checkPortConnections()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::checkPortConnections()
{
	// check each vhdl port
	foreach (QSharedPointer<VhdlPort> port, ports_)
    {
		// check each instance of this type
		foreach (QSharedPointer<VhdlComponentInstance> instantation, instantations_)
        {
			// if the named port is connected in this instance
			if (instantation->hasConnection(port->name()))
            {
				// port is needed so it can not be commented and search can be stopped for this port
				port->setCommented(false);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::portType()
//-----------------------------------------------------------------------------
QString VhdlComponentDeclaration::portType( const QString& portName ) const
{
    QString busInterfaceName = getContainingBusInterfaceName(portName);

	// used to search for the correct port
    VhdlPortSorter sorter(busInterfaceName, portName, component_->getPort(portName)->getDirection());
	
	// if the named port is found
	if (ports_.contains(sorter))
    {
		return ports_.value(sorter)->type();
	}
	else
    {
		return QString("undefined port");
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::isScalarPort()
//-----------------------------------------------------------------------------
bool VhdlComponentDeclaration::isScalarPort( const QString& portName ) const
{
    QString busInterfaceName = getContainingBusInterfaceName(portName);

    VhdlPortSorter sorter(busInterfaceName, portName, component_->getPort(portName)->getDirection());

	QString type;

	if (ports_.contains(sorter))
    {
		type = ports_.value(sorter)->type();
		return VhdlGeneral::isScalarType(type);
	}
	// if port is not found then it is not scalar
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::setEntityName()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::setEntityName( const QString& entityName )
{
	typeName_ = entityName;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::portDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction VhdlComponentDeclaration::portDirection( const QString& portName ) const
{
	return component_->getPort(portName)->getDirection();
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::getPortPhysLeftBound()
//-----------------------------------------------------------------------------
QString VhdlComponentDeclaration::getPortPhysLeftBound( const QString& portName ) const
{
	return component_->getPort(portName)->getLeftBound();
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::getPortPhysRightBound()
//-----------------------------------------------------------------------------
QString VhdlComponentDeclaration::getPortPhysRightBound( const QString& portName ) const
{
	return component_->getPort(portName)->getRightBound();
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::getContainingBusInterfaceName()
//-----------------------------------------------------------------------------
QString VhdlComponentDeclaration::getContainingBusInterfaceName(QString const& portName) const
{
    QString busName = "none";

    QSharedPointer<BusInterface> containingInterface = component_->getInterfaceForPort(portName);
    if (!containingInterface.isNull())
    {
        busName = containingInterface->name();
    }

    return busName;
}