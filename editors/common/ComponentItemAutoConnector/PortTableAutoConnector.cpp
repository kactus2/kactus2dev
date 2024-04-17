//-----------------------------------------------------------------------------
// File: PortTableAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Automatically connects ports of two components.
//-----------------------------------------------------------------------------

#include "PortTableAutoConnector.h"

#include <editors/common/PortUtilities.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: PortTableAutoConnector::findPossibleCombinations()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QVector<QString> > > PortTableAutoConnector::findPossibleCombinations(
    QSharedPointer<Component> firstComponent, QSharedPointer<Component> secondComponent) const
{
    QVector<QPair<QString, QVector<QString> > > possibleCombinations;

    QSharedPointer<QList<QSharedPointer<Port> > > firstPorts = firstComponent->getPorts();
    QSharedPointer<QList<QSharedPointer<Port> > > secondPorts = secondComponent->getPorts();

    for (auto currentPort : *firstPorts)
    {
        QVector<QString> connectablePorts = getConnectablePortNames(currentPort->getDirection(), secondPorts);
        if (!connectablePorts.isEmpty())
        {
            QPair<QString, QVector<QString> > combination;
            combination.first = currentPort->name();
            combination.second = connectablePorts;

            possibleCombinations.append(combination);
        }
    }

    return possibleCombinations;
}

//-----------------------------------------------------------------------------
// Function: PortTableAutoConnector::getConnectablePorts()
//-----------------------------------------------------------------------------
QVector<QString> PortTableAutoConnector::getConnectablePortNames(DirectionTypes::Direction portDirection,
    QSharedPointer<QList<QSharedPointer<Port>>> secondItemPorts) const
{
    QVector<QString> connectablePorts;

    QVector<DirectionTypes::Direction> connectableDirections = getConnectableDirections(portDirection);
    if (!connectableDirections.isEmpty())
    {
        for (auto comparisonPort : *secondItemPorts)
        {
            if (connectableDirections.contains(comparisonPort->getDirection()))
            {
                connectablePorts.append(comparisonPort->name());
            }
        }
    }

    return connectablePorts;
}

//-----------------------------------------------------------------------------
// Function: PortTableAutoConnector::getConnectableDirections()
//-----------------------------------------------------------------------------
QVector<DirectionTypes::Direction> PortTableAutoConnector::getConnectableDirections(
    DirectionTypes::Direction portDirection) const
{
    return PortUtilities::getConnectableDirections(portDirection);
}

//-----------------------------------------------------------------------------
// Function: PortTableAutoConnector::findAlreadyConnectedItems()
//-----------------------------------------------------------------------------
QList<QPair<QString, QString > > PortTableAutoConnector::findAlreadyConnectedItems(QString const& firstInstanceName, QString const& secondInstanceName, QSharedPointer<Design> design) const
{
    QList<QPair<QString, QString > > foundConnections;

    // Find connections connecting both instances.
    for (auto const& adHocConnection : *design->getAdHocConnections())
    {
        bool firstComponentInstanceFound = false;
        bool secondComponentInstanceFound = false;
        
        QSet<QString> firstInstancePorts;
        QSet<QString> secondInstancePorts;

        for (auto const& portReference : *adHocConnection->getInternalPortReferences())
        {
            if (portReference->getComponentRef() == firstInstanceName)
            {
                firstComponentInstanceFound = true;
                firstInstancePorts.insert(portReference->getPortRef());
            }
            else if (portReference->getComponentRef() == secondInstanceName)
            {
                secondComponentInstanceFound = true;
                secondInstancePorts.insert(portReference->getPortRef());
            }
        }

        if (firstComponentInstanceFound && secondComponentInstanceFound)
        {
            for (auto const& firstPort : firstInstancePorts)
            {
                for (auto const& secondPort : secondInstancePorts)
                {
                    foundConnections.emplace_back(firstPort, secondPort);
                }
            }
        }
    }

    return foundConnections;
}

//-----------------------------------------------------------------------------
// Function: PortTableAutoConnector::createTableWidgetItem()
//-----------------------------------------------------------------------------
QTableWidgetItem* PortTableAutoConnector::createTableWidgetItem(QString const& itemName,
    QSharedPointer<Component> containingComponent) const
{
    QTableWidgetItem* newTableItem = TableAutoConnector::createTableWidgetItem(itemName, containingComponent);

    QSharedPointer<Port> itemPort = containingComponent->getPort(itemName);
    QIcon portItemIcon(PortUtilities::getDirectionIconPath(itemPort->getDirection()));

    newTableItem->setIcon(portItemIcon);
    return newTableItem;
}
