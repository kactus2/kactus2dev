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
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: GraphicsColumn()
//-----------------------------------------------------------------------------
GraphicsColumn::GraphicsColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout, QGraphicsScene* scene)
    : QGraphicsRectItem(0, scene),
      layout_(layout),
      desc_(),
      nameLabel_(0),
      itemLayout_(0),
      items_(),
      oldPos_(),
      mouseNearResizeArea_(false),
      oldWidth_(0)
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

    // Set the column as the parent of the item.
    item->setParentItem(this);

    item->setPos(pos);
    item->setFlag(ItemStacksBehindParent);

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

        if (itemLayout_ != 0)
        {
            itemLayout_->updateItemMove(items_, item, MIN_Y_PLACEMENT);
            itemLayout_->setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: removeItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::removeItem(QGraphicsItem* item)
{
    items_.removeAll(item);
    item->setParentItem(0);

    if (itemLayout_ != 0)
    {
        itemLayout_->updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT);
    }
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
        setWidth(qMax<int>(0, snappedRight - 1));
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
    if (mouseNearResizeArea_)
    {
        if (desc_.getWidth() != oldWidth_)
        {
            QSharedPointer<QUndoCommand> cmd(new GraphicsColumnResizeCommand(this, oldWidth_));
            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }
    }
    else
    {
        QGraphicsRectItem::mouseReleaseEvent(event);

        setZValue(0.0);
        layout_->onReleaseColumn(this);

        // Create an undo command.
        QSharedPointer<QUndoCommand> cmd = createMoveUndoCommand();

        // Add the undo command to the edit provider only if there were any actual changes.
        if (cmd->childCount() > 0 || pos() != oldPos_)
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onMoveItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::onMoveItem(QGraphicsItem* item)
{
    // Restrict the position so that the item cannot be placed too high.
    //item->setPos(snapPointToGrid(item->x(), qMax(MIN_Y_PLACEMENT - item->boundingRect().top(), item->y())));

    // Update the item move based on the column content type.
    if (itemLayout_ != 0)
    {
        itemLayout_->updateItemMove(items_, item, MIN_Y_PLACEMENT);
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

                if (itemLayout_ != 0)
                {
                    itemLayout_->updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT);
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
        switchColumn(item, column);
        return;
    }

    // Check if auto-reorganize is enabled and the item needs to be moved somewhere else.
    if (layout_->isAutoReorganized())
    {
        int index = layout_->getColumns().indexOf(this);
        int newIndex = index;

        // Check if the item cannot locate in this column.
        if (!isItemAllowed(item))
        {
            // Find the next column on the right that can contain the item.
            ++newIndex;

            for (; newIndex < layout_->getColumns().size(); ++newIndex)
            {
                if (layout_->getColumns().at(newIndex)->isItemAllowed(item))
                {
                    break;
                }
            }

            // If not found, create a new one with default properties.
            if (newIndex == layout_->getColumns().size())
            {
                newIndex = layout_->autoCreateColumn();
                return;
            }
        }
//         else
//         {
//             // Otherwise check if the item can be placed to a column more on the left.
//             for (newIndex = 0; newIndex < layout_->getColumns().size(); ++newIndex)
//             {
//                 if (layout_->getColumns().at(newIndex)->isItemAllowed(item))
//                 {
//                     break;
//                 }
//             }
//         }

        if (newIndex != index)
        {
            switchColumn(item, layout_->getColumns().at(newIndex));
            return;
        }

        // Remove unused columns from the right.
        for (int index = layout_->getColumns().indexOf(this) + 1; index < layout_->getColumns().size(); ++index)
        {
            if (layout_->getColumns().at(index)->isEmpty())
            {
                layout_->removeColumn(layout_->getColumns().at(index));
            }
            else
            {
                ++index;
            }
        }
    }

    setZValue(1001.0);
}

//-----------------------------------------------------------------------------
// Function: onReleaseItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::onReleaseItem(QGraphicsItem* item)
{
    setZValue(0.0);

    if (itemLayout_ != 0)
    {
        itemLayout_->setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT);
    }
}

//-----------------------------------------------------------------------------
// Function: updateItemPositions()
//-----------------------------------------------------------------------------
void GraphicsColumn::updateItemPositions()
{
    // Just update the item positions.
    if (itemLayout_ != 0)
    {
        itemLayout_->updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT);
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
    if (mouseNearResizeArea_)
    {
        // Save the old width before resize.
        oldWidth_ = desc_.getWidth();
    }
    else
    {
        QGraphicsRectItem::mousePressEvent(event);

        oldPos_ = pos();
        prepareColumnMove();
    }
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

    if (desc.getContentType() == COLUMN_CONTENT_IO)
    {
        itemLayout_ = QSharedPointer<IVGraphicsLayout<QGraphicsItem> >(new VCollisionLayout<QGraphicsItem>(IO_SPACING));
    }
    else
    {
        itemLayout_ = QSharedPointer<IVGraphicsLayout<QGraphicsItem> >(new VStackedLayout<QGraphicsItem>(SPACING));
    }

    setRect(0, 0, desc_.getWidth(), HEIGHT);
    layout_->updateColumnPositions();

    foreach (QGraphicsItem* item, items_)
    {
        item->setX(desc_.getWidth() / 2);
    }

    updateNameLabel();
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
    nameLabel_->setTextWidth(qMax<unsigned int>(140, desc_.getWidth()));
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

    if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
        qAbs(event->pos().x() - boundingRect().right()) <= 10)
    {
        if (!mouseNearResizeArea_)
        {
            QApplication::setOverrideCursor(Qt::SplitHCursor);
            mouseNearResizeArea_ = true;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::hoverLeaveEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);

    if (mouseNearResizeArea_)
    {
        // Restore the old cursor.
        QApplication::restoreOverrideCursor();
        mouseNearResizeArea_ = false;
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::updateCursor()
//-----------------------------------------------------------------------------
void GraphicsColumn::updateCursor(QGraphicsSceneHoverEvent* event)
{
    if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
        qAbs(event->pos().x() - boundingRect().right()) <= 10)
    {
        if (!mouseNearResizeArea_)
        {
            QApplication::setOverrideCursor(Qt::SplitHCursor);
            mouseNearResizeArea_ = true;
        }
    }
    else
    {
        if (mouseNearResizeArea_)
        {
            // Restore the old cursor.
            QApplication::restoreOverrideCursor();
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

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::switchColumn()
//-----------------------------------------------------------------------------
void GraphicsColumn::switchColumn(QGraphicsItem* item, GraphicsColumn* column)
{
    // We have to switch the column and update this column's item positions
    // without the moving item.
    items_.removeAll(item);

    QPointF newPos = column->mapFromScene(item->scenePos());
    item->setParentItem(column);
    item->setPos(newPos);

    if (itemLayout_ != 0)
    {
        itemLayout_->updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT);
    }

    setZValue(0.0);

    // And call the new column's onMoveItem().
    column->onMoveItem(item);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::setItemLayout()
//-----------------------------------------------------------------------------
void GraphicsColumn::setItemLayout(QSharedPointer< IVGraphicsLayout<QGraphicsItem> > itemLayout)
{
    itemLayout_ = itemLayout;
}
