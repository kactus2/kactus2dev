//-----------------------------------------------------------------------------
// File: InterfaceDirectionNameSorter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.08.2014
//
// Description:
// Class for sorting component port names by interface, direction and name.
//-----------------------------------------------------------------------------

#include "InterfaceDirectionNameSorter.h"
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Model.h>

//-----------------------------------------------------------------------------
// Function: InterfaceDirectionNameSorter::InterfaceDirectionNameSorter()
//-----------------------------------------------------------------------------
InterfaceDirectionNameSorter::InterfaceDirectionNameSorter()
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceDirectionNameSorter::InterfaceDirectionNameSorter()
//-----------------------------------------------------------------------------
InterfaceDirectionNameSorter::~InterfaceDirectionNameSorter()
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceDirectionNameSorter::sortedPortsNames()
//-----------------------------------------------------------------------------
QStringList InterfaceDirectionNameSorter::sortedPortNames(QSharedPointer<Component> component) const
{
    QStringList portNames;

    QList<QSharedPointer<Port> > ports = sortedPorts(component);

    foreach(QSharedPointer<Port> port, ports)
    {
        portNames.append(port->name());
    }

    return portNames;
}

//-----------------------------------------------------------------------------
// Function: InterfaceDirectionNameSorter::sortedPorts()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Port> > InterfaceDirectionNameSorter::sortedPorts(QSharedPointer<Component> component) const
{
    if (component.isNull())
    {
        return QList<QSharedPointer<Port> >();
    }

    QMap<SortKey, QSharedPointer<Port> > sortedPorts;
    foreach(QSharedPointer<Port> port, *component->getPorts())
    {
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
            component->getInterfacesUsedByPort(port->name());

        if (busInterfaces->size() == 1)
        {
            SortKey key(busInterfaces->first()->name(), port->getDirection(), port);
            sortedPorts.insert(key, port);
        }
        else if (!busInterfaces->isEmpty())
        {
            SortKey key("several", port->getDirection(), port);
            sortedPorts.insert(key, port);
        }
        else
        {
            SortKey key("none", port->getDirection(), port);
            sortedPorts.insert(key, port);
        }
    }

    return sortedPorts.values();
}

//-----------------------------------------------------------------------------
// Function: InterfaceDirectionNameSorter::Sorter::operator<()
//-----------------------------------------------------------------------------
bool InterfaceDirectionNameSorter::SortKey::operator<(SortKey const& other) const
{
    if (interfaceName != other.interfaceName)
    {
        if (interfaceName == "none")
        {
            return false;
        }
        else if (other.interfaceName == "none")
        {
            return true;
        }    
        else if (interfaceName == "several")
        {
            return false;
        }
        else if (other.interfaceName == "several")
        {
            return true;
        }
        else
        {
            return interfaceName < other.interfaceName;
        }
    }

    if (portDirection != other.portDirection)
    {
        return portDirection < other.portDirection;
    }

    return port->name().compare(other.port->name(), Qt::CaseInsensitive) <= 0;
}
