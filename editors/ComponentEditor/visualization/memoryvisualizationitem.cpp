//-----------------------------------------------------------------------------
// File: memoryvisualizationitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.10.2012
//
// Description:
// A base class for graphics items to visualize memory objects.
//-----------------------------------------------------------------------------

#include "memoryvisualizationitem.h"
#include "memorygapitem.h"

#include <common/KactusColors.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QRectF>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::MemoryVisualizationItem()
//-----------------------------------------------------------------------------
MemoryVisualizationItem::MemoryVisualizationItem(QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent):
ExpandableItem(parent),
    expressionParser_(expressionParser)
{
    QPen pen(Qt::gray);
    setPen(pen);
    setExpansionPen(pen);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::itemTotalRect()
//-----------------------------------------------------------------------------
QRectF MemoryVisualizationItem::itemTotalRect() const
{    
    QRectF totalRect(rect());

    // The rectangle must contain this item and also the child item.
    for (auto const& child : childItems_)
    {   
        if (child->isVisible())
        {
            totalRect = totalRect.united(mapRectFromItem(child, child->itemTotalRect()));
        }        
    }

    return totalRect;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::addChild()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::addChild(MemoryVisualizationItem* childItem)
{
    childItems_.insert(childItem->getOffset(), childItem);

    childItem->setWidth(childWidth_);
    childItem->setVisible(isExpanded());

    connect(childItem, SIGNAL(expandStateChanged()), this, SLOT(repositionChildren()), Qt::UniqueConnection);
    connect(childItem, SIGNAL(expandStateChanged()), this, SIGNAL(expandStateChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::removeChild()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::removeChild(MemoryVisualizationItem* childItem)
{
    quint64 offset = childItems_.key(childItem);

    Q_ASSERT(childItems_.contains(offset));
    childItems_.remove(offset, childItem);

    disconnect(childItem, SIGNAL(expandStateChanged()), this, SLOT(repositionChildren()));
    disconnect(childItem, SIGNAL(expandStateChanged()), this, SIGNAL(expandStateChanged()));

    showExpandIconIfHasChildren();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::getChildCount()
//-----------------------------------------------------------------------------
int MemoryVisualizationItem::getChildCount() const
{
    return childItems_.count();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setWidth()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setWidth(qreal width)
{
    qreal newChildWidth = width - MemoryVisualizationItem::CHILD_INDENTATION;

    if (childWidth_ != newChildWidth)
    {
        childWidth_ = newChildWidth;

        VisualizerItem::setWidth(width);
        
        for (auto& child : childItems_)
        {
            child->setWidth(childWidth_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::boundingRect()
//-----------------------------------------------------------------------------
QRectF MemoryVisualizationItem::boundingRect() const
{
    return itemTotalRect();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::redoChildLayout()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::redoChildLayout()
{    
    updateChildMap();

    repositionChildren();

    showExpandIconIfHasChildren();
}

//-----------------------------------------------------------------------------
// Function: memoryvisualizationitem::parseExpression()
//-----------------------------------------------------------------------------
quint64 MemoryVisualizationItem::parseExpression(QString const& expression) const
{
    return expressionParser_->parseExpression(expression).toULongLong();
}

//-----------------------------------------------------------------------------
// Function: memoryvisualizationitem::getExpressionParser()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionParser> MemoryVisualizationItem::getExpressionParser() const
{
    return expressionParser_;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::showExpandIconIfHasChildren()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::showExpandIconIfHasChildren()
{    
    ExpandableItem::setShowExpandableItem(childItems_.isEmpty() == false);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::updateChildMap()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::updateChildMap()
{
    removeGapsAndSortChildren();
   
    fillGapsBetweenChildren();

    markConflictingChildren();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::removeGapsAndSortChildren()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::removeGapsAndSortChildren()
{
    QMultiMap<quint64, MemoryVisualizationItem*> updatedMap;

    for (auto item = childItems_.cbegin(); item != childItems_.cend(); ++item)
    {
        if (auto gap = dynamic_cast<MemoryGapItem*>(*item))
        {
            delete gap;
        }
        else
        {
            updatedMap.insert((*item)->getOffset(), *item);
        }
    }

    // Sort childs with same offset for stable order.
    for (auto const& offset : updatedMap.uniqueKeys())
    {
        if (updatedMap.count(offset) != 1)
        {
            auto childs = updatedMap.values(offset);
            updatedMap.remove(offset);

            std::stable_sort(childs.begin(), childs.end(), compareItems);
            for (auto& child : childs)
            {
                updatedMap.insert(offset, child);
            }
        }
    }

    childItems_ = updatedMap;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::fillGapsBetweenChildren()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::fillGapsBetweenChildren()
{
    quint64 lastAddressInUse = getOffset();

    for (auto i = childItems_.begin(); i != childItems_.end(); ++i)
    {
        if (auto current = i.value(); current->isPresent())
        {
            if (quint64 currentOffset = childItems_.key(current); 
                lastAddressInUse == 0 && currentOffset > 0 && i == childItems_.begin())
            {
                i = createMemoryGap(0, currentOffset - 1);
                lastAddressInUse = currentOffset - 1;
            }
            else if (emptySpaceBeforeChild(current, lastAddressInUse))
            {
                i = createMemoryGap(lastAddressInUse + 1, currentOffset - 1);
            }

            lastAddressInUse = qMax(current->getLastAddress(), lastAddressInUse);
        }
    }

    // Fill in any addresses left between children and the end of this item.
    if (childItems_.isEmpty() == false && getLastAddress() > lastAddressInUse)
    {
        createMemoryGap(lastAddressInUse + 1, getLastAddress());
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::markConflictingChildren()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::markConflictingChildren()
{    
    const auto offset = getOffset();
    const auto lastAddress = getLastAddress();

    quint64 highestAddressInUse = offset;

    for (auto child = childItems_.begin(); child != childItems_.end(); ++child)
    {
        if (auto& current = child.value(); current->isPresent())
        {
            auto const& currentOffset = child.key();

            bool overlaps = currentOffset > offset && currentOffset <= highestAddressInUse;
            bool isOutsideBounds = currentOffset < offset || current->getLastAddress() > lastAddress;

            current->setConflicted(overlaps || isOutsideBounds);

            if (overlaps)
            {
                // Mark any overlapping items conflicted.
                for (auto preceding = childItems_.begin(); preceding != child; ++preceding)
                {
                    if ((*preceding)->getLastAddress() >= currentOffset)
                    {
                        (*preceding)->setConflicted(true);
                    }
                }
            }

            highestAddressInUse = qMax(current->getLastAddress(), highestAddressInUse);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::repositionChildren()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::repositionChildren()
{
    qreal yCoordinate = rect().bottom();
    
    bool parentExpanded = isExpanded();
    for (auto& current : childItems_)
    {
        bool present = current->isPresent();
        current->setVisible(present && parentExpanded);

        if (present)
        {
            current->setPos(MemoryVisualizationItem::CHILD_INDENTATION, yCoordinate);
            yCoordinate = mapRectFromItem(current, current->itemTotalRect()).bottom();
        }
    }

    resizeToContent();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::mousePressEvent()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
    emit selectEditor();
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::emptySpaceBeforeChild()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::emptySpaceBeforeChild(MemoryVisualizationItem const* current,
    quint64 lastAddressInUse) const
{    
    return current->getOffset() > lastAddressInUse + 1;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::createMemoryGap()
//-----------------------------------------------------------------------------
QMultiMap<quint64, MemoryVisualizationItem*>::iterator MemoryVisualizationItem::createMemoryGap(quint64 startAddress,
    quint64 endAddress)
{
    auto gap = new MemoryGapItem(expressionParser_, this);
    gap->setWidth(childWidth_);
    gap->setDisplayOffset(startAddress);
    gap->setDisplayLastAddress(endAddress);    
    gap->repositionLabels();

    return childItems_.insert(gap->getOffset(), gap);
}
