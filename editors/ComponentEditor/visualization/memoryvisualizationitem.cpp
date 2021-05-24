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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QRectF>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::MemoryVisualizationItem()
//-----------------------------------------------------------------------------
MemoryVisualizationItem::MemoryVisualizationItem(QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent):
ExpandableItem(parent),
    childItems_(),
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
    for (auto child : childItems_)
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
    childItems_.insertMulti(childItem->getOffset(), childItem);

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
        
        for (auto child : childItems_)
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
// Function: MemoryVisualizationItem::setDisplayOffset()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setDisplayOffset(quint64 const& address)
{
    firstAddress_ = address;
    setTopLabelText(firstAddress_);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::getDisplayOffset()
//-----------------------------------------------------------------------------
quint64 MemoryVisualizationItem::getDisplayOffset()
{
    return firstAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setDisplayLastAddress()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setDisplayLastAddress(quint64 const& address)
{
    lastAddress_ = address;

    if (firstAddress_ != lastAddress_)
    {
        setBottomLabelText(lastAddress_);
    }
    else
    {
        setBottomLabelText(QString());
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::getDisplayLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryVisualizationItem::getDisplayLastAddress()
{
    return lastAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setConflicted()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setConflicted(bool conflicted)
{
    conflicted_ = conflicted;

    QBrush colorBrush = defaultBrush();
    if (conflicted)
    {
        colorBrush = KactusColors::MISSING_COMPONENT;
    }

    QAbstractGraphicsShapeItem::setBrush(colorBrush);
    setExpansionBrush(colorBrush);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::isConflicted()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::isConflicted() const
{
    return conflicted_;
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
    QMap<quint64, MemoryVisualizationItem*> updatedMap;

    for (auto item = childItems_.cbegin(); item != childItems_.cend(); ++item)
    {
        MemoryGapItem* gap = dynamic_cast<MemoryGapItem*>(*item);
        if (gap)
        {
            delete gap;
        }
        else
        {
            updatedMap.insertMulti((*item)->getOffset(), *item);
        }
    }

    // Sort childs with same offset for stable order.
    for (quint64 const& offset : updatedMap.uniqueKeys())
    {
        if (updatedMap.count(offset) != 1)
        {
            QList<MemoryVisualizationItem*> childs = updatedMap.values(offset);
            updatedMap.remove(offset);

            qStableSort(childs.begin(), childs.end(), compareItems);
            for (auto& child : childs)
            {
                updatedMap.insertMulti(offset, child);
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
        auto current = i.value();
        if (current->isPresent())
        {
            quint64 currentOffset = childItems_.key(current);

            if (lastAddressInUse == 0 && currentOffset > 0 && i == childItems_.begin())
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
    if (getLastAddress() > lastAddressInUse)
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
        MemoryVisualizationItem* current = child.value();

        if (current->isPresent())
        {
            quint64 currentOffset = childItems_.key(current);

            bool overlaps = currentOffset > offset && currentOffset <= highestAddressInUse;
            bool isOutsideBounds = currentOffset < offset || current->getLastAddress() > lastAddress;

            current->setConflicted(overlaps || isOutsideBounds);

            if (overlaps)
            {
                // Walk backwards and mark any overlapping items conflicted.
                for (auto previous = child - 1; previous != childItems_.begin() - 1; --previous)
                {
                    if ((*previous)->getLastAddress() >= currentOffset)
                    {
                        (*previous)->setConflicted(true);
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
    for (MemoryVisualizationItem* current : childItems_)
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
// Function: MemoryVisualizationItem::setLeftTopCorner()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setTopLabelText(QString const& text) 
{
    VisualizerItem::setTopLabelText(groupByFourDigits(text));
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setLeftTopCorner()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setTopLabelText(quint64 address)
{
    VisualizerItem::setTopLabelText(toHexString(address));
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::toHexString()
//-----------------------------------------------------------------------------
QString MemoryVisualizationItem::toHexString(quint64 address)
{
    QString str = QString::number(address, 16);
    str = str.toUpper();

    int bitWidth = getBitWidth();

    // one hexadecimal number accounts for four bits
    int fieldSize = bitWidth / 4;
    if (fieldSize*4 < bitWidth)
    {
        fieldSize++; //Round upwards, e.g. 7 bits needs 4 hex digits
    }

    QString padded = QString("%1").arg(str, fieldSize, QLatin1Char('0'));
    return groupByFourDigits(padded);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setLeftBottomCorner()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setBottomLabelText(QString const& text)
{
    VisualizerItem::setBottomLabelText(groupByFourDigits(text));
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::setLeftBottomCorner()
//-----------------------------------------------------------------------------
void MemoryVisualizationItem::setBottomLabelText(quint64 address)
{
    VisualizerItem::setBottomLabelText(toHexString(address));
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
QMap<quint64, MemoryVisualizationItem*>::iterator MemoryVisualizationItem::createMemoryGap(quint64 startAddress,
    quint64 endAddress)
{
    MemoryGapItem* gap = new MemoryGapItem(expressionParser_, this);
    gap->setWidth(childWidth_);
    gap->setDisplayOffset(startAddress);
    gap->setDisplayLastAddress(endAddress);    

    return childItems_.insert(gap->getOffset(), gap);
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::groupByFourDigits()
//-----------------------------------------------------------------------------
QString MemoryVisualizationItem::groupByFourDigits(QString const& text) const
{
    QString groupedText(text);

    int size = text.size();
    for (int i = size; i > 0; i -= 4) 
    {
        groupedText.insert(i, QLatin1Char(' '));
    }

    return groupedText;
}

//-----------------------------------------------------------------------------
// Function: MemoryVisualizationItem::compareItems()
//-----------------------------------------------------------------------------
bool MemoryVisualizationItem::compareItems(const MemoryVisualizationItem* s1, const MemoryVisualizationItem* s2)
{
    // Item with bigger last address precedes the other.
    return s1->getLastAddress() > s2->getLastAddress();
}
