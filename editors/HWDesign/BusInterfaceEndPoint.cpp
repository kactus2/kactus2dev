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
    LibraryInterface* library, QGraphicsItem* parent, QVector2D const& dir) :
    HWConnectionEndpoint(busIf->name(), component, parent, dir),
    busInterface_(busIf),
    library_(library)
{
    portMapWarning_->setVisible(false);
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
    QPolygonF shape;

    DirectionTypes::Direction direction =
        InterfaceGraphics::getInterfaceDirection(busInterface_, getOwnerComponent());
    TransactionalTypes::Initiative initiative =
        InterfaceGraphics::getInterfaceInitiative(busInterface_, getOwnerComponent());

    if (direction == DirectionTypes::DIRECTION_INVALID && initiative != TransactionalTypes::INITIATIVE_INVALID)
    {
        shape = getInterfaceShapeWithInitiative(initiative);
    }
    else
    {
        shape = getInterfaceShapeWithDirection(direction);
    }

    if (busInterface_->getAllPortMaps()->isEmpty())
    {
        portMapWarning_->setVisible(true);
    }
    else
    {
        portMapWarning_->setVisible(false);
    }

    setPolygon(shape);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getInterfaceShapeWithDirection()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getInterfaceShapeWithDirection(DirectionTypes::Direction direction) const
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
// Function: BusInterfaceEndPoint::getInterfaceShapeWithInitiative()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getInterfaceShapeWithInitiative(TransactionalTypes::Initiative initiative) const
{
    if (initiative == TransactionalTypes::PROVIDES)
    {
        return getInitiativeProvidesShape();
    }
    else if (initiative == TransactionalTypes::REQUIRES)
    {
        return getInitiativeRequiresShape();
    }
    else
    {
        return getInitiativeRequiresProvidesShape();
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
        return QString();
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
// Function: BusInterfaceEndPoint::setDirection()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setDirection(QVector2D const& dir)
{
    portMapWarning_->moveSymbol(dir);

    HWConnectionEndpoint::setDirection(dir);
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
    for (QGraphicsItem* item : items)
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

    auto diagram = dynamic_cast<DesignDiagram*>(scene());
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
            endUpdateConnectionPositions(moveCommand.data());

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
    if (!HWConnectionEndpoint::isConnectionValid(other) || !other->isBus() || other->getBusInterface() == nullptr)
    {
        return false;
    }

    return canConnectToInterface(other);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::onConnect()
//-----------------------------------------------------------------------------
bool BusInterfaceEndPoint::onConnect(ConnectionEndpoint const* other)
{
    updateInterface();

    return other != nullptr;
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

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getInitiativeProvidesShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getInitiativeProvidesShape() const
{
    int squareSize = GridSize;

    QPolygonF shape;

    shape << QPointF(-squareSize / 2, squareSize * 3 / 4)
        << QPointF(-squareSize / 2, -squareSize / 4)
        << QPointF(-squareSize / 4, -squareSize / 4)
        << QPointF(-squareSize / 4, -squareSize / 2)
        << QPointF(squareSize / 4, -squareSize / 2)
        << QPointF(squareSize / 4, -squareSize / 4)
        << QPointF(squareSize / 2, -squareSize / 4)
        << QPointF(squareSize / 2, squareSize * 3 / 4);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getInitiativeRequiresShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getInitiativeRequiresShape() const
{
    int squareSize = GridSize;

    QPolygonF shape;

    shape << QPointF(-squareSize / 2, squareSize * 3 / 4)
        << QPointF(-squareSize / 2, -squareSize / 2)
        << QPointF(-squareSize / 4, -squareSize / 2)
        << QPointF(-squareSize / 4, -squareSize / 4)
        << QPointF(squareSize / 4, -squareSize / 4)
        << QPointF(squareSize / 4, -squareSize / 2)
        << QPointF(squareSize / 2, -squareSize / 2)
        << QPointF(squareSize / 2, squareSize * 3 / 4);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getInitiativeRequiresProvidesShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getInitiativeRequiresProvidesShape() const
{
    int squareSize = GridSize;

    QPolygonF shape;

    shape << QPointF(-squareSize / 2, squareSize * 3 / 4)
        << QPointF(-squareSize / 2, -squareSize / 4)
        << QPointF(-squareSize / 4, -squareSize / 4)
        << QPointF(-squareSize / 4, -squareSize / 2)
        << QPointF(squareSize / 4, -squareSize / 2)
        << QPointF(squareSize / 4, -squareSize / 4)
        << QPointF(squareSize / 2, -squareSize / 4)
        << QPointF(squareSize / 2, squareSize * 3 / 4)
        << QPointF(squareSize / 4, squareSize * 3 / 4)
        << QPointF(squareSize / 4, squareSize / 2)
        << QPointF(-squareSize / 4, squareSize / 2)
        << QPointF(-squareSize / 4, squareSize * 3 / 4);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getLibraryAccess()
//-----------------------------------------------------------------------------
LibraryInterface* BusInterfaceEndPoint::getLibraryAccess() const
{
    return library_;
}
