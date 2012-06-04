//-----------------------------------------------------------------------------
// File: GraphicsColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2012
//
// Description:
// Graphics column class for managing a collection of graphics items in one
// column.
//-----------------------------------------------------------------------------

#include "GraphicsColumn.h"

#include "GraphicsColumnUndoCommands.h"
#include "GraphicsColumnLayout.h"

#include <common/layouts/VStackedLayout.h>
#include <common/layouts/VCollisionLayout.h>
#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>
#include <common/GenericEditProvider.h>
#include <common/DesignDiagram.h>

#include <models/component.h>

#include <QLinearGradient>
#include <QPen>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>

//-----------------------------------------------------------------------------
// Function: GraphicsColumn()
//-----------------------------------------------------------------------------
GraphicsColumn::GraphicsColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout, QGraphicsScene* scene)
    : QGraphicsRectItem(0, scene),
      layout_(layout),
      desc_(),
      nameLabel_(0),
      oldPos_(),
      mouseNearResizeArea_(false),
      oldCursor_()
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setBrush(QBrush(QColor(210, 210, 210)));
    setPen(QPen(Qt::black, 1));
    setAcceptHoverEvents(true);

    // Update the name label.
    nameLabel_ = new QGraphicsTextItem(this);
    nameLabel_->setAcceptHoverEvents(false);
    QFont font = nameLabel_->font();
    font.setBold(true);
    nameLabel_->setFont(font);

    setColumnDesc(desc);
}

//-----------------------------------------------------------------------------
// Function: ~GraphicsColumn()
//-----------------------------------------------------------------------------
GraphicsColumn::~GraphicsColumn()
{
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void GraphicsColumn::setName(QString const& name)
{
    desc_.setName(name);
    updateNameLabel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: addItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::addItem(QGraphicsItem* item, bool load)
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

    // TODO: emit contentChanged() necessary?
}

//-----------------------------------------------------------------------------
// Function: removeItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::removeItem(QGraphicsItem* item)
{
    items_.removeAll(item);
    setParentItem(0);

    if (desc_.getContentType() != COLUMN_CONTENT_IO)
    {
        VStackedLayout::updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
    }

    // TODO: emit contentChanged() necessary?
}

//-----------------------------------------------------------------------------
// Function: setOffsetY()
//-----------------------------------------------------------------------------
void GraphicsColumn::setOffsetY(qreal y)
{
    // Update the rectangle and the label position.
    setRect(0, y, desc_.getWidth(), HEIGHT);
    nameLabel_->setPos(nameLabel_->x(), 5 + y);
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    // If the mouse is moved near the resize area, change the column's width accordingly.
    if (mouseNearResizeArea_)
    {
        qreal snappedRight = (static_cast<int>(event->pos().x() + 10.0) / 20) * 20;
        setWidth(std::max<int>(0, snappedRight - 1));
    }
    else
    {
        // Otherwise handle the movement of the column.
        QGraphicsRectItem::mouseMoveEvent(event);

        setZValue(1001.0);
        layout_->onMoveColumn(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);

    setZValue(0.0);
    layout_->onReleaseColumn(this);

    // Create an undo command.
    QSharedPointer<QUndoCommand> cmd = createMoveUndoCommand();

    // Add the undo command to the edit provider only if there were any actual changes.
    if (cmd->childCount() > 0 || pos() != oldPos_)
    {
        static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: onMoveItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::onMoveItem(QGraphicsItem* item)
{
    // Restrict the position so that the item cannot be placed too high.
    //item->setPos(snapPointToGrid(item->x(), std::max(MIN_Y_PLACEMENT - item->boundingRect().top(), item->y())));

    // Update the item move based on the column content type.
    if (desc_.getContentType() == COLUMN_CONTENT_IO)
    {
        VCollisionLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, IO_SPACING);
    }
    else
    {
        VStackedLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, SPACING);
    }

    // Check if any graphics item stack is under the item.
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
    GraphicsColumn* column = layout_->findColumnAt(item->scenePos());

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
void GraphicsColumn::onReleaseItem(QGraphicsItem* item)
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
void GraphicsColumn::updateItemPositions()
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
QString const& GraphicsColumn::getName() const
{
    return desc_.getName();
}

//-----------------------------------------------------------------------------
// Function: isEmpty()
//-----------------------------------------------------------------------------
bool GraphicsColumn::isEmpty() const
{
    return items_.empty();
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant GraphicsColumn::itemChange(GraphicsItemChange change, const QVariant &value)
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
void GraphicsColumn::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mousePressEvent(event);

    oldPos_ = pos();
    prepareColumnMove();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::mapStackToScene()
//-----------------------------------------------------------------------------
QPointF GraphicsColumn::mapStackToScene(QPointF const& pos) const
{
    return mapToScene(pos);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::mapStackFromScene()
//-----------------------------------------------------------------------------
QPointF GraphicsColumn::mapStackFromScene(QPointF const& pos) const
{
    return mapFromScene(pos);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::isItemAllowed()
//-----------------------------------------------------------------------------
bool GraphicsColumn::isItemAllowed(QGraphicsItem* item) const
{
    return isItemAllowed(item, desc_.getAllowedItems());
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::isItemAllowed()
//-----------------------------------------------------------------------------
bool GraphicsColumn::isItemAllowed(QGraphicsItem* item, unsigned int) const
{
    return (item->type() != type());
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::getLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout& GraphicsColumn::getLayout()
{
    return *layout_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::getContentType()
//-----------------------------------------------------------------------------
ColumnContentType GraphicsColumn::getContentType() const
{
    return desc_.getContentType();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::getColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc const& GraphicsColumn::getColumnDesc() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::setColumnDesc()
//-----------------------------------------------------------------------------
void GraphicsColumn::setColumnDesc(ColumnDesc const& desc)
{
    desc_ = desc;

    switch (desc.getContentType())
    {
    case COLUMN_CONTENT_IO:
        {
            desc_.setAllowedItems(CIT_INTERFACE);
            break;
        }

    case COLUMN_CONTENT_COMPONENTS:
        {
            desc_.setAllowedItems(CIT_COMPONENT);
            break;
        }

    case COLUMN_CONTENT_BUSES:
        {
            desc_.setAllowedItems(CIT_CHANNEL | CIT_BRIDGE);
            break;
        }

    default:
        {
            break;
        }
    }

    setRect(0, 0, desc_.getWidth(), HEIGHT);
    layout_->updateColumnPositions();

    foreach (QGraphicsItem* item, items_)
    {
        item->setX(desc_.getWidth() / 2);
    }

    updateNameLabel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::setWidth()
//-----------------------------------------------------------------------------
void GraphicsColumn::setWidth(unsigned int width)
{
    desc_.setWidth(width);

    setRect(0, 0, desc_.getWidth(), HEIGHT);
    layout_->updateColumnPositions();

    foreach (QGraphicsItem* item, items_)
    {
        item->setX(desc_.getWidth() / 2);
    }

    updateNameLabel();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::updateNameLabel()
//-----------------------------------------------------------------------------
void GraphicsColumn::updateNameLabel()
{
    nameLabel_->setHtml("<center>" + desc_.getName() + "</center>");
    nameLabel_->setTextWidth(std::max<unsigned int>(140, desc_.getWidth()));
    nameLabel_->setPos((desc_.getWidth() - nameLabel_->textWidth()) / 2.0, 5.0);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::hoverMoveEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverMoveEvent(event);
    updateCursor(event);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::hoverEnterEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverEnterEvent(event);
    updateCursor(event);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::hoverLeaveEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
    updateCursor(event);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::updateCursor()
//-----------------------------------------------------------------------------
void GraphicsColumn::updateCursor(QGraphicsSceneHoverEvent* event)
{
    if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
        std::abs(event->pos().x() - boundingRect().right()) <= 10)
    {
        if (!mouseNearResizeArea_ )
        {
            oldCursor_ = cursor();
            setCursor(QCursor(Qt::SplitHCursor));
            mouseNearResizeArea_ = true;
        }
    }
    else
    {
        if (mouseNearResizeArea_)
        {
            // Restore the old cursor.
            setCursor(oldCursor_);
            mouseNearResizeArea_ = false;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::prepareColumnMove()
//-----------------------------------------------------------------------------
void GraphicsColumn::prepareColumnMove()
{
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::createMoveUndoCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> GraphicsColumn::createMoveUndoCommand()
{
    if (pos() != oldPos_)
    {
        return QSharedPointer<QUndoCommand>(new GraphicsColumnMoveCommand(layout_, this, oldPos_));
    }
    else
    {
        return QSharedPointer<QUndoCommand>(new QUndoCommand());
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::getItems()
//-----------------------------------------------------------------------------
QList<QGraphicsItem*> const& GraphicsColumn::getItems() const
{
    return items_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::isAllowedItemsValid()
//-----------------------------------------------------------------------------
bool GraphicsColumn::isAllowedItemsValid(unsigned int allowedItems) const
{
    foreach (QGraphicsItem* item, items_)
    {
        if (!isItemAllowed(item, allowedItems))
        {
            return false;
        }
    }

    return true;
}
