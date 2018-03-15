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

#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::BusInterfaceItem()
//-----------------------------------------------------------------------------
BusInterfaceItem::BusInterfaceItem(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf,
    QSharedPointer<InterfaceGraphicsData> dataGroup, QGraphicsItem *parent):
BusInterfaceEndPoint(busIf, component, parent),
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
// Function: BusInterfaceItem::updateName()
//-----------------------------------------------------------------------------
void BusInterfaceItem::updateName(QString const&, QString const& newName)
{
    dataGroup_->setName(newName);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::createMoveCommandForClashedItem()
//-----------------------------------------------------------------------------
void BusInterfaceItem::createMoveCommandForClashedItem(ConnectionEndpoint* endPoint, QPointF endPointPosition,
    DesignDiagram* diagram, QSharedPointer<QUndoCommand> parentCommand)
{
    if (endPoint && endPoint->isHierarchical() && endPoint->pos() != endPointPosition)
    {
        HWColumn* itemColumn = dynamic_cast<HWColumn*>(endPoint->parentItem());
        if (itemColumn)
        {
            new ItemMoveCommand(endPoint, endPointPosition, itemColumn, diagram, parentCommand.data());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::canConnectToInterface()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::canConnectToInterface(ConnectionEndpoint const* otherEndPoint) const
{
    QSharedPointer<BusInterface> otherInterface = otherEndPoint->getBusInterface();

    return (otherInterface &&
        (getBusInterface()->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
        !otherInterface->getBusType().isValid() ||
        (otherInterface->getBusType() == getBusInterface()->getBusType())));
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
// Function: BusInterfaceItem::moveItemByMouse()
//-----------------------------------------------------------------------------
void BusInterfaceItem::moveItemByMouse()
{
    QPointF currentPosition = pos();
    QPointF mappedPosition = oldColumn_->mapToScene(pos());

    setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

    HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
    if (column)
    {
        column->onMoveItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::createMouseMoveCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> BusInterfaceItem::createMouseMoveCommand(DesignDiagram* diagram)
{
    if (!oldColumn_)
    {
        return QSharedPointer<QUndoCommand>();
    }

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

    return cmd;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getCurrentPosition()
//-----------------------------------------------------------------------------
QPointF BusInterfaceItem::getCurrentPosition() const
{
    return scenePos();
}
