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

#include "SWCompItem.h"
#include "HWMappingItem.h"
#include "SWConnection.h"
#include "SWInterfaceItem.h"

#include "SystemMoveCommands.h"

#include <QLinearGradient>
#include <QPen>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

#include "SystemColumnLayout.h"

#include <common/layouts/VStackedLayout.h>
#include <common/layouts/VCollisionLayout.h>
#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>
#include <common/GenericEditProvider.h>
#include <common/DesignDiagram.h>

#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: SystemColumn()
//-----------------------------------------------------------------------------
SystemColumn::SystemColumn(ColumnDesc const& desc, SystemColumnLayout* layout, QGraphicsScene* scene)
    : QGraphicsRectItem(0, scene),
      layout_(layout),
      desc_(),
      nameLabel_(0)
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
    
    setColumnDesc(desc);
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
    desc_.setName(name);
    updateNameLabel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: addItem()
//-----------------------------------------------------------------------------
void SystemColumn::addItem(QGraphicsItem* item, bool load)
{
    // Remove the item from the previous column.
    if (item->parentItem() != 0)
    {
        dynamic_cast<IGraphicsItemStack*>(item->parentItem())->removeItem(item);
    }

    // Map the position to the column's local coordinate system
    // and constrain the item to the horizontal center of the column.
    QPointF pos = mapFromScene(item->scenePos());
    pos.setX(desc_.getWidth() / 2.0);

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

        if (desc_.getContentType() == COLUMN_CONTENT_IO)
        {
            VCollisionLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, IO_SPACING);
            VCollisionLayout::setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT, IO_SPACING);
        }
        else
        {
            VStackedLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, SPACING);
            VStackedLayout::setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: removeItem()
//-----------------------------------------------------------------------------
void SystemColumn::removeItem(QGraphicsItem* item)
{
    items_.removeAll(item);
    setParentItem(0);

    if (desc_.getContentType() != COLUMN_CONTENT_IO)
    {
        VStackedLayout::updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: setOffsetY()
//-----------------------------------------------------------------------------
void SystemColumn::setOffsetY(qreal y)
{
    // Update the rectangle and the label position.
    setRect(0, y, desc_.getWidth(), HEIGHT);
    nameLabel_->setPos(nameLabel_->x(), 5 + y);
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void SystemColumn::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
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

    // End position update for the connections.
    foreach (SWConnection* conn, conns_)
    {
        conn->endUpdatePosition(cmd.data());
    }

    conns_.clear();

    if (cmd->childCount() > 0 || pos() != oldPos_)
    {
        static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: onMoveItem()
//-----------------------------------------------------------------------------
void SystemColumn::onMoveItem(QGraphicsItem* item)
{
    // Restrict the position so that the item cannot be placed too high.
    //item->setPos(snapPointToGrid(item->x(), std::max(MIN_Y_PLACEMENT - item->boundingRect().top(), item->y())));
    if (desc_.getContentType() == COLUMN_CONTENT_IO)
    {
        VCollisionLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, IO_SPACING);
    }
    else
    {
        VStackedLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, SPACING);
    }

    // Check if any HW mapping item is under the item.
    foreach (QGraphicsItem* childItem, items_)
    {
        IGraphicsItemStack* childStack = dynamic_cast<IGraphicsItemStack*>(childItem);

        if (childStack != 0 && childStack->isItemAllowed(item))
        {
            QRectF intersection = childItem->sceneBoundingRect().intersected(item->sceneBoundingRect());

            if (intersection.height() >= 3 * GridSize)
            {
                // Switch the mapping item as the parent.
                items_.removeAll(item);

                if (desc_.getContentType() != COLUMN_CONTENT_IO)
                {
                    VStackedLayout::updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
                }

                setZValue(0.0);

                QPointF newPos = childStack->mapStackFromScene(item->scenePos());
                item->setParentItem(childItem);
                item->setPos(newPos);
                item->setFlag(ItemStacksBehindParent, false);

                // And call its onMoveItem().
                childStack->onMoveItem(item);
                return;
            }
        }
    }

    // If none of the child components handled the movement, determine the column under
    // the item's current position.
    SystemColumn* column = layout_->findColumnAt(item->scenePos());

    // Check if the column is different than the current one.
    if (column != 0 && column != this && column->isItemAllowed(item))
    {
        // We have to switch the column and update this column's item positions
        // without the moving item.
        items_.removeAll(item);

        QPointF newPos = column->mapFromScene(item->scenePos());
        item->setParentItem(column);
        item->setPos(newPos);

        if (desc_.getContentType() != COLUMN_CONTENT_IO)
        {
            VStackedLayout::updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
        }

        setZValue(0.0);

        // And call the new column's onMoveItem().
        column->onMoveItem(item);
        return;
    }

    setZValue(1001.0);
}

//-----------------------------------------------------------------------------
// Function: onReleaseItem()
//-----------------------------------------------------------------------------
void SystemColumn::onReleaseItem(QGraphicsItem* item)
{
    setZValue(0.0);
    
    if (desc_.getContentType() == COLUMN_CONTENT_IO)
    {
        VCollisionLayout::setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT, IO_SPACING);
    }
    else
    {
        VStackedLayout::setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: updateItemPositions()
//-----------------------------------------------------------------------------
void SystemColumn::updateItemPositions()
{
    // Just update the item positions.
    if (desc_.getContentType() != COLUMN_CONTENT_IO)
    {
        VStackedLayout::updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& SystemColumn::getName() const
{
    return desc_.getName();
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
    foreach (QGraphicsItem* item, items_)
    {
        foreach (QGraphicsItem* childItem, item->childItems())
        {
            if (childItem->type() == SWCompItem::Type)
            {
                foreach (QGraphicsItem* childItem2, childItem->childItems())
                {
                    if (childItem2->type() == SWPortItem::Type)
                    {
                        SWConnectionEndpoint* endpoint = static_cast<SWConnectionEndpoint*>(childItem2);

                        foreach (SWConnection* conn, endpoint->getConnections())
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
            else if (dynamic_cast<SWConnectionEndpoint*>(childItem))
            {
                SWConnectionEndpoint* endpoint = static_cast<SWConnectionEndpoint*>(childItem);

                foreach (SWConnection* conn, endpoint->getConnections())
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

//-----------------------------------------------------------------------------
// Function: SystemColumn::mapStackToScene()
//-----------------------------------------------------------------------------
QPointF SystemColumn::mapStackToScene(QPointF const& pos) const
{
    return mapToScene(pos);
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::isItemAllowed()
//-----------------------------------------------------------------------------
bool SystemColumn::isItemAllowed(QGraphicsItem* item) const
{
    if (desc_.getContentType() == COLUMN_CONTENT_IO)
    {
        return (item->type() == SWInterfaceItem::Type);
    }
    else
    {
        return (item->type() == SWCompItem::Type || item->type() == HWMappingItem::Type);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::mapStackFromScene()
//-----------------------------------------------------------------------------
QPointF SystemColumn::mapStackFromScene(QPointF const& pos) const
{
    return mapFromScene(pos);
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::getLayout()
//-----------------------------------------------------------------------------
SystemColumnLayout& SystemColumn::getLayout()
{
    return *layout_;
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::getContentType()
//-----------------------------------------------------------------------------
ColumnContentType SystemColumn::getContentType() const
{
    return desc_.getContentType();
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::getColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc const& SystemColumn::getColumnDesc() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::setColumnDesc()
//-----------------------------------------------------------------------------
void SystemColumn::setColumnDesc(ColumnDesc const& desc)
{
    desc_ = desc;
    setRect(0, 0, desc_.getWidth(), HEIGHT);
    layout_->updateColumnPositions();

    updateNameLabel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::updateNameLabel()
//-----------------------------------------------------------------------------
void SystemColumn::updateNameLabel()
{
    nameLabel_->setHtml("<center>" + desc_.getName() + "</center>");
    nameLabel_->setTextWidth(std::max<unsigned int>(140, desc_.getWidth()));
    nameLabel_->setPos((desc_.getWidth() - nameLabel_->textWidth()) / 2.0, 5.0);
}
