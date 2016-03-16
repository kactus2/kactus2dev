//-----------------------------------------------------------------------------
// File: GraphicsColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2012
//
// Description:
// Graphics column class for managing a collection of graphics items in one column.
//-----------------------------------------------------------------------------

#include "GraphicsColumn.h"

#include "GraphicsColumnUndoCommands.h"
#include "GraphicsColumnLayout.h"

#include <common/IEditProvider.h>

#include <common/layouts/VStackedLayout.h>
#include <common/layouts/VCollisionLayout.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>
#include <designEditors/common/DesignDiagram.h>

#include <QPen>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::GraphicsColumn()
//-----------------------------------------------------------------------------
GraphicsColumn::GraphicsColumn(QSharedPointer<ColumnDesc> desc, GraphicsColumnLayout* layout)
    : QGraphicsRectItem(),
      layout_(layout),
      columnData_(),
      nameLabel_(new QGraphicsTextItem(this)),
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
    nameLabel_->setAcceptHoverEvents(false);
    QFont font = nameLabel_->font();
    font.setBold(true);
    nameLabel_->setFont(font);

    setColumnDesc(desc);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::~GraphicsColumn()
//-----------------------------------------------------------------------------
GraphicsColumn::~GraphicsColumn()
{
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::setName()
//-----------------------------------------------------------------------------
void GraphicsColumn::setName(QString const& name)
{
    columnData_->setName(name);
    updateNameLabel();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::name()
//-----------------------------------------------------------------------------
QString GraphicsColumn::name() const
{
    return columnData_->name();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::setColumnDesc()
//-----------------------------------------------------------------------------
void GraphicsColumn::setColumnDesc(QSharedPointer<ColumnDesc> desc)
{
    columnData_ = desc;

    if (desc->getContentType() == ColumnTypes::IO)
    {
        columnData_->setAllowedItems(ColumnTypes::INTERFACE);
    }
    else if (desc->getContentType() == ColumnTypes::COMPONENTS)
    {
        columnData_->setAllowedItems(ColumnTypes::COMPONENT);
    }
    else if (desc->getContentType() == ColumnTypes::BUSES)
    {
        columnData_->setAllowedItems(ColumnTypes::CHANNEL | ColumnTypes::BRIDGE);
    }

    if (desc->getContentType() == ColumnTypes::IO)
    {
        setItemLayout(QSharedPointer<IVGraphicsLayout<QGraphicsItem> >(new VCollisionLayout<QGraphicsItem>(IO_SPACING)));
    }
    else
    {
        setItemLayout(QSharedPointer<IVGraphicsLayout<QGraphicsItem> >(new VStackedLayout<QGraphicsItem>(SPACING)));
    }

    setRect(0, 0, columnData_->getWidth(), HEIGHT);
    layout_->updateColumnPositions();

    foreach (QGraphicsItem* item, items_)
    {
        item->setX(columnData_->getWidth() / 2);
    }

    updateNameLabel();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::getColumnDesc()
//-----------------------------------------------------------------------------
QSharedPointer<ColumnDesc> GraphicsColumn::getColumnDesc() const
{
    return columnData_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::setWidth()
//-----------------------------------------------------------------------------
void GraphicsColumn::setWidth(unsigned int width)
{
    columnData_->setWidth(width);

    setRect(0, 0, columnData_->getWidth(), HEIGHT);
    layout_->updateColumnPositions();

    unsigned int centerX = columnData_->getWidth() / 2;
    foreach (QGraphicsItem* item, items_)
    {
        item->setX(centerX);
    }

    updateNameLabel();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::setOffsetY()
//-----------------------------------------------------------------------------
void GraphicsColumn::setOffsetY(qreal y)
{
    // Update the rectangle and the label position.
    setRect(0, y, columnData_->getWidth(), HEIGHT);
    nameLabel_->setPos(nameLabel_->x(), 5 + y);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::getItems()
//-----------------------------------------------------------------------------
QList<QGraphicsItem*> GraphicsColumn::getItems() const
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
// Function: GraphicsColumn::isEmpty()
//-----------------------------------------------------------------------------
bool GraphicsColumn::isEmpty() const
{
    return items_.empty();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::addItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::addItem(QGraphicsItem* item, bool load)
{
    // Remove the item from the previous column.
    if (item->parentItem() != 0)
    {
        dynamic_cast<IGraphicsItemStack*>(item->parentItem())->removeItem(item);
    }

    // Constrain the item to the horizontal center of the column.
    QPointF pos = mapFromScene(item->scenePos());
    pos.setX(columnData_->getWidth() / 2.0);

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
            itemLayout_->setItemPos(items_, item, columnData_->getWidth() / 2, MIN_Y_PLACEMENT);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::removeItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::removeItem(QGraphicsItem* item)
{
    items_.removeAll(item);

    if (itemLayout_ != 0)
    {
        itemLayout_->updateItemPositions(items_, columnData_->getWidth() / 2, MIN_Y_PLACEMENT);
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::onMoveItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::onMoveItem(QGraphicsItem* item)
{
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

            if (intersection.height() >= 3*GridSize)
            {
                // Switch the mapping item as the parent.
                removeItem(item);

                setZValue(0.0);

                QPointF newPos = childStack->mapStackFromScene(item->scenePos());
                item->setParentItem(childItem);
                item->setPos(newPos);
                item->setFlag(ItemStacksBehindParent, false);

                childStack->onMoveItem(item);
                return;
            }
        }
    }

    // If none of the child components handled the movement, find the column under the item's current position.
    GraphicsColumn* column = layout_->findColumnAt(item->scenePos());
    if (column != 0 && column != this && column->isItemAllowed(item))
    {
        switchItemToColumn(item, column);
        return;
    }

    // Check if auto-reorganize is enabled and the item needs to be moved somewhere else.
    if (layout_->isAutoReorganized())
    {
        int index = layout_->getColumns().indexOf(this);
        int newIndex = index;

        for (; newIndex < layout_->getColumns().size(); newIndex++)
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

        if (newIndex != index)
        {
            switchItemToColumn(item, layout_->getColumns().at(newIndex));
            return;
        }

        // Remove unused columns from the right.
        for (int i = layout_->getColumns().indexOf(this) + 1; i < layout_->getColumns().size(); i++)
        {
            if (layout_->getColumns().at(i)->isEmpty())
            {
                layout_->removeColumn(layout_->getColumns().at(i));
                i--;
            }
        }
    }

    setZValue(1001.0);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::onReleaseItem()
//-----------------------------------------------------------------------------
void GraphicsColumn::onReleaseItem(QGraphicsItem* item)
{
    setZValue(0.0);

    if (itemLayout_ != 0)
    {
        itemLayout_->setItemPos(items_, item, columnData_->getWidth() / 2, MIN_Y_PLACEMENT);
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::updateItemPositions()
//-----------------------------------------------------------------------------
void GraphicsColumn::updateItemPositions()
{
    if (itemLayout_ != 0)
    {
        itemLayout_->updateItemPositions(items_, columnData_->getWidth() / 2, MIN_Y_PLACEMENT);
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
    return isItemAllowed(item, columnData_->getAllowedItems());
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::getContentType()
//-----------------------------------------------------------------------------
ColumnTypes::ColumnContentType GraphicsColumn::getContentType() const
{
    return columnData_->getContentType();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::mousePressEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (mouseNearResizeArea_)
    {
        // Save the old width before resize.
        oldWidth_ = columnData_->getWidth();
    }
    else
    {
        QGraphicsRectItem::mousePressEvent(event);

        oldPos_ = pos();
        prepareColumnMove();
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::mouseMoveEvent()
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
        // Snap the resize to two grid units.
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
// Function: GraphicsColumn::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (mouseNearResizeArea_ && columnData_->getWidth() != oldWidth_)
    {
        QSharedPointer<QUndoCommand> cmd(new GraphicsColumnResizeCommand(this, oldWidth_));
        static_cast<DesignDiagram*>(scene())->getEditProvider()->addCommand(cmd);
    }
    else if (!mouseNearResizeArea_)
    {
        QGraphicsRectItem::mouseReleaseEvent(event);

        setZValue(0.0);
        layout_->onReleaseColumn(this);

        QSharedPointer<QUndoCommand> undoCommand = createMoveUndoCommand();

        // Add the undo command to the edit provider only if there were any actual changes.
        if (undoCommand->childCount() > 0 || pos() != oldPos_)
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider()->addCommand(undoCommand);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::hoverEnterEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverEnterEvent(event);

    if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
        qAbs(event->pos().x() - boundingRect().right()) <= GridSize &&
        !mouseNearResizeArea_)
    {
        QApplication::setOverrideCursor(Qt::SplitHCursor);
        mouseNearResizeArea_ = true;
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::hoverMoveEvent()
//-----------------------------------------------------------------------------
void GraphicsColumn::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverMoveEvent(event);

    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    if (qAbs(event->pos().x() - boundingRect().right()) <= 10)
    {
        if (!mouseNearResizeArea_)
        {
            QApplication::setOverrideCursor(Qt::SplitHCursor);
            mouseNearResizeArea_ = true;
        }
    }
    else if (mouseNearResizeArea_)
    {
        QApplication::restoreOverrideCursor();
        mouseNearResizeArea_ = false;
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
        QApplication::restoreOverrideCursor();
        mouseNearResizeArea_ = false;
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::itemChange()
//-----------------------------------------------------------------------------
QVariant GraphicsColumn::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        QPointF position(value.toPointF().x(), 0.0);
        columnData_->setPosition(position.x());
        return position;
    }
    else if (change == ItemChildRemovedChange)
    {
        removeItem(value.value<QGraphicsItem*>());
    }

    return QGraphicsRectItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::prepareColumnMove()
//-----------------------------------------------------------------------------
void GraphicsColumn::prepareColumnMove()
{
    // Nothing to do.
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
// Function: GraphicsColumn::setItemLayout()
//-----------------------------------------------------------------------------
void GraphicsColumn::setItemLayout(QSharedPointer< IVGraphicsLayout<QGraphicsItem> > itemLayout)
{
    itemLayout_ = itemLayout;
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::switchColumn()
//-----------------------------------------------------------------------------
void GraphicsColumn::switchItemToColumn(QGraphicsItem* item, GraphicsColumn* column)
{
    // We have to switch the column and update this column's item positions without the moving item.
    QPointF newPos = column->mapFromScene(item->scenePos());
    item->setParentItem(column);
    item->setPos(newPos);

    setZValue(0.0);
    column->onMoveItem(item);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumn::updateNameLabel()
//-----------------------------------------------------------------------------
void GraphicsColumn::updateNameLabel()
{
    nameLabel_->setHtml("<center>" + columnData_->name() + "</center>");
    nameLabel_->setTextWidth(qMax<unsigned int>(140, columnData_->getWidth()));
    nameLabel_->setPos((columnData_->getWidth() - nameLabel_->textWidth()) / 2.0, 5.0);
}
