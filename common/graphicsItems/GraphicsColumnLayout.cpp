//-----------------------------------------------------------------------------
// File: GraphicsColumnLayout.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.5.2011
//
// Description:
// Graphics column layout which manages the ordering and positioning
// of graphics columns.
//-----------------------------------------------------------------------------

#include "GraphicsColumnLayout.h"

#include "GraphicsColumn.h"

#include <common/layouts/HStackedLayout.h>

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout::GraphicsColumnLayout(QGraphicsScene* scene)
    : scene_(scene),
      columns_(),
      layoutWidth_(0.0),
      offsetY_(0)
{
}

//-----------------------------------------------------------------------------
// Function: ~GraphicsColumnLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout::~GraphicsColumnLayout()
{
    // Delete all columns and clear the column list.
    for (int i = 0; i < columns_.size(); ++i)
    {
        delete columns_.at(i);
    }

    columns_.clear();
}

//-----------------------------------------------------------------------------
// Function: addItem()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::addItem(QGraphicsItem* item)
{
    for (int i = 0; i < columns_.size(); ++i)
    {
        if (columns_.at(i)->isItemAllowed(item))
        {
            columns_.at(i)->addItem(item);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
//  Function: addColumn()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::addColumn(GraphicsColumn* column, bool append)
{
    if (column->scene() == 0)
    {
        scene_->addItem(column);
    }

    if (append)
    {
        column->setPos(layoutWidth_, 0.0);
    }

    column->setOffsetY(offsetY_);

    connect(column, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    // Add the column to the list of columns and update the layout width.
    columns_.append(column);
    layoutWidth_ += column->boundingRect().width();

    // Update the position in the layout.
    HStackedLayout::updateItemMove(columns_, column);
    HStackedLayout::setItemPos(columns_, column, 0.0);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: removeColumn()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::removeColumn(GraphicsColumn* column)
{
    if (columns_.removeAll(column) > 0)
    {
        layoutWidth_ -= column->boundingRect().width();
        HStackedLayout::updateItemPositions(columns_, 0.0, 0.0, 0.0);
        scene_->removeItem(column);

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: findColumnAt()
//-----------------------------------------------------------------------------
GraphicsColumn* GraphicsColumnLayout::findColumnAt(QPointF pt)
{
    for (int i = 0; i < columns_.size(); ++i)
    {
        // The point is inside the column if the x coordinate is within it (y can be anything).
        if (pt.x() >= columns_.at(i)->sceneBoundingRect().left() &&
            pt.x() < columns_.at(i)->sceneBoundingRect().right())
        {
            return columns_.at(i);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: onMoveColumn()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::onMoveColumn(GraphicsColumn* column)
{
    // Clamp the coordinates.
    QPointF pos = column->pos();
    pos.setY(0.0);
    pos.setX(std::max(std::min(pos.x(), layoutWidth_ - (column->boundingRect().width())), 0.0));
    column->setPos(pos);
    column->update();

    HStackedLayout::updateItemMove(columns_, column);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: onReleaseColumn()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::onReleaseColumn(GraphicsColumn* column)
{
    HStackedLayout::setItemPos(columns_, column, 0.0);
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnLayout::updateColumnPositions()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::updateColumnPositions()
{
    HStackedLayout::updateItemPositions(columns_, 0.0, 0.0, 0.0);

    layoutWidth_ = 0.0;

    foreach (GraphicsColumn* column, columns_)
    {
        layoutWidth_ += column->boundingRect().width();
    }
}

//-----------------------------------------------------------------------------
// Function: setOffsetY()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::setOffsetY(qreal y)
{
    for (int i = 0; i < columns_.size(); ++i)
    {
        columns_.at(i)->setOffsetY(y);
    }

    offsetY_ = y;
}

//-----------------------------------------------------------------------------
// Function: getColumns()
//-----------------------------------------------------------------------------
QList<GraphicsColumn*> const& GraphicsColumnLayout::getColumns() const
{
    return columns_;
}

//-----------------------------------------------------------------------------
// Function: updatePositions()
//-----------------------------------------------------------------------------
void GraphicsColumnLayout::updatePositions()
{
    for (int i = 0; i < columns_.size(); ++i)
    {
        columns_[i]->updateItemPositions();
    }
}
