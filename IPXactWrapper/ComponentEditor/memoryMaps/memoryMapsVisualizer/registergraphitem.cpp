/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.cpp
 *		Project: Kactus 2
 */

#include "registergraphitem.h"
#include "fieldgraphitem.h"
#include <models/field.h>
#include <common/utils.h>
#include "addressblockgraphitem.h"
#include <IPXactWrapper/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <IPXactWrapper/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <IPXactWrapper/ComponentEditor/visualization/fieldgapitem.h>

#include <common/graphicsItems/visualizeritem.h>
#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

RegisterGraphItem::RegisterGraphItem(QSharedPointer<Register> reg,
									 QGraphicsItem* parent):
MemoryVisualizationItem(parent),
register_(reg) {

	Q_ASSERT(register_);
	QBrush brush(KactusColors::REGISTER_COLOR);
	setDefaultBrush(brush);

    setOverlappingTop(reg->getMSB());
    setOverlappingBottom(reg->getOffset());
}

RegisterGraphItem::~RegisterGraphItem() {
}

void RegisterGraphItem::refresh() {
	setName(register_->getName());
	
	// get the base of the parent address block
	quint64 base = 0;
	MemoryVisualizationItem* addrBlock = dynamic_cast<MemoryVisualizationItem*>(parentItem());
	if (addrBlock) {
		base = addrBlock->getOffset();
	}

	// get the offset of the register
	quint64 offset = Utils::str2Uint(register_->getAddressOffset());
	// calculate the start address of the register
	quint64 startAddress = base + offset;
	// show the start address of the register
	setLeftTopCorner(startAddress);

	// show the end address of the register
	unsigned int addrUnit = getAddressUnitSize();
	// prevent division by zero
	if (addrUnit == 0) {
		addrUnit = 1;
	}	
	unsigned int addrUnits = register_->getSize() / addrUnit;
	// Round upwards. Needed for unorthodox regsize, e.g. 30b/8=3 becomes 4 (Bytes)
	if (addrUnits * addrUnit < register_->getSize()) {
		addrUnits++;
	}

	quint64 endAddress = base + offset + addrUnits -1;
    
    setOverlappingTop(startAddress);
    setOverlappingBottom(endAddress);
    
    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + register_->getName() + "<br>" +
        "<b>First address: </b>" + AddressSpaceVisualizationItem::addr2Str(getOffset(), getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + AddressSpaceVisualizationItem::addr2Str(getLastAddress(), getBitWidth()) + 
        "<br>" +
        "<b>Size [bits]: </b>" + QString::number(register_->getSize()));

	// set the positions for the children
	reorganizeChildren();

	MemoryVisualizationItem* parentGraphItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

quint64 RegisterGraphItem::getOffset() const {
	Q_ASSERT(register_);
	
	// the register offset from the address block
	QString offsetStr = register_->getAddressOffset();
	quint64 regOffset = Utils::str2Uint(offsetStr);

	// the address block's offset
	AddressBlockGraphItem* blockItem = static_cast<AddressBlockGraphItem*>(parentItem());
	Q_ASSERT(blockItem);
	quint64 blockOffset = blockItem->getOffset();

	// the total offset is the address block's offset added with register's offset
	return blockOffset + regOffset;
}

int RegisterGraphItem::getBitWidth() const {
	Q_ASSERT(register_);
	return register_->getWidth();
}

void RegisterGraphItem::removeChild( MemoryVisualizationItem* childItem ) {
    quint64 offset = childItem->getLastAddress();

    Q_ASSERT(childItems_.contains(offset));
    int removeCount = childItems_.remove(offset, childItem);
    Q_ASSERT(removeCount == 1);
}

void RegisterGraphItem::reorganizeChildren() {

	// remove the gaps and update msbs of fields
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
	QList<FieldGapItem*> gaps;

	// the offset where the previous block starts
	quint64 OffsetMin = register_->getMSB();

    // previous conflicting area.
    FieldGapItem* prevConflict = 0;

    // x-coordinate for childs.        
	qreal xCoordinate = rect().left();

    // the previous and top-most child items.
	MemoryVisualizationItem* previous = NULL;
    MemoryVisualizationItem* topItem = NULL;   
    
	for (QMap<quint64, MemoryVisualizationItem*>::iterator i = --childItems_.end();
		i != --childItems_.begin(); --i) { 

        MemoryVisualizationItem* item = i.value();
		Q_ASSERT(item);

		// pointer to the possible gap to add between two fields
		FieldGapItem* gap = 0;

		// if there is a gap between the last field and this field
		if (OffsetMin > item->getLastAddress() + 1 ||
             previous == 0 && OffsetMin == item->getLastAddress() + 1) {

			// create the gap item
			gap = new FieldGapItem(this);

            // set the first address of the gap
            gap->setStartAddress(item->getLastAddress(), false);

            // if the gap is not at the start 
            if (previous && previous->getOffset() <= register_->getMSB()) {
                // set the last address for the gap
                gap->setEndAddress(OffsetMin, false);
            }
            else {
                // the gap starts from the end of the parent
                gap->setEndAddress(OffsetMin, true);
            }			

            // set the gap to the end of the last item
            gap->setPos(xCoordinate, rect().bottom());
            gap->setWidth(getChildWidth(gap));
            gap->setVisible(isExpanded());

            gaps.append(gap);

            // update the x-coordinate to avoid setting a field under the gap
            xCoordinate += gap->rect().width();
        }

        // If field overlaps with others.
        if (topItem && item->getLastAddress() >= topItem->getOffset() )
        {
            item->setConflicted(true);
            previous->setConflicted(true);

            // Field is completely inside other fields.
            if (item->getOffset() >= topItem->getOffset())
            {
                item->setCompleteOverlap();

                // Field is completely overlapping with previous and previous does not overlap with
                // completely with any other field.
                if (item->getOffset() >= previous->getOffset() &&
                    item->getLastAddress() <= previous->getLastAddress() &&
                    previous->getOverlappingBottom() != -1)
                {
                    topItem = previous;                    
                }
            }                


            // Field partially overlaps with previous top-most item.
            else
            {
                item->setOverlappingTop(topItem->getOffset() - 1);
                item->setWidth(getChildWidth(item));

                quint64 conflictEnd = item->getLastAddress();
                MemoryVisualizationItem* previousTopItem = topItem;

                // Previous top-most item is completely overlapped by this field and another field.
                if (prevConflict && prevConflict->getOffset() - 1 <= item->getLastAddress())
                {
                    
                    if (previousTopItem != previous)
                    {
                        // Update the coordinates of previous to align conflicting area correctly.     
                        previous->setPos(xCoordinate - getChildWidth(previousTopItem), rect().bottom());
                    }

                    previousTopItem->setCompleteOverlap();                   
                    conflictEnd = prevConflict->getOffset() - 1;
                    topItem = item;
                }
                // Previous top-most item is completely under this field. 
                else if (item->getLastAddress() == previousTopItem->getLastAddress())
                {                                   
                    if (previousTopItem != previous)
                    {
                        // Update the coordinates of previous to align conflicting area correctly.     
                        previous->setPos(xCoordinate - getChildWidth(previousTopItem), rect().bottom());
                    }

                    previousTopItem->setCompleteOverlap(); 
                    topItem = item;
                }

                // Previous top-most item is partially under this field. 
                else 
                {                 
                    xCoordinate -= getChildWidth(previousTopItem);
                    previousTopItem->setOverlappingBottom(item->getLastAddress() + 1); 
                    previousTopItem->setWidth(getChildWidth(previousTopItem));                                                          
                    xCoordinate += getChildWidth(previousTopItem);                                

                    if (previous != previousTopItem)
                    {
                        // Update the coordinates of previous to align conflicting area correctly.   
                        previous->setPos(xCoordinate, rect().bottom());
                    }

                    topItem = item;
                }

                gap = new FieldGapItem("conflicted", this);                
                gap->setConflicted(true);
                gap->setStartAddress(previousTopItem->getOffset(),true);
                gap->setEndAddress(conflictEnd,true); 
                gap->setVisible(isExpanded());
                gap->setWidth(getChildWidth(gap));
                gaps.append(gap);

                prevConflict = gap;
            }
        }
        else
        {
            topItem = item;
        }

        if (previous) {
            QRectF previousRect;

            // if there was a gap
            if (gap) {

                // find the position where previous field ended
                previousRect = mapRectFromItem(previous, previous->itemTotalRect());

                // set the gap to be under the previous field
                gap->setPos(previousRect.right(), rect().bottom());

                // update the previous rect to the end of the gap
                previousRect = mapRectFromItem(gap, gap->itemTotalRect());	
            }
            // if there was no gap then use the previous defined field
            else {
                // set the next item to start after the previous
                previousRect = mapRectFromItem(previous, previous->itemTotalRect());
            }

            xCoordinate = previousRect.right();
        }

		// update the last address of the field
		OffsetMin = qMin(item->getOffset(),OffsetMin);
		item->setPos(xCoordinate, rect().bottom());
		previous = item;
	}

	// if the LSB bit(s) does not belong to any field
	if (previous && OffsetMin > 0) {
		// create the gap 
		FieldGapItem* gap = new FieldGapItem(this);

		// the gap starts from the start of the register
		gap->setStartAddress(0, true);

		// the gap ends to the start of the defined field
        gap->setEndAddress(topItem->getOffset(), false);

        // increase the x-coordinate to avoid setting the next field on top of the gap
        if (topItem == previous)
        {
            xCoordinate += topItem->rect().width();
        }

		// gap is positioned after the last field
		gap->setPos(xCoordinate, rect().bottom());
        gap->setWidth(getChildWidth(gap));
		gap->setVisible(isExpanded());

		gaps.append(gap);
	}

	// add the gaps to the child items
	foreach (MemoryGapItem* gap, gaps) {
		childItems_.insert(gap->getOffset(), gap);
	}

	// reorganize the text blocks of this register
	ExpandableItem::reorganizeChildren();
}

void RegisterGraphItem::updateChildMap() {
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
 
        // update the status and msbs for the item
        if (item->getLastAddress() > register_->getMSB() )
        {
            item->setConflicted(true);
        }
        else
        {
            item->setConflicted(false);
        }
        item->setOverlappingTop(item->getLastAddress());
        item->setOverlappingBottom(item->getOffset());
        item->setWidth(getChildWidth(item));
        quint64 msb = item->getLastAddress();
        newMap.insertMulti(msb, item);
    }

    // Sort childs with same last bit for stable order.
    foreach(quint64 msb, newMap.uniqueKeys())
    {
        if(newMap.count(msb) != 1)
        {
            QList<MemoryVisualizationItem*> childs = newMap.values(msb);
            qSort(childs.begin(), childs.end(), addressLessThan );
            newMap.remove(msb);
            foreach(MemoryVisualizationItem* child, childs)
            {
                newMap.insertMulti(msb,child);
            }
        }
    }

    // update the original map
    childItems_ = newMap;
}



unsigned int RegisterGraphItem::getAddressUnitSize() const 
{
	AddressBlockGraphItem* addrBlock = static_cast<AddressBlockGraphItem*>(parentItem());
	Q_ASSERT(addrBlock);
	return addrBlock->getAddressUnitSize();
}

quint64 RegisterGraphItem::getLastAddress() const {
	AddressBlockGraphItem* addrBlock = static_cast<AddressBlockGraphItem*>(parentItem());
	Q_ASSERT(addrBlock);

	// how many bits one address unit takes
	unsigned int addrUnit = addrBlock->getAddressUnitSize();

	// prevent division by zero
	if (addrUnit == 0) {
		addrUnit = 1;
	}

	// the offset of the start of register
	quint64 offset = getOffset();

	// how many address unit are contained in the register
	unsigned int size = register_->getWidth() / addrUnit;
	if (size*addrUnit < register_->getWidth()) {
		size++; //Round truncated number upwards
	}
	// if size is not defined then never return a negative number
	if (size == 0) {
		return 0;
	}

	// the last address contained in the register
	return offset + size - 1;
}

void RegisterGraphItem::setWidth(qreal width)
{
    setRect(0, 0, width, VisualizerItem::DEFAULT_HEIGHT);

    childWidth_ = width;

    reorganizeChildren();
}

 qreal RegisterGraphItem::getChildWidth(MemoryVisualizationItem* child) const
{
    Q_ASSERT(child);
    if ( child->getOverlappingBottom() == -1 || child->getOverlappingTop() == -1)
    {
        return 0;
    }

    unsigned int highBit = child->getOverlappingTop();

    if (highBit > register_->getMSB())
    {       
        highBit = register_->getMSB();
    }

    unsigned int lowBit = child->getOverlappingBottom();

    if (lowBit > register_->getMSB())
    {
        return 0;
    }

    quint64 bits =  qMin(highBit - lowBit + 1,register_->getSize());    
    qreal bitWidth = childWidth_/register_->getSize();
    return bitWidth * bits;
 }

bool RegisterGraphItem::addressLessThan(const MemoryVisualizationItem* s1, 
    const MemoryVisualizationItem* s2)
{
    // Field with higher offset (lsb) precedes lower offset.
    return s1->getOffset() > s2->getOffset();
}