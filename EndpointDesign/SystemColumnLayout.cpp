//-----------------------------------------------------------------------------
// File: SystemColumnLayout.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.5.2011
//
// Description:
// Node column layout which manages the ordering and positioning
// of node columns.
//-----------------------------------------------------------------------------

#include "SystemColumnLayout.h"

#include "SystemColumn.h"

#include <common/layouts/HStackedLayout.h>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: SystemColumnLayout()
//-----------------------------------------------------------------------------
SystemColumnLayout::SystemColumnLayout(QGraphicsScene* scene) : scene_(scene), columns_(),
                                                            layoutWidth_(0.0), offsetY_(0)
{
}

//-----------------------------------------------------------------------------
// Function: ~SystemColumnLayout()
//-----------------------------------------------------------------------------
SystemColumnLayout::~SystemColumnLayout()
{
    // Delete all columns and clear the column list.
    for (int i = 0; i < columns_.size(); ++i)
    {
        delete columns_.at(i);
    }

    columns_.clear();
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void SystemColumnLayout::addColumn(QString const& name)
{
    // Create the node column and position it accordingly.
    SystemColumn* column = new SystemColumn(name, this, scene_);
    column->setPos(layoutWidth_, 0.0);

    addColumn(column);
}

//-----------------------------------------------------------------------------
//  Function: addColumn()
//-----------------------------------------------------------------------------
void SystemColumnLayout::addColumn(SystemColumn* column)
{
    if (column->scene() == 0)
    {
        scene_->addItem(column);
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
void SystemColumnLayout::removeColumn(SystemColumn* column)
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
// Function: removeColumn()
//-----------------------------------------------------------------------------
void SystemColumnLayout::removeColumn(int index)
{
    columns_.removeAt(index);
    HStackedLayout::updateItemPositions(columns_, 0.0, 0.0, 0.0);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: findColumnAt()
//-----------------------------------------------------------------------------
SystemColumn* SystemColumnLayout::findColumnAt(QPointF pt)
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
void SystemColumnLayout::onMoveColumn(SystemColumn* column)
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
void SystemColumnLayout::onReleaseColumn(SystemColumn* column)
{
    HStackedLayout::setItemPos(columns_, column, 0.0);
}

//-----------------------------------------------------------------------------
// Function: setOffsetY()
//-----------------------------------------------------------------------------
void SystemColumnLayout::setOffsetY(qreal y)
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
QList<SystemColumn*> const& SystemColumnLayout::getColumns() const
{
    return columns_;
}
