/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.cpp
 *		Project: Kactus 2
 */

#include "registergraphitem.h"
#include "fieldgraphitem.h"

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/visualization/fieldgapitem.h>

#include <common/KactusColors.h>
#include <common/utils.h>
#include <common/graphicsItems/visualizeritem.h>

#include <IPXACTmodels/field.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::RegisterGraphItem()
//-----------------------------------------------------------------------------
RegisterGraphItem::RegisterGraphItem(QSharedPointer<Register> reg, QGraphicsItem* parent):
MemoryVisualizationItem(parent),
register_(reg)
{
	Q_ASSERT(register_);

    QBrush brush(KactusColors::REGISTER_COLOR);
	setDefaultBrush(brush);

    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::~RegisterGraphItem()
//-----------------------------------------------------------------------------
RegisterGraphItem::~RegisterGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: registergraphitem::refresh()
//-----------------------------------------------------------------------------
void RegisterGraphItem::refresh() 
{
    updateDisplay();
    reorganizeChildren();

    emit sizeChanged();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void RegisterGraphItem::updateDisplay()
{
    setName(register_->getName());

    quint64 startAddress = getOffset();

    // show the end address of the register
    unsigned int aub = getAddressUnitSize();

    // prevent division by zero
    if (aub == 0)
    {
        aub = 1;
    }	

    unsigned int registerSize = register_->getSize();
    unsigned int sizeInAUB = registerSize / aub;

    // Round upwards. Needed for unorthodox regsize, e.g. 30b/8=3 becomes 4 (Bytes)
    if (sizeInAUB * aub < registerSize)
    {
        sizeInAUB++;
    }

    quint64 endAddress = startAddress + sizeInAUB -1;

    setDisplayOffset(startAddress);
    setDisplayLastAddress(endAddress);

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + register_->getName() + "<br>" +
        "<b>First address: </b>" + toHexString(getOffset()) + "<br>" +
        "<b>Last address: </b>" + toHexString(getLastAddress()) + "<br>" +
        "<b>Size [bits]: </b>" + QString::number(registerSize));
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 RegisterGraphItem::getOffset() const
{
	// the register offset from the address block
	quint64 regOffset = Utils::str2Uint(register_->getAddressOffset());

	// the address block's offset
	MemoryVisualizationItem* blockItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(blockItem);
	quint64 blockOffset = blockItem->getOffset();

	// the total offset is the address block's offset added with register's offset
	return blockOffset + regOffset;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int RegisterGraphItem::getBitWidth() const
{
	return register_->getWidth();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::addChild()
//-----------------------------------------------------------------------------
void RegisterGraphItem::addChild(MemoryVisualizationItem* childItem)
{
    MemoryVisualizationItem::addChild(childItem);
    
    if (childItems_.count() == 1)
    {
        emit sizeChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::removeChild()
//-----------------------------------------------------------------------------
void RegisterGraphItem::removeChild( MemoryVisualizationItem* childItem )
{
    quint64 offset = childItem->getLastAddress();

    Q_ASSERT(childItems_.contains(offset));
    childItems_.remove(offset, childItem);

    if (childItems_.isEmpty())
    {
        emit sizeChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::reorganizeChildren()
//-----------------------------------------------------------------------------
void RegisterGraphItem::reorganizeChildren()
{
	// remove the gaps and update msbs of fields
	updateChildMap();
    showExpandIconIfHasChildren();

	// save the new gap child items to be added to map
	QList<FieldGapItem*> gaps;

	// the offset where the previous block starts
	quint64 OffsetMin = register_->getMSB();

    // previous conflicting area.
    FieldGapItem* prevConflict = 0;

    // x-coordinate for childs.        
	qreal xCoordinate = rect().left();

    // the previous and top-most child items.
	MemoryVisualizationItem* previous = 0;
    MemoryVisualizationItem* topItem = 0;   
    
	for (QMap<quint64, MemoryVisualizationItem*>::iterator i = --childItems_.end();
		i != --childItems_.begin(); --i) { 

        MemoryVisualizationItem* item = i.value();
		Q_ASSERT(item);

		// pointer to the possible gap to add between two fields
		FieldGapItem* gap = 0;

		// if there is a gap between the last field and this field
		if (OffsetMin > item->getLastAddress() + 1 ||
            (previous == 0 && OffsetMin == item->getLastAddress() + 1)) {

			// create the gap item
			gap = new FieldGapItem(tr("Reserved"), this);

            // set the first address of the gap
            gap->setStartAddress(item->getLastAddress() + 1);

            // if the gap is not at the start 
            if (previous && previous->getOffset() <= register_->getMSB()) {
                // set the last address for the gap
                gap->setEndAddress(OffsetMin - 1);
            }
            else {
                // the gap starts from the end of the parent
                gap->setEndAddress(OffsetMin);
            }			

            // set the gap to the end of the last item
            gap->setPos(xCoordinate, rect().bottom());
            gap->setWidth(widthForChild(gap));
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
                    !previous->isCompletelyOverlapped())
                {
                    topItem = previous;                    
                }
            }                


            // Field partially overlaps with previous top-most item.
            else
            {
                item->setDisplayOffset(topItem->getOffset() - 1);
                item->setWidth(widthForChild(item));

                quint64 conflictEnd = item->getLastAddress();
                MemoryVisualizationItem* previousTopItem = topItem;

                // Previous top-most item is completely overlapped by this field and another field.
                if (prevConflict && prevConflict->getOffset() - 1 <= item->getLastAddress())
                {
                    
                    if (previousTopItem != previous)
                    {
                        // Update the coordinates of previous to align conflicting area correctly.     
                        previous->setPos(xCoordinate - widthForChild(previousTopItem), rect().bottom());
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
                        previous->setPos(xCoordinate - widthForChild(previousTopItem), rect().bottom());
                    }

                    previousTopItem->setCompleteOverlap(); 
                    topItem = item;
                }

                // Previous top-most item is partially under this field. 
                else 
                {                 
                    xCoordinate -= widthForChild(previousTopItem);
                    previousTopItem->setDisplayLastAddress(item->getLastAddress() + 1); 
                    previousTopItem->setWidth(widthForChild(previousTopItem));                                                          
                    xCoordinate += widthForChild(previousTopItem);                                

                    if (previous != previousTopItem)
                    {
                        // Update the coordinates of previous to align conflicting area correctly.   
                        previous->setPos(xCoordinate, rect().bottom());
                    }

                    topItem = item;
                }

                gap = new FieldGapItem("conflicted", this);                
                gap->setConflicted(true);
                gap->setStartAddress(previousTopItem->getOffset());
                gap->setEndAddress(conflictEnd); 
                gap->setVisible(isExpanded());
                gap->setWidth(widthForChild(gap));
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
	if (previous && OffsetMin > 0)
    {
		// create the gap 
		FieldGapItem* gap = new FieldGapItem(tr("Reserved"), this);

		// the gap starts from the start of the register
		gap->setStartAddress(0);

		// the gap ends to the start of the defined field
        gap->setEndAddress(topItem->getOffset() - 1);

        // increase the x-coordinate to avoid setting the next field on top of the gap
        if (topItem == previous)
        {
            xCoordinate += topItem->rect().width();
        }

		// gap is positioned after the last field
		gap->setPos(xCoordinate, rect().bottom());
        gap->setWidth(widthForChild(gap));
		gap->setVisible(isExpanded());

		gaps.append(gap);
	}

	// add the gaps to the child items
	foreach (MemoryGapItem* gap, gaps)
    {
		childItems_.insert(gap->getOffset(), gap);
	}

	// reorganize the text blocks of this register
	ExpandableItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::updateChildMap()
//-----------------------------------------------------------------------------
void RegisterGraphItem::updateChildMap()
{
    QMap<quint64, MemoryVisualizationItem*> newMap;

    // go through all children and ask their offsets
    foreach (MemoryVisualizationItem* item, childItems_)
    {
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
        item->setDisplayOffset(item->getLastAddress());
        item->setDisplayLastAddress(item->getOffset());
        item->setWidth(widthForChild(item));
        quint64 msb = item->getLastAddress();
        newMap.insertMulti(msb, item);
    }

    // Sort childs with same last bit for stable order.
    foreach(quint64 msb, newMap.uniqueKeys())
    {
        if(newMap.count(msb) != 1)
        {
            QList<MemoryVisualizationItem*> childs = newMap.values(msb);
            qSort(childs.begin(), childs.end(), compareItems );
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

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int RegisterGraphItem::getAddressUnitSize() const 
{
	MemoryVisualizationItem* addrBlock = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(addrBlock);
	return addrBlock->getAddressUnitSize();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 RegisterGraphItem::getLastAddress() const
{
	MemoryVisualizationItem* addrBlock = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(addrBlock);

	// how many bits one address unit takes
	unsigned int addrUnit = addrBlock->getAddressUnitSize();

	// prevent division by zero
	if (addrUnit == 0)
    {
		addrUnit = 1;
	}

	// the offset of the start of register
	quint64 offset = getOffset();

	// how many address unit are contained in the register
	unsigned int size = register_->getWidth() / addrUnit;
	if (size*addrUnit < register_->getWidth()) 
    {
		size++; //Round truncated number upwards
	}
	// if size is not defined then never return a negative number
	if (size == 0) {
		return 0;
	}

	// the last address contained in the register
	return offset + size - 1;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::setWidth()
//-----------------------------------------------------------------------------
void RegisterGraphItem::setWidth(qreal width)
{
    if (childWidth_ != width)
    {
        childWidth_ = width;
        VisualizerItem::setWidth(width);
        reorganizeChildren();
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::widthForChild()
//-----------------------------------------------------------------------------
 qreal RegisterGraphItem::widthForChild(MemoryVisualizationItem* child) const
{
    Q_ASSERT(child);
    if (child->isCompletelyOverlapped())
    {
        return 0;
    }

    unsigned int highBit = child->getDisplayOffset();

    if (highBit > register_->getMSB())
    {       
        highBit = register_->getMSB();
    }

    unsigned int lowBit = child->getDisplayLastAddress();

    if (lowBit > register_->getMSB())
    {
        return 0;
    }

    quint64 bits =  qMin(highBit - lowBit + 1, register_->getSize());    
    qreal bitWidth = childWidth_/register_->getSize();
    return bitWidth * bits;
 }

 //-----------------------------------------------------------------------------
 // Function: RegisterGraphItem::compareItems()
 //-----------------------------------------------------------------------------
bool RegisterGraphItem::compareItems(MemoryVisualizationItem const* s1, MemoryVisualizationItem const* s2)
{
    // Field with higher offset (lsb) precedes lower offset.
    return s1->getOffset() > s2->getOffset();
}
