/* 
*  	Created on: 15.10.2012
*      Author: Antti Kamppi
* 		filename: memoryvisualizationitem.cpp
*		Project: Kactus 2
*/

#include "memoryvisualizationitem.h"
#include "memorygapitem.h"

#include <common/KactusColors.h>

#include <QRectF>
#include <QPen>

#include <QDebug>

MemoryVisualizationItem::MemoryVisualizationItem( QGraphicsItem* parent /*= 0*/ ):
ExpandableItem(parent),
    originalBrush_(QBrush()),
firstFreeAddress_(-1),
lastFreeAddress_(-1)
{
    QPen pen(Qt::gray);
    setPen(pen);
    setExpansionPen(pen);
}

MemoryVisualizationItem::~MemoryVisualizationItem() {
}

void MemoryVisualizationItem::setBrush(const QBrush& brush)
{
    originalBrush_ = brush;
    QAbstractGraphicsShapeItem::setBrush(brush);
}

void MemoryVisualizationItem::reorganizeChildren() {
    // first find out the width for all items
    qreal width = itemTotalWidth();

    // update the offsets of the child items so they are displayed in correct order
    updateChildMap();

    // if there are no children then this can not be expanded or collapsed
    if (childItems_.isEmpty()) {
        ExpandableItem::setShowExpandableItem(false);
    }
    // if there are children then display the expand/collapse item
    else {
        ExpandableItem::setShowExpandableItem(true);
    }

    // save the new gap child items to be added to map
    QList<MemoryGapItem*> gaps;
    // the offset where the last block ended
    quint64 previousBlockEnd = getOffset();

    // Last conflicted gap.
    MemoryGapItem* prevConflict = 0;

    qreal yCoordinate = rect().bottom();
    MemoryVisualizationItem* previous = NULL;
    foreach (MemoryVisualizationItem* item, childItems_) {

        Q_ASSERT(item);
        item->setWidth(width);
        item->setNotConflicted();

        // pointer to the possible gap item
        MemoryGapItem* gap = 0;

        // if there is a gap between the last block and this block
        if (item->getOffset() > previousBlockEnd + 1) {

            // create the gap item
            gap = new MemoryGapItem(this);

            // if the gap is not at the start 
            if (previous) {
                // set the first address of the gap
                gap->setStartAddress(previousBlockEnd, false);
            }
            else {
                // the gap starts from the start of the parent
                gap->setStartAddress(previousBlockEnd, true);
            }			

            // set the last address for the gap
            gap->setEndAddress(item->getOffset(), false);

            // set the gap to the end of the last item
            gap->setPos(MemoryVisualizationItem::CHILD_INDENTATION, yCoordinate);

            // update the y-coordinate to avoid setting a block under the gap
            yCoordinate += VisualizerItem::ITEM_HEIGHT;

            gap->setVisible(isExpanded());
            gap->setWidth(width);

            gaps.append(gap);
        }

        // if the block and the next block are overlapping.
        else if (previous && item->getOffset() <= previousBlockEnd )
        {

            // This and previous block overlap completely.
            if (item->getOffset() == previous->getOffset() &&
                item->getLastAddress() == previous->getLastAddress())
            {
                previous->setConflicted();
                item->setConflicted();
            }
            // This block is completely inside the previous block.
            else if (item->getOffset() >= previous->getOffset() &&
                item->getLastAddress() <= previous->getLastAddress() )
            {
                item->setConflicted();
                previous->setConflicted();
            }

            // The previous block is completely inside this block.
            else if (previous->getOffset() >= item->getOffset() &&
                previous->getLastAddress() <= item->getLastAddress())
            {
                item->setConflicted();
                previous->setConflicted();
            }          

            // Partial overlapping.
            else
            {                
                gap = new MemoryGapItem(this);
                gap->setConflicted();
                gap->setName("conflicted");
                gap->setStartAddress(item->getOffset(),true);
                gap->setOverlappingTop(item->getOffset());
                gap->setEndAddress(previousBlockEnd,true);                     
                gap->setOverlappingBottom(previousBlockEnd);
                                
                // set the conflicted gap to the end of the last item.
                gap->setPos(MemoryVisualizationItem::CHILD_INDENTATION, yCoordinate);

                // update the y-coordinate to avoid setting a block under the gap.
                yCoordinate += VisualizerItem::ITEM_HEIGHT;

                gap->setVisible(isExpanded());
                gap->setWidth(width);

                gaps.append(gap);

                // Update display of the first address on this block.
                item->setOverlappingTop(previousBlockEnd + 1);

                // If previous block is completely overlapped by the preceding block and this block.
                if (prevConflict && prevConflict->getLastAddress() + 1 >= item->getOffset())
                {
                    previous->setCompleteOverlap();
                }
                // Update display of last address on the previous block.                
                else
                {
                    previous->setOverlappingBottom(item->getOffset() - 1);
                }

                prevConflict = gap;
            }
        }

        // if there has been previous item drawn
        if (previous) {

            QRectF previousRect;

            // if there was a gap
            if (gap) {

                // find the position where previous block ended
                previousRect = mapRectFromItem(previous, previous->itemTotalRect());

                // set the gap to be under the previous block
                gap->setPos(MemoryVisualizationItem::CHILD_INDENTATION, previousRect.bottom());

                // update the previous rect to the end of the gap
                previousRect = mapRectFromItem(gap, gap->itemTotalRect());	
            }
            // if there was no gap then use the previous defined block
            else {
                // set the next item to start after the previous
                previousRect = mapRectFromItem(previous, previous->itemTotalRect());
            }

            // update the y coordinate to avoid setting items on top of each other
            yCoordinate = previousRect.bottom();
        }

        // update the last address of the block        
        if(item->getLastAddress() >= previousBlockEnd){
        previousBlockEnd = item->getLastAddress();
}
        item->setPos(MemoryVisualizationItem::CHILD_INDENTATION, yCoordinate);
        previous = item;
    }

    // check if there is gap after the last address but only if there is at least one child
    if (getLastAddress() > previousBlockEnd && !childItems_.isEmpty()) {
        // create the gap item
        MemoryGapItem* gap = new MemoryGapItem(this);

        // set the first address of the gap
        gap->setStartAddress(previousBlockEnd, false);

        // set the last address for the gap
        gap->setEndAddress(getLastAddress());

        // increase the y-coordinate to avoid setting the gap on top of the last block.
        yCoordinate = mapRectFromItem(previous, previous->itemTotalRect()).bottom();

        // set the gap to the end of the last item
        gap->setPos(MemoryVisualizationItem::CHILD_INDENTATION, yCoordinate);

        gap->setVisible(isExpanded());
        gap->setWidth(width);

        gaps.append(gap);
    }

    // add the gaps to the child items
    foreach (MemoryGapItem* gap, gaps) {
        childItems_.insert(gap->getOffset(), gap);
    }

    // update the width of this item to match the width of all
    setWidth(width);
}

void MemoryVisualizationItem::addChild( MemoryVisualizationItem* childItem ) {
    quint64 offset = childItem->getOffset();
    childItems_.insertMulti(offset, childItem);

    childItem->setVisible(isExpanded());
}

void MemoryVisualizationItem::removeChild( MemoryVisualizationItem* childItem ) {
    quint64 offset = childItem->getOffset();

    Q_ASSERT(childItems_.contains(offset));
    int removeCount = childItems_.remove(offset, childItem);
    Q_ASSERT(removeCount == 1);
}

void MemoryVisualizationItem::updateChildMap() {
    QMap<quint64, MemoryVisualizationItem*> newMap;

    // go through all children and ask their offsets
    foreach (MemoryVisualizationItem* item, childItems_) {

        // if the item is a gap then it is not added
        MemoryGapItem* gap = dynamic_cast<MemoryGapItem*>(item);
        if (gap) {
            gap->setParent(NULL);
            delete gap;
            gap = NULL;
            continue;
        }

        // update the offset for the item
        quint64 offset = item->getOffset();
        newMap.insertMulti(offset, item);
    }

    // Sort childs with same offset for stable order.
    foreach(quint64 offset, newMap.uniqueKeys())
    {
        if(newMap.count(offset) != 1)
        {
            QList<MemoryVisualizationItem*> childs = newMap.values(offset);
            qSort(childs);
            newMap.remove(offset);
            foreach(MemoryVisualizationItem* child, childs)
            {
                newMap.insertMulti(offset,child);
            }
        }
    }

    // update the original map
    childItems_ = newMap;
}

void MemoryVisualizationItem::setWidth( qreal width ) {
    setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);
    ExpandableItem::reorganizeChildren();
}

QRectF MemoryVisualizationItem::boundingRect() const {
    QRectF bounds = itemTotalRect();
    return bounds;
}

void MemoryVisualizationItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;
    setLeftTopCorner(firstFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_){        
        setLeftBottomCorner("");
    }
}

void MemoryVisualizationItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;
    setLeftBottomCorner(lastFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_)
    {
        setLeftBottomCorner("");
    }
}

void MemoryVisualizationItem::setCompleteOverlap()
{
    setConflicted();
    setLeftTopCorner("");
    setLeftBottomCorner("");
    VisualizerItem::setRightTopCorner("");
}

void MemoryVisualizationItem::setLeftTopCorner( const QString& text ) {
    QString str(text);

    // add white space after four digits
    int size = str.size();
    for (int i = size; i > 2; i -= 4) {
        str.insert(i, " ");
    }
    VisualizerItem::setLeftTopCorner(str);
}

void MemoryVisualizationItem::setLeftTopCorner( quint64 address ) {
    // convert the number into hexadecimal form
    QString str = QString::number(address, 16);
    str = str.toUpper();

    int bitWidth = getBitWidth();

    // one hexadecimal digit accounts for four bits
    int fieldSize = bitWidth / 4;
    if (fieldSize*4 < bitWidth) {
        fieldSize++; //Round upwards, e.g. 7bits needs 4 hex digits
    }
    QString padded = QString("%1").arg(str, fieldSize, QChar('0'));

    // group the string to groups of four characters
    int size = padded.size();
    for (int i = size; i > 0; i -= 4) {
        padded.insert(i, " ");
    }

    // add the identifier indicating a hexadecimal number
    padded.prepend("0x");

    VisualizerItem::setLeftTopCorner(padded);
}

void MemoryVisualizationItem::setLeftBottomCorner( const QString& text ) {
    QString str(text);

    // add white space after four digits
    int size = str.size();
    for (int i = size; i > 2; i -= 4) {
        str.insert(i, " ");
    }
    VisualizerItem::setLeftBottomCorner(str);
}

void MemoryVisualizationItem::setLeftBottomCorner( quint64 address ) {
    // convert the number into hexadecimal form
    QString str = QString::number(address, 16);
    str = str.toUpper();

    int bitWidth = getBitWidth();

    // one hexadecimal number accounts for four bits
    int fieldSize = bitWidth / 4;
    if (fieldSize*4 < bitWidth) {
        fieldSize++; //Round upwards, e.g. 7bits needs 4 hex digits
    }
    QString padded = QString("%1").arg(str, fieldSize, QChar('0'));

    // group the string to groups of four characters
    int size = padded.size();
    for (int i = size; i > 0; i -= 4) {
        padded.insert(i, " ");
    }

    // add the identifier indicating a hexadecimal number
    padded.prepend("0x");

    VisualizerItem::setLeftBottomCorner(padded);
}

void MemoryVisualizationItem::setConflicted()
{
    //setName(getName() + " (conflicted)");
    QAbstractGraphicsShapeItem::setBrush(KactusColors::MISSING_COMPONENT);
    setExpansionBrush(KactusColors::MISSING_COMPONENT);
}

void MemoryVisualizationItem::setNotConflicted()
{
    //setName(getName().remove(" (conflicted)"));
    QAbstractGraphicsShapeItem::setBrush(originalBrush_);
    setExpansionBrush(originalBrush_);
}

void MemoryVisualizationItem::mousePressEvent( QGraphicsSceneMouseEvent* event ) {
    // perform the base class functionality and inform that this item should be selected
    QGraphicsItem::mousePressEvent(event);
    emit selectEditor();
}
