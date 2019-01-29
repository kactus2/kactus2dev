//-----------------------------------------------------------------------------
// File: ConnectedPortsTable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.01.2019
//
// Description:
// Table for connected ports of two components.
//-----------------------------------------------------------------------------

#include "ConnectedPortsTable.h"

#include <common/graphicsItems/ComponentItem.h>

#include <editors/ComponentEditor/busInterfaces/portmaps/JaroWinklerAlgorithm.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::ConnectedPortsTable()
//-----------------------------------------------------------------------------
ConnectedPortsTable::ConnectedPortsTable(ComponentItem* firstItem, ComponentItem* secondItem, QWidget* parent):
QTableWidget(parent),
portConnections_()
{
    setColumnCount(2);
    verticalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);

    QString firstHeader = tr("Port from\n") + getItemName(firstItem);
    QString secondHeader = tr("Port from\n") + getItemName(secondItem);
    QStringList horizontalHeaders = { firstHeader, secondHeader };

    setHorizontalHeaderLabels(horizontalHeaders);
    findInitialConnections(firstItem, secondItem);
    createPortTableItems();
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::~ConnectedPortsTable()
//-----------------------------------------------------------------------------
ConnectedPortsTable::~ConnectedPortsTable()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::getConnectedPorts()
//-----------------------------------------------------------------------------
QVector<QPair<ConnectedPortsTable::ConnectedPort, ConnectedPortsTable::ConnectedPort> > ConnectedPortsTable::
    getConnectedPorts() const
{
    return portConnections_;
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::clearConnectedPorts()
//-----------------------------------------------------------------------------
void ConnectedPortsTable::clearConnectedPorts()
{
    setRowCount(0);
    portConnections_.clear();
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::getItemName()
//-----------------------------------------------------------------------------
QString ConnectedPortsTable::getItemName(ComponentItem* componentItem) const
{
    if (!componentItem->displayName().isEmpty())
    {
        return componentItem->displayName();
    }
    else
    {
        return componentItem->name();
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::findInitialConnections()
//-----------------------------------------------------------------------------
void ConnectedPortsTable::findInitialConnections(ComponentItem* firstItem, ComponentItem* secondItem)
{
    const double JAROWINKLERTRESHOLD = 0.75;

    QSharedPointer<QList<QSharedPointer<Port> > > firstPorts = firstItem->componentModel()->getPorts();
    QSharedPointer<QList<QSharedPointer<Port> > > secondPorts = secondItem->componentModel()->getPorts();

    for (auto currentPort : *firstPorts)
    {
        DirectionTypes::Direction portDirection = currentPort->getDirection();
        QVector<QSharedPointer<Port> > connectablePorts = getConnectablePorts(portDirection, secondPorts);

        QMap<double, QSharedPointer<Port> > possibleConnections;

        for (auto comparisonPort : connectablePorts)
        {
            double jaroDistance =
                JaroWinklerAlgorithm::calculateJaroWinklerDistance(currentPort->name(), comparisonPort->name());

            possibleConnections.insertMulti(jaroDistance, comparisonPort);
        }

        if (!possibleConnections.isEmpty())
        {
            QSharedPointer<Port> matchingPort = possibleConnections.last();
            if (matchingPort)
            {
                QPair<ConnectedPort, ConnectedPort> portCombination;

                ConnectedPort firstPortItem;
                firstPortItem.port_ = currentPort;
                firstPortItem.containingItem_ = firstItem;

                ConnectedPort secondPortItem;
                secondPortItem.port_ = matchingPort;
                secondPortItem.containingItem_ = secondItem;

                portCombination.first = firstPortItem;
                portCombination.second = secondPortItem;

                portConnections_.append(portCombination);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::getConnectablePorts()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<Port> > ConnectedPortsTable::getConnectablePorts(DirectionTypes::Direction portDirection,
    QSharedPointer<QList<QSharedPointer<Port>>> secondItemPorts) const
{
    //! In -> Out & InOut & phantom
    //! Out -> In & InOut & phantom
    //! InOut -> InOut & phantom
    //! Phantom -> In & Out & InOut & Phantom
    QList<DirectionTypes::Direction> connectableDirections;
    if (portDirection == DirectionTypes::IN)
    {
        connectableDirections = { DirectionTypes::OUT, DirectionTypes::INOUT, DirectionTypes::DIRECTION_PHANTOM };
    }
    else if (portDirection == DirectionTypes::OUT)
    {
        connectableDirections = { DirectionTypes::IN, DirectionTypes::INOUT, DirectionTypes::DIRECTION_PHANTOM };
    }
    else if (portDirection == DirectionTypes::INOUT)
    {
        connectableDirections = { DirectionTypes::INOUT, DirectionTypes::DIRECTION_PHANTOM };
    }
    else if (portDirection == DirectionTypes::DIRECTION_PHANTOM)
    {
        connectableDirections =
        { DirectionTypes::IN, DirectionTypes::OUT, DirectionTypes::INOUT, DirectionTypes::DIRECTION_PHANTOM };
    }

    QVector<QSharedPointer<Port> > connectablePorts;
    if (!connectableDirections.isEmpty())
    {
        for (auto comparisonPort : *secondItemPorts)
        {
            if (connectableDirections.contains(comparisonPort->getDirection()))
            {
                connectablePorts.append(comparisonPort);
            }
        }
    }

    return connectablePorts;
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::createPortTableItems()
//-----------------------------------------------------------------------------
void ConnectedPortsTable::createPortTableItems()
{
    setRowCount(portConnections_.size());

    for (int i = 0; i < portConnections_.size(); ++i)
    {
        QSharedPointer<Port> firstPort = portConnections_.at(i).first.port_;
        QString firstIconPath = getIconPath(firstPort->getDirection());
        QSharedPointer<Port> secondPort = portConnections_.at(i).second.port_;
        QString secondIconPath = getIconPath(secondPort->getDirection());

        QTableWidgetItem* firstItem = new QTableWidgetItem(QIcon(firstIconPath), firstPort->name());
        QTableWidgetItem* secondItem = new QTableWidgetItem(QIcon(secondIconPath), secondPort->name());

        setItem(i, 0, firstItem);
        setItem(i, 1, secondItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::getIconPath()
//-----------------------------------------------------------------------------
QString ConnectedPortsTable::getIconPath(DirectionTypes::Direction portDirection)
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
