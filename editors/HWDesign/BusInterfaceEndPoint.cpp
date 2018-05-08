//-----------------------------------------------------------------------------
// File: BusInterfaceEndPoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.02.2018
//
// Description:
// Parent interface class for HW bus interfaces.
//-----------------------------------------------------------------------------

#include "BusInterfaceEndPoint.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/IEditProvider.h>

#include <editors/common/DesignDiagram.h>
#include <editors/common/diagramgrid.h>
#include <editors/HWDesign/InterfaceGraphics.h>
#include <editors/HWDesign/HWMoveCommands.h>
#include <editors/HWDesign/columnview/HWColumn.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::BusInterfaceEndPoint()
//-----------------------------------------------------------------------------
BusInterfaceEndPoint::BusInterfaceEndPoint(QSharedPointer<BusInterface> busIf, QSharedPointer<Component> component,
    QGraphicsItem *parent, QVector2D const& dir):
HWConnectionEndpoint(busIf->name(), component, parent, dir),
busInterface_(busIf),
oldPos_()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::~BusInterfaceEndPoint()
//-----------------------------------------------------------------------------
BusInterfaceEndPoint::~BusInterfaceEndPoint()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> BusInterfaceEndPoint::getBusInterface() const
{
    return busInterface_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::updateEndPointGraphics()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::updateEndPointGraphics()
{
    DirectionTypes::Direction direction =
        InterfaceGraphics::getInterfaceDirection(busInterface_, getOwnerComponent());

    QPolygonF shape = getInterfaceShape(direction);
    setPolygon(shape);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getInterfaceShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getInterfaceShape(DirectionTypes::Direction direction) const
{
    if (direction == DirectionTypes::IN)
    {
        if (isHierarchical())
        {
            return getDirectionOutShape();
        }
        else
        {
            return getDirectionInShape();
        }
    }
    else if (direction == DirectionTypes::OUT)
    {
        if (isHierarchical())
        {
            return getDirectionInShape();
        }
        else
        {
            return getDirectionOutShape();
        }
    }
    else
    {
        return getDirectionInOutShape();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::name()
//-----------------------------------------------------------------------------
QString BusInterfaceEndPoint::name() const
{
    return busInterface_->name();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setName()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setName(QString const& name)
{
    beginUpdateConnectionNames();

    QString previousName = busInterface_->name();

    busInterface_->setName(name);

    updateName(previousName, name);

    updateInterface();
    emit contentChanged();

    endUpdateConnectionNames();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setBusInterface()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setBusInterface(QSharedPointer<BusInterface> newBus)
{
    busInterface_ = newBus;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::description()
//-----------------------------------------------------------------------------
QString BusInterfaceEndPoint::description() const
{
    if (busInterface_)
    {
        return busInterface_->description();
    }
    else
    {
        return QString("");
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setDescription()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setDescription(QString const& description)
{
    if (busInterface_)
    {
        busInterface_->setDescription(description);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::isExclusive()
//-----------------------------------------------------------------------------
bool BusInterfaceEndPoint::isExclusive() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::isBus()
//-----------------------------------------------------------------------------
bool BusInterfaceEndPoint::isBus() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getType()
//-----------------------------------------------------------------------------
ConnectionEndpoint::EndpointType BusInterfaceEndPoint::getType() const
{
    return ConnectionEndpoint::ENDPOINT_TYPE_BUS;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setInterfaceMode()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setInterfaceMode(General::InterfaceMode mode)
{
    if (busInterface_)
    {
        busInterface_->setInterfaceMode(mode);
        updateInterface();
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceEndPoint::getInterfaceMode() const
{
    return busInterface_->getInterfaceMode();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getOldPosition()
//-----------------------------------------------------------------------------
QPointF BusInterfaceEndPoint::getOldPosition() const
{
    return oldPos_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setOldPosition()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setOldPosition(QPointF const& newPosition)
{
    oldPos_ = newPosition;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::saveOldPortPositions()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::saveOldPortPositions(QList<QGraphicsItem*> items)
{
    foreach (QGraphicsItem* item, items)
    {
        ConnectionEndpoint* endPointItem = dynamic_cast<ConnectionEndpoint*>(item);
        if (endPointItem && item != this && endPointItem->isHierarchical() == isHierarchical())
        {
            ConnectionEndpoint* port = static_cast<ConnectionEndpoint*>(item);
            oldPortPositions_.insert(port, port->pos());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getOldPortPositions()
//-----------------------------------------------------------------------------
QMap<ConnectionEndpoint*, QPointF> BusInterfaceEndPoint::getOldPortPositions() const
{
    return oldPortPositions_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::clearOldPortPositions()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::clearOldPortPositions()
{
    oldPortPositions_.clear();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::beginUpdateConnectionPositions()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::beginUpdateConnectionPositions()
{
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0)
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (sceneIsLocked())
    {
        return;
    }

    HWConnectionEndpoint::mouseReleaseEvent(event);
    setZValue(0.0);

    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram)
    {
        QSharedPointer<QUndoCommand> moveCommand = createMouseMoveCommand(diagram);
        if (moveCommand)
        {
            QMap<ConnectionEndpoint*, QPointF> oldPortPositions = getOldPortPositions();
            QMap<ConnectionEndpoint*, QPointF>::iterator cur = oldPortPositions.begin();
            while (cur != oldPortPositions.end())
            {
                createMoveCommandForClashedItem(cur.key(), cur.value(), diagram, moveCommand);
                ++cur;
            }

            clearOldPortPositions();

            // End the position update for all connections.
            foreach (QGraphicsItem *item, scene()->items())
            {
                GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

                if (conn != 0)
                {
                    conn->endUpdatePosition(moveCommand.data());
                }
            }

            // Add the undo command to the edit stack only if it has changes.
            if (moveCommand->childCount() > 0 || getOldPosition() != getCurrentPosition())
            {
                diagram->getEditProvider()->addCommand(moveCommand);
                moveCommand->redo();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::isConnectionValid()
//-----------------------------------------------------------------------------
bool BusInterfaceEndPoint::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!HWConnectionEndpoint::isConnectionValid(other) || !other->isBus() || other->getBusInterface() == 0)
    {
        return false;
    }

    return canConnectToInterface(other);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::onDisconnect()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::onDisconnect(ConnectionEndpoint const*)
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::onConnect()
//-----------------------------------------------------------------------------
bool BusInterfaceEndPoint::onConnect(ConnectionEndpoint const* other)
{
    updateInterface();

    return other != 0;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getDirectionOutShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getDirectionOutShape() const
{
    int squareSize = GridSize;
    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize/2)
        << QPointF(-squareSize/2, 0)
        << QPointF(0, -squareSize/2)
        << QPointF(squareSize/2, 0)
        << QPointF(squareSize/2, squareSize/2);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getDirectionInShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getDirectionInShape() const
{
    QPolygonF shape;

    int squareSize = GridSize;

    shape << QPointF(-squareSize/2, 0)
        << QPointF(-squareSize/2, -squareSize/2)
        << QPointF(squareSize/2, -squareSize/2)
        << QPointF(squareSize/2, 0)
        << QPointF(0, squareSize/2);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getDirectionInOutShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getDirectionInOutShape() const
{
    int squareSize = GridSize;

    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize/2)
        << QPointF(-squareSize/2, 0)
        << QPointF(0, -squareSize/2)
        << QPointF(squareSize/2, 0)
        << QPointF(squareSize/2, squareSize/2)
        << QPointF(0, squareSize);

    return shape;
}
