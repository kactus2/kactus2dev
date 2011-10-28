//-----------------------------------------------------------------------------
// File: DiagramColumnLayout.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.4.2011
//
// Description:
// Diagram column layout which manages the ordering and positioning
// of diagram columns.
//-----------------------------------------------------------------------------

#include "DiagramColumnLayout.h"

#include "DiagramColumn.h"

#include <common/layouts/HStackedLayout.h>
#include <common/GenericEditProvider.h>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: DiagramColumnLayout()
//-----------------------------------------------------------------------------
DiagramColumnLayout::DiagramColumnLayout(GenericEditProvider& editProvider,
                                         QGraphicsScene* scene) : editProvider_(editProvider),
                                                                  scene_(scene), columns_(),
                                                                  layoutWidth_(0.0), offsetY_(0)
{
}

//-----------------------------------------------------------------------------
// Function: ~DiagramColumnLayout()
//-----------------------------------------------------------------------------
DiagramColumnLayout::~DiagramColumnLayout()
{
    columns_.clear();
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void DiagramColumnLayout::addColumn(QString const& name, ColumnContentType contentType,
                                    unsigned int allowedItems)
{
    // Create the diagram column and position it accordingly.
    DiagramColumn* column = new DiagramColumn(name, contentType, allowedItems, this);
    column->setPos(layoutWidth_, 0.0);

    addColumn(column);
}

//-----------------------------------------------------------------------------
//  Function: addColumn()
//-----------------------------------------------------------------------------
void DiagramColumnLayout::addColumn(DiagramColumn* column)
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
// Function: addItem()
//-----------------------------------------------------------------------------
void DiagramColumnLayout::addItem(QGraphicsItem* item)
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
// Function: removeColumn()
//-----------------------------------------------------------------------------
void DiagramColumnLayout::removeColumn(DiagramColumn* column)
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
void DiagramColumnLayout::removeColumn(int index)
{
    columns_.removeAt(index);
    HStackedLayout::updateItemPositions(columns_, 0.0, 0.0, 0.0);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: findColumnAt()
//-----------------------------------------------------------------------------
DiagramColumn* DiagramColumnLayout::findColumnAt(QPointF pt)
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
void DiagramColumnLayout::onMoveColumn(DiagramColumn* column)
{
    // Clamp the coordinates.
    QPointF pos = column->pos();
    pos.setY(0.0);
    pos.setX(std::max(std::min(pos.x(), layoutWidth_ - (column->boundingRect().width())), 0.0));
    column->setPos(pos);
    column->update();

    HStackedLayout::updateItemMove(columns_, column);
}

//-----------------------------------------------------------------------------
// Function: onReleaseColumn()
//-----------------------------------------------------------------------------
void DiagramColumnLayout::onReleaseColumn(DiagramColumn* column)
{
    HStackedLayout::setItemPos(columns_, column, 0.0);
}

//-----------------------------------------------------------------------------
// Function: setOffsetY()
//-----------------------------------------------------------------------------
void DiagramColumnLayout::setOffsetY(qreal y)
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
QList<DiagramColumn*> const& DiagramColumnLayout::getColumns() const
{
    return columns_;
}

//-----------------------------------------------------------------------------
// Function: updateColumnPositions()
//-----------------------------------------------------------------------------
void DiagramColumnLayout::updateColumnPositions()
{    
    HStackedLayout::updateItemPositions(columns_, 0.0, 0.0, 0.0);
}

//-----------------------------------------------------------------------------
// Function: getEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider& DiagramColumnLayout::getEditProvider()
{
    return editProvider_;
}
