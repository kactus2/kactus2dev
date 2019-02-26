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

    QVector<DirectionTypes::Direction> connectableDirections =
        PortUtilities::getConnectableDirections(portDirection);
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
// Function: PortTableAutoConnector::createTableWidgetItem()
//-----------------------------------------------------------------------------
QTableWidgetItem* PortTableAutoConnector::createTableWidgetItem(QString const& itemName,
    QSharedPointer<Component> containingComponent) const
{
    QTableWidgetItem* newTableItem = TableAutoConnector::createTableWidgetItem(itemName, containingComponent);

    QSharedPointer<Port> itemPort = containingComponent->getPort(itemName);
    QIcon portItemIcon(getIconPath(itemPort->getDirection()));

    newTableItem->setIcon(portItemIcon);
    return newTableItem;
}

//-----------------------------------------------------------------------------
// Function: PortTableAutoConnector::getIconPath()
//-----------------------------------------------------------------------------
QString PortTableAutoConnector::getIconPath(DirectionTypes::Direction portDirection) const
{
    QString iconPath = ":icons/common/graphics/cross.png";
    if (portDirection == DirectionTypes::IN)
    {
        iconPath = ":icons/common/graphics/input.png";
    }
    else if (portDirection == DirectionTypes::OUT)
    {
        iconPath = ":icons/common/graphics/output.png";
    }
    else if (portDirection == DirectionTypes::INOUT)
    {
        iconPath = ":icons/common/graphics/inout.png";
    }
    else if (portDirection == DirectionTypes::DIRECTION_PHANTOM)
    {
        iconPath = ":icons/common/graphics/phantom.png";
    }

    return iconPath;
}
