//-----------------------------------------------------------------------------
// File: HierarchicalPortTableAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically connects ports of a component item and a top level component.
//-----------------------------------------------------------------------------

#include "HierarchicalPortTableAutoConnector.h"

#include <IPXACTmodels/Design/Design.h>

#include <editors/common/PortUtilities.h>

//-----------------------------------------------------------------------------
// Function: HierarchicalPortTableAutoConnector::getConnectableDirections()
//-----------------------------------------------------------------------------
QVector<DirectionTypes::Direction> HierarchicalPortTableAutoConnector::getConnectableDirections(
    DirectionTypes::Direction portDirection) const
{
    return PortUtilities::getConnectableHierarchicalDirections(portDirection);
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortTableAutoConnector::findAlreadyConnectedItems()
//-----------------------------------------------------------------------------
QList<QPair<QString, QString > > HierarchicalPortTableAutoConnector::findAlreadyConnectedItems(
    QString const& firstInstanceName, QString const& secondInstanceName, QSharedPointer<Design> design) const
{
    QList<QPair<QString, QString > > foundConnections;

    // Find connections connecting both instances.
    for (auto const& adHocConnection : *design->getAdHocConnections())
    {
        bool firstComponentInstanceFound = false;

        QSet<QString> firstInstancePorts;
        QSet<QString> secondInstancePorts;

        for (auto const& portReference : *adHocConnection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == firstInstanceName)
            {
                firstComponentInstanceFound = true;
                firstInstancePorts.insert(portReference->getPortRef());
            }
        }

        if (firstComponentInstanceFound)
        {
            for (auto const& firstPort : firstInstancePorts)
            {
                // All external ports must be connected
                for (auto const& secondPort : *adHocConnection->getExternalPortReferences())
                {
                    foundConnections.emplace_back(firstPort, secondPort->getPortRef());
                }
            }
        }
    }

    return foundConnections;
}
