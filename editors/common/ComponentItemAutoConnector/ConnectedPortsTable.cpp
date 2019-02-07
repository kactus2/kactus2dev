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

#include <editors/common/ComponentItemAutoConnector/PortList.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/JaroWinklerAlgorithm.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::ConnectedPortsTable()
//-----------------------------------------------------------------------------
ConnectedPortsTable::ConnectedPortsTable(ComponentItem* firstItem, ComponentItem* secondItem, QWidget* parent):
QTableWidget(parent),
firstItem_(firstItem),
secondItem_(secondItem),
dragSourceComponentItem_(),
portConnections_(),
removeRowAction_(new QAction(tr("Remove row"), this))
{
    setDragDropMode(QAbstractItemView::DropOnly);

    setColumnCount(2);
    verticalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);

    QString firstHeader = tr("Port from\n") + getItemName(firstItem);
    QString secondHeader = tr("Port from\n") + getItemName(secondItem);
    QStringList horizontalHeaders = { firstHeader, secondHeader };

    setHorizontalHeaderLabels(horizontalHeaders);
    connectAutomatically();

    connect(removeRowAction_, SIGNAL(triggered()), this, SLOT(onRemoveRow()), Qt::UniqueConnection);
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
    QVector<QPair<ConnectedPortsTable::ConnectedPort, ConnectedPortsTable::ConnectedPort> > fullConnections;

    for (auto connection : portConnections_)
    {
        if (connection.first.port_ && connection.second.port_)
        {
            fullConnections.append(connection);
        }
    }

    return fullConnections;
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
// Function: ConnectedPortsTable::connectAutomatically()
//-----------------------------------------------------------------------------
void ConnectedPortsTable::connectAutomatically()
{
    clearConnectedPorts();
    findInitialConnections();
    createPortTableItems();
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
void ConnectedPortsTable::findInitialConnections()
{
    const double JAROWINKLERTRESHOLD = 0.75;

    QSharedPointer<QList<QSharedPointer<Port> > > firstPorts = firstItem_->componentModel()->getPorts();
    QSharedPointer<QList<QSharedPointer<Port> > > secondPorts = secondItem_->componentModel()->getPorts();

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
                firstPortItem.containingItem_ = firstItem_;

                ConnectedPort secondPortItem;
                secondPortItem.port_ = matchingPort;
                secondPortItem.containingItem_ = secondItem_;

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

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::contextMenuEvent()
//-----------------------------------------------------------------------------
void ConnectedPortsTable::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());

    if (index.isValid())
    {
        QMenu menu(this);

        menu.addAction(removeRowAction_);

        menu.exec(event->globalPos());
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::onRemoveRow()
//-----------------------------------------------------------------------------
void ConnectedPortsTable::onRemoveRow()
{
    portConnections_.remove(currentIndex().row());

    removeRow(currentIndex().row());
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::dropEvent()
//-----------------------------------------------------------------------------
void ConnectedPortsTable::dropEvent(QDropEvent *event)
{
    PortList* sourceList = qobject_cast<PortList*>(event->source());
    if (sourceList)
    {
        dragSourceComponentItem_ = sourceList->getContainingItem();
    }

    QTableWidget::dropEvent(event);
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::dropMimeData()
//-----------------------------------------------------------------------------
bool ConnectedPortsTable::dropMimeData(int row, int column, const QMimeData *data, Qt::DropAction action)
{
    int newColumn = column;

    if (data->hasText())
    {
        QString portName = data->text();

        if (dragSourceComponentItem_ == firstItem_)
        {
            newColumn = 0;
        }
        else
        {
            newColumn = 1;
        }

        QSharedPointer<Port> draggedPort = getPortByName(portName, dragSourceComponentItem_);

        QTableWidgetItem* itemOnRow;

        if (row < portConnections_.size())
        {
            itemOnRow = item(row, newColumn);
            if (!itemOnRow)
            {
                itemOnRow = new QTableWidgetItem();
                setItem(row, newColumn, itemOnRow);
            }

            QPair<ConnectedPortsTable::ConnectedPort, ConnectedPortsTable::ConnectedPort> connectionAtRow =
                portConnections_.at(row);

            ConnectedPortsTable::ConnectedPort newPairing;
            newPairing.port_ = draggedPort;
            newPairing.containingItem_ = dragSourceComponentItem_;
            if (dragSourceComponentItem_ == firstItem_)
            {
                connectionAtRow.first = newPairing;
            }
            else
            {
                connectionAtRow.second = newPairing;
            }

            portConnections_.replace(row, connectionAtRow);
        }
        else
        {
            setRowCount(rowCount() + 1);

            itemOnRow = new QTableWidgetItem();
            setItem(row, newColumn, itemOnRow);

            QPair<ConnectedPortsTable::ConnectedPort, ConnectedPortsTable::ConnectedPort> newConnection;
            if (dragSourceComponentItem_ == firstItem_)
            {
                newConnection.first.port_ = draggedPort;
                newConnection.first.containingItem_ = firstItem_;
            }
            else
            {
                newConnection.second.port_ = draggedPort;
                newConnection.second.containingItem_ = secondItem_;
            }

            portConnections_.append(newConnection);
        }

        itemOnRow->setIcon(QIcon(getIconPath(draggedPort->getDirection())));
        itemOnRow->setText(portName);

    }

    dragSourceComponentItem_ = nullptr;

    return QTableWidget::dropMimeData(row, newColumn, data, action);
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::getPortByName()
//-----------------------------------------------------------------------------
QSharedPointer<Port> ConnectedPortsTable::getPortByName(QString const& portName, ComponentItem* containingItem)
    const
{
    for (auto port : *containingItem->componentModel()->getPorts())
    {
        if (port->name() == portName)
        {
            return port;
        }
    }

    return QSharedPointer<Port>();
}

//-----------------------------------------------------------------------------
// Function: ConnectedPortsTable::mimeTypes()
//-----------------------------------------------------------------------------
QStringList ConnectedPortsTable::mimeTypes() const
{
    QStringList types(QTableWidget::mimeTypes());
    types << "text/plain";

    return types;
}
