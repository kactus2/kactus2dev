//-----------------------------------------------------------------------------
// File: diagraminterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.3.2011
//
// Description:
// Diagram interface graphics item.
//-----------------------------------------------------------------------------

#include "BusInterfaceItem.h"

#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"

#include "columnview/HWColumn.h"

#include <common/IEditProvider.h>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::BusInterfaceItem()
//-----------------------------------------------------------------------------
BusInterfaceItem::BusInterfaceItem(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf,
    QSharedPointer<InterfaceGraphicsData> dataGroup, QGraphicsItem *parent):
BusInterfaceEndPoint(busIf, parent, QVector2D(1.0f, 0.0f)),
component_(component),
dataGroup_(dataGroup),
oldColumn_(0)
{
    setTemporary(busIf == 0);
    setTypeLocked(busIf != 0 && busIf->getInterfaceMode() != General::INTERFACE_MODE_COUNT);
    setPolygon(getDirectionOutShape());

    dataGroup_->setName(busIf->name());

    if (dataGroup_->hasPosition())
    {
        setPos(dataGroup_->getPosition());
    }

    if (dataGroup_->hasDirection())
    {
        setDirection(dataGroup_->getDirection());
    }
    else
    {
        dataGroup_->setDirection(getDirection());
    }

	getNameLabel()->setRotation(-rotation());

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::~BusInterfaceItem()
//-----------------------------------------------------------------------------
BusInterfaceItem::~BusInterfaceItem()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::define()
//-----------------------------------------------------------------------------
void BusInterfaceItem::define(QSharedPointer<BusInterface> busIf)
{
    // Add the bus interface to the component.
    component_->getBusInterfaces()->append(busIf);
    setBusInterface(busIf);

    // Update the interface visuals.
    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getPortsForView()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Port> > BusInterfaceItem::getPortsForView(QString const& activeView) const
{
    Q_ASSERT(getBusInterface() != 0);
    QList<QSharedPointer<Port> > ports;

    foreach (QSharedPointer<PortMap> portMap, getBusInterface()->getPortMapsForView(activeView))
    {
        if (portMap->getPhysicalPort())
        {
            QSharedPointer<Port> port = component_->getPort(portMap->getPhysicalPort()->name_);
            if (port)
            {
                ports.append(port);
            }
        }
    }

    return ports;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getAllPorts()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Port> > BusInterfaceItem::getAllPorts() const
{
    QList<QSharedPointer<Port> > ports;

    foreach (QSharedPointer<PortMap> portMap, *getBusInterface()->getAllPortMaps())
    {
        if (portMap->getPhysicalPort())
        {
            QSharedPointer<Port> port = component_->getPort(portMap->getPhysicalPort()->name_);
            if (port)
            {
                ports.append(port);
            }
        }
    }

    return ports;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::updateName()
//-----------------------------------------------------------------------------
void BusInterfaceItem::updateName(QString const& /*previousName*/, QString const& newName)
{
    dataGroup_->setName(newName);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::onConnect()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::onConnect(ConnectionEndpoint const* other)
{
    if (static_cast<HWDesignDiagram*>(scene())->isLoading())
    {
        return true;
    }

    updateInterface();

    return other != 0;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::onDisonnect()
//-----------------------------------------------------------------------------
void BusInterfaceItem::onDisconnect(ConnectionEndpoint const*)
{
    if (!getConnections().empty() ||
        getBusInterface()->getInterfaceMode() == General::INTERFACE_MODE_COUNT || isTypeLocked())
    {
        // Don't do anything.
        return;
    }

    // Update the interface visuals.
    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::isConnectionValid()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!HWConnectionEndpoint::isConnectionValid(other) || !other->isBus() || other->getBusInterface() == 0)
    {
        return false;
    }

    QSharedPointer<BusInterface> otherBusIf = other->getBusInterface();

    return (getBusInterface()->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
        !otherBusIf->getBusType().isValid() ||
        (otherBusIf->getBusType() == getBusInterface()->getBusType()));
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* BusInterfaceItem::encompassingComp() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> BusInterfaceItem::getOwnerComponent() const
{
    Q_ASSERT(component_);
    return component_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::isHierarchical()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::setDirection()
//-----------------------------------------------------------------------------
void BusInterfaceItem::setDirection(QVector2D const& dir)
{
    HWConnectionEndpoint::setDirection(dir);

    dataGroup_->setDirection(dir);

    setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::setLabelPosition()
//-----------------------------------------------------------------------------
void BusInterfaceItem::setLabelPosition()
{
    qreal nameWidth = getNameLabel()->boundingRect().width();

    // Check if the port is directed to the left.
    if (getDirection().x() < 0)
    {
        getNameLabel()->setPos(0, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the port is directed to the right.
    else
    {
        getNameLabel()->setPos(0, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getDataExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> BusInterfaceItem::getDataExtension() const
{
    return dataGroup_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::itemChange()
//-----------------------------------------------------------------------------
QVariant BusInterfaceItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        return snapPointToGrid(value.toPointF());
    }
    else if (change == ItemRotationHasChanged)
    {
        getNameLabel()->setRotation(-rotation());

        dataGroup_->setDirection(getDirection());
    }
    else if (change == ItemScenePositionHasChanged)
    {   
        dataGroup_->setPosition(value.toPointF());

        foreach (GraphicsConnection* interconnection, getConnections())
        {
            interconnection->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::mousePressEvent()
//-----------------------------------------------------------------------------
void BusInterfaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<HWColumn*>(parentItem());
    setOldPosition(scenePos());
    Q_ASSERT(oldColumn_ != 0);

    saveOldPortPositions(scene()->items());
    
    beginUpdateConnectionPositions();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void BusInterfaceItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (!sceneIsLocked())
    {
        BusInterfaceEndPoint::mouseMoveEvent(event);

        setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

        HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
        if (column)
        {
            column->onMoveItem(this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void BusInterfaceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (sceneIsLocked())
    {
        return;
    }

    HWConnectionEndpoint::mouseReleaseEvent(event);
    setZValue(0.0);

    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram && oldColumn_ != 0)
    {
        HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        // Check if the interface position was really changed.
        if (getOldPosition() != scenePos())
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, getOldPosition(), oldColumn_, diagram));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<ConnectionEndpoint*, QPointF> oldPortPositions = getOldPortPositions();
        QMap<ConnectionEndpoint*, QPointF>::iterator cur = oldPortPositions.begin();
        while (cur != oldPortPositions.end())
        {
            ConnectionEndpoint* endPointItem = cur.key();
            if (endPointItem->isHierarchical() && endPointItem->scenePos() != cur.value())
            {
                HWColumn* itemColumn = dynamic_cast<HWColumn*>(endPointItem->parentItem());
                if (itemColumn)
                {
                    new ItemMoveCommand(cur.key(), cur.value(), itemColumn, diagram, cmd.data());
                }
            }

            ++cur;
        }

        clearOldPortPositions();

        // End the position update for all connections.
        foreach (QGraphicsItem *item, scene()->items())
        {
            GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

            if (conn != 0)
            {
                conn->endUpdatePosition(cmd.data());
            }
        }

        // Add the undo command to the edit stack only if it has changes.
        if (cmd->childCount() > 0 || getOldPosition() != scenePos())
        {
            diagram->getEditProvider()->addCommand(cmd);
            cmd->redo();
        }

        oldColumn_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getDirectionInShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceItem::getDirectionInShape() const
{
    int squareSize = GridSize;
    
    /*  /\
     *  ||
     */
    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize)
        << QPointF(-squareSize/2, -squareSize / 2)
        << QPointF(0, -squareSize)
        << QPointF(squareSize/2, -squareSize / 2)
        << QPointF(squareSize/2, squareSize);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getDirectionOutShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceItem::getDirectionOutShape() const
{
    int squareSize = GridSize;

    /*  ||
     *  \/
     */
    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize / 2)
        << QPointF(-squareSize/2, -squareSize)
        << QPointF(squareSize/2, -squareSize)
        << QPointF(squareSize/2, squareSize / 2)
        << QPointF(0, squareSize);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getDirectionInOutShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceItem::getDirectionInOutShape() const
{
    int squareSize = GridSize;

    QPolygonF shape;
    /*  /\
     *  ||
     *  \/
     */
    shape << QPointF(-squareSize/2, squareSize / 2)
        << QPointF(-squareSize/2, -squareSize / 2)
        << QPointF(0, -squareSize)
        << QPointF(squareSize/2, -squareSize / 2)
        << QPointF(squareSize/2, squareSize / 2)
        << QPointF(0, squareSize);

    return shape;
}