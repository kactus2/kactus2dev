//-----------------------------------------------------------------------------
// File: InterfaceDirectionNameSorter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
// Function: InterfaceDirectionNameSorter::sortedPorts()
//-----------------------------------------------------------------------------
QStringList InterfaceDirectionNameSorter::sortedPortNames(QSharedPointer<Component> component) const
{
    if (component.isNull())
    {
        return QStringList();
    }

    QMap<SortKey, QString> sortedPorts;
    foreach(QString portName, component->getPortNames())
    {
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
            component->getInterfacesUsedByPort(portName);

        if (busInterfaces->size() == 1)
		{
            SortKey key(busInterfaces->first()->name(), component->getModel()->getPort(portName)->getDirection(),
                portName);
			sortedPorts.insert(key, portName);
		}
        else if (!busInterfaces->isEmpty())
        {
            SortKey key("several", component->getModel()->getPort(portName)->getDirection(), portName);
            sortedPorts.insert(key, portName);
        }
		else
		{
			SortKey key("none", component->getModel()->getPort(portName)->getDirection(), portName);
			sortedPorts.insert(key, portName);
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

    return portName.compare(other.portName, Qt::CaseInsensitive) <= 0;
}
