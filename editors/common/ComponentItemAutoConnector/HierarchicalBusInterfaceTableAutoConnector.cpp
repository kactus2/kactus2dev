//-----------------------------------------------------------------------------
// File: HierarchicalBusInterfaceTableAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically connects active and hierarchical bus interfaces.
//-----------------------------------------------------------------------------

#include "HierarchicalBusInterfaceTableAutoConnector.h"

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceTableAutoConnector::HierarchicalBusInterfaceTableAutoConnector()
//-----------------------------------------------------------------------------
HierarchicalBusInterfaceTableAutoConnector::HierarchicalBusInterfaceTableAutoConnector(LibraryInterface* library):
BusInterfaceTableAutoConnector(library)
{

}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceTableAutoConnector::getCompatibleInterfaceModes()
//-----------------------------------------------------------------------------
QVector<General::InterfaceMode> HierarchicalBusInterfaceTableAutoConnector::getCompatibleInterfaceModes(
    General::InterfaceMode const& busMode) const
{
    return General::getCompatibleInterfaceModeForHierarchicalInterface(busMode);
}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceTableAutoConnector::findAlreadyConnectedItems()
//-----------------------------------------------------------------------------
QList<QPair<QString, QString > > HierarchicalBusInterfaceTableAutoConnector::findAlreadyConnectedItems(QString const& firstInstanceName, QString const& secondInstanceName, QSharedPointer<Design> design) const
{
    QList<QPair<QString, QString > > foundConnections;

    for (auto const& interconnection : *design->getInterconnections())
    {
        auto startInterface = interconnection->getStartInterface();

        if (startInterface->getComponentReference() == firstInstanceName)
        {
            for (auto const& hierInterface : *interconnection->getHierInterfaces())
            {
                foundConnections.emplace_back(startInterface->getBusReference(), hierInterface->getBusReference());
            }
        }
    }

    return foundConnections;
}
