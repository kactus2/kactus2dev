//-----------------------------------------------------------------------------
// File: SystemColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.5.2011
//
// Description:
// System column class for managing SW mapping components in a system design.
//-----------------------------------------------------------------------------

#include "SystemColumn.h"

#include "EndpointDesignDiagram.h"
#include "SystemMoveCommands.h"

#include <QLinearGradient>
#include <QPen>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "SystemColumnLayout.h"
#include "MappingComponentItem.h"
#include "EndpointConnection.h"

#include <common/layouts/VStackedLayout.h>
#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>
#include <common/GenericEditProvider.h>

#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: SystemColumn()
//-----------------------------------------------------------------------------
SystemColumn::SystemColumn(QString const& name, SystemColumnLayout* layout,
                           QGraphicsScene* scene) : QGraphicsRectItem(0, scene),
                                                    layout_(layout), name_(), nameLabel_(0)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setBrush(QBrush(QColor(210, 210, 210)));
    setPen(QPen(Qt::black, 1));

    // Update the name label.
    nameLabel_ = new QGraphicsTextItem(this);
    QFont font = nameLabel_->font();
    font.setBold(true);
    nameLabel_->setFont(font);
    setName(name);
}

//-----------------------------------------------------------------------------
// Function: ~SystemColumn()
//-----------------------------------------------------------------------------
SystemColumn::~SystemColumn()
{
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void SystemColumn::setName(QString const& name)
{
    nameLabel_->setHtml("<center>" + name + "</center>");
    nameLabel_->setTextWidth(std::max<unsigned int>(140, WIDTH));
    nameLabel_->setPos((WIDTH - nameLabel_->textWidth()) / 2.0, 5.0);

    name_ = name;
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: addItem()
//-----------------------------------------------------------------------------
void SystemColumn::addItem(MappingComponentItem* item, bool load)
{
    // Map the position to the column's local coordinate system
    // and constrain the item to the horizontal center of the column.
    QPointF pos = mapFromScene(item->scenePos());
    pos.setX(WIDTH / 2.0);

    item->setPos(pos);
    item->setFlag(ItemStacksBehindParent);

    // Set the column as the parent of the item.
    item->setParentItem(this);

    if (load)
    {
        // Place the item at the correct index in the items list.
        for (int i = 0; i < items_.size(); ++i)
        {
            if (item->y() < items_.at(i)->y())
            {
                items_.insert(i, item);
                return;
            }
        }

        // If we got here, the item must be placed at the end of the list.
        items_.append(item);
    }
    else
    {
        items_.append(item);

        VStackedLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, SPACING);
        VStackedLayout::setItemPos(items_, item, WIDTH / 2, MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: removeItem()
//-----------------------------------------------------------------------------
void SystemColumn::removeItem(MappingComponentItem* item)
{
    items_.removeAll(item);
    setParentItem(0);
    VStackedLayout::updateItemPositions(items_, WIDTH / 2, MIN_Y_PLACEMENT, SPACING);
}

//-----------------------------------------------------------------------------
// Function: setOffsetY()
//-----------------------------------------------------------------------------
void SystemColumn::setOffsetY(qreal y)
{
    // Update the rectangle and the label position.
    setRect(0, y, WIDTH, HEIGHT);
    nameLabel_->setPos(nameLabel_->x(), 5 + y);
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void SystemColumn::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<EndpointDesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    QGraphicsRectItem::mouseMoveEvent(event);

    setZValue(1001.0);
    layout_->onMoveColumn(this);
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SystemColumn::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);

    setZValue(0.0);
    layout_->onReleaseColumn(this);

    // Create an undo command only if the position really changed.
    QSharedPointer<QUndoCommand> cmd;

    if (pos() != oldPos_)
    {
        cmd = QSharedPointer<QUndoCommand>(new SystemColumnMoveCommand(layout_, this, oldPos_));
    }
    else
    {
        cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    // End position update for the interconnections.
    foreach (EndpointConnection* conn, conns_)
    {
        conn->endUpdatePosition(cmd.data());
    }

    conns_.clear();

    if (cmd->childCount() > 0 || pos() != oldPos_)
    {
        static_cast<EndpointDesignDiagram*>(scene())->getEditProvider().addCommand(cmd);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: onMoveItem()
//-----------------------------------------------------------------------------
void SystemColumn::onMoveItem(MappingComponentItem* item, SystemColumn* oldColumn)
{
    // Determine the column under the item's current position.
    SystemColumn* column = layout_->findColumnAt(item->scenePos());

    // Check if the column is different than the current one.
    if (column != 0 && column != this)
    {
        // We have to switch the column and update this column's item positions
        // without the moving item.
        items_.removeAll(item);

        QPointF newPos = column->mapFromScene(item->scenePos());
        item->setParentItem(column);
        item->setPos(newPos);

        VStackedLayout::updateItemPositions(items_, WIDTH / 2, MIN_Y_PLACEMENT, SPACING);
        setZValue(0.0);

        // And call the new column's onMoveItem().
        column->onMoveItem(item, oldColumn);
        return;
    }

    setZValue(1001.0);

    // Restrict the position so that the item cannot be placed too high.
    item->setPos(snapPointToGrid(item->x(), std::max(MIN_Y_PLACEMENT - item->boundingRect().top(), item->y())));
    VStackedLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, SPACING);
}

//-----------------------------------------------------------------------------
// Function: onReleaseItem()
//-----------------------------------------------------------------------------
void SystemColumn::onReleaseItem(MappingComponentItem* item)
{
    setZValue(0.0);
    VStackedLayout::setItemPos(items_, item, WIDTH / 2, MIN_Y_PLACEMENT, SPACING);
}

//-----------------------------------------------------------------------------
// Function: updateItemPositions()
//-----------------------------------------------------------------------------
void SystemColumn::updateItemPositions()
{
    // Just update the item positions.
    VStackedLayout::updateItemPositions(items_, WIDTH / 2, MIN_Y_PLACEMENT, SPACING);
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& SystemColumn::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: isEmpty()
//-----------------------------------------------------------------------------
bool SystemColumn::isEmpty() const
{
    return items_.empty();
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant SystemColumn::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
    case ItemPositionChange:
        {
            return QPointF(value.toPointF().x(), 0.0);
        }
    }

    return QGraphicsRectItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void SystemColumn::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mousePressEvent(event);

    oldPos_ = pos();

    // Begin position update for the connections.
    foreach (MappingComponentItem* item, items_)
    {
        foreach (QGraphicsItem* childItem, item->childItems())
        {
            if (childItem->type() == ProgramEntityItem::Type)
            {
                ProgramEntityItem* progEntity = static_cast<ProgramEntityItem*>(childItem);

                foreach (EndpointItem* endpoint, progEntity->getEndpoints())
                {
                    foreach (EndpointConnection* conn, endpoint->getConnections())
                    {
                        if (!conns_.contains(conn))
                        {
                            conn->beginUpdatePosition();
                            conns_.insert(conn);
                        }
                    }
                }
            }
        }
    }
}
