//-----------------------------------------------------------------------------
// File: HierarchicalPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "HierarchicalPortItem.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/DesignDiagram.h>
#include <editors/common/GraphicsItemLabel.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

#include <QBrush>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::HierarchicalPortItem()
//-----------------------------------------------------------------------------
HierarchicalPortItem::HierarchicalPortItem(QSharedPointer<Component> component, QSharedPointer<Port> port,
    QSharedPointer<Kactus2Placeholder> dataGroup, QGraphicsItem* parent):
AdHocItem(port, component, parent, QVector2D(1.0f, 0.0f)),
dataGroup_(dataGroup),
oldColumn_(0),
oldInterfacePositions_(),
oldPos_()
{
    if (dataGroup_->getAttributeValue("portName").isEmpty())
    {
        dataGroup_->setAttribute("portName", getPort()->name());
    }

    if (!dataGroup_->getAttributeValue("x").isEmpty())
    {
        QPointF position(dataGroup_->getAttributeValue("x").toInt(), dataGroup_->getAttributeValue("y").toInt());
        setPos(position);
    }

    getNameLabel()->setRotation(-rotation());

    setPolygon(getPortShape());

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::~HierarchicalPortItem()
//-----------------------------------------------------------------------------
HierarchicalPortItem::~HierarchicalPortItem()
{

}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::isHierarchical()
//-----------------------------------------------------------------------------
bool HierarchicalPortItem::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::setDirection()
//-----------------------------------------------------------------------------
void HierarchicalPortItem::setDirection(QVector2D const& dir)
{
    HWConnectionEndpoint::setDirection(dir);
    setLabelPosition();
    setTieOffLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool HierarchicalPortItem::isDirectionFixed() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::setLabelPosition()
//-----------------------------------------------------------------------------
void HierarchicalPortItem::setLabelPosition()
{
    qreal nameWidth = getNameLabel()->boundingRect().width();
    qreal nameHeight = getNameLabel()->boundingRect().height();

    // Check if the port is directed to the left.
    if (getDirection().x() < 0)
    {
        getNameLabel()->setPos(-nameHeight / 2 + 4, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the port is directed to the right.
    else
    {
        getNameLabel()->setPos(nameHeight / 2 - 4, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::itemChange()
//-----------------------------------------------------------------------------
QVariant HierarchicalPortItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        return snapPointToGrid(value.toPointF());
    }
    else if (change == ItemRotationHasChanged)
    {
        getNameLabel()->setRotation(-rotation());
        if (getTieOffLabel() != 0)
        {
            getTieOffLabel()->setRotation(-rotation());
        }
    }
    else if (change == ItemScenePositionHasChanged)
    {
        dataGroup_->setAttribute("x", QString::number(value.toPointF().x()));
        dataGroup_->setAttribute("y", QString::number(value.toPointF().y()));

        foreach (GraphicsConnection* interconnection, getConnections())
        {
            interconnection->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::saveOldPortPositions()
//-----------------------------------------------------------------------------
void HierarchicalPortItem::saveOldPortPositions()
{
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<GraphicsColumn*>(parentItem());
    oldPos_ = scenePos();
    Q_ASSERT(oldColumn_ != 0);

    foreach (QGraphicsItem* item, scene()->items()) //column->childItems())
    {
        if (item->type() == GFX_TYPE_DIAGRAM_INTERFACE || item->type() == HierarchicalPortItem::Type)
        {
            oldInterfacePositions_.insert(item, item->scenePos());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::moveItemByMouse()
//-----------------------------------------------------------------------------
void HierarchicalPortItem::moveItemByMouse()
{
    setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

    GraphicsColumn* column = dynamic_cast<GraphicsColumn*>(parentItem());
    Q_ASSERT(column != 0);
    column->onMoveItem(this);
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void HierarchicalPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mouseReleaseEvent(event);
    setZValue(0.0);

    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram && oldColumn_ != 0)
    {
        GraphicsColumn* column = dynamic_cast<GraphicsColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        // Check if the interface position was really changed.
        if (oldPos_ != scenePos())
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldColumn_, diagram));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<QGraphicsItem*, QPointF>::iterator cur = oldInterfacePositions_.begin();
        while (cur != oldInterfacePositions_.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                new ItemMoveCommand(cur.key(), cur.value(), oldColumn_, diagram, cmd.data());
            }

            cur++;
        }

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
        if (cmd->childCount() > 0 || oldPos_ != scenePos())
        {
            diagram->getEditProvider()->addCommand(cmd);
            cmd->redo();
        }

        oldColumn_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::getDataGroup()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Placeholder> HierarchicalPortItem::getDataGroup() const
{
    return dataGroup_;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::setDataGroup()
//-----------------------------------------------------------------------------
void HierarchicalPortItem::setDataGroup(QSharedPointer<Kactus2Placeholder> newDataGroup)
{
    dataGroup_ = newDataGroup;
}

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItem::labelShouldBeDrawnLeft()
//-----------------------------------------------------------------------------
bool HierarchicalPortItem::labelShouldBeDrawnLeft() const
{
    return getDirection().x() < 0;
}
