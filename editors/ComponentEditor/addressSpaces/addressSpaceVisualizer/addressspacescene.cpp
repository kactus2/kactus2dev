//-----------------------------------------------------------------------------
// File: addressspacescene.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 22.12.2012
//
// Description:
// The graphics scene containing the segments and local memory map of an address space.
//-----------------------------------------------------------------------------

#include "addressspacescene.h"
#include "segmentgraphitem.h"
#include "addressspacevisualizationitem.h"
#include "addressspacegapitem.h"
#include "localaddrblockgraphitem.h"
#include "AddressSpaceConflictedItem.h"

#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <common/graphicsItems/visualizeritem.h>

//-----------------------------------------------------------------------------
// Function: AddressSpaceScene()
//-----------------------------------------------------------------------------
AddressSpaceScene::AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace,
     QSharedPointer<ExpressionParser> expressionParser,
									 QObject *parent):
QGraphicsScene(parent),
addrSpace_(addrSpace),
segmentItems_(),
addrBlockItems_(),
exceedingSegments_(),
exceedingAddrBlocks_(),
expressionParser_(expressionParser)
{
}

//-----------------------------------------------------------------------------
// Function: ~AddressSpaceScene()
//-----------------------------------------------------------------------------
AddressSpaceScene::~AddressSpaceScene() 
{
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void AddressSpaceScene::refresh() 
{
	// remove previous items
	clear();
	segmentItems_.clear();
	addrBlockItems_.clear();
    exceedingAddrBlocks_.clear();
    exceedingSegments_.clear();

    quint64 addressSpaceEnd = addressSpaceLastAddress();

    QList<QSharedPointer<Segment> >& segments = addrSpace_->getSegments();
    foreach(QSharedPointer<Segment> current, segments)
    {
        SegmentGraphItem* segItem = new SegmentGraphItem(current, addrSpace_->getWidthExpression(), expressionParser_);
        addItem(segItem);

        if ( segItem->getOffset() > addressSpaceEnd)
        {
            exceedingSegments_.insert(segItem->getOffset(), segItem);
        }
        else
        {
            segmentItems_.insert(segItem->getOffset(), segItem);
        }
    }

    QSharedPointer<MemoryMap> localMap = addrSpace_->getLocalMemoryMap();
    QList<QSharedPointer<MemoryMapItem> >& blocks = localMap->getItems();
    foreach (QSharedPointer<MemoryMapItem> memItem, blocks)
    {
        QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
        if (addrBlock) {

            LocalAddrBlockGraphItem* blockItem = new LocalAddrBlockGraphItem(addrBlock, 
                addrSpace_->getWidthExpression(), expressionParser_);
            addItem(blockItem);

            if (blockItem->getOffset() > addressSpaceEnd)
            {
                exceedingAddrBlocks_.insert(blockItem->getOffset(), blockItem);
            }
            else
            {
                addrBlockItems_.insert(blockItem->getOffset(), blockItem);
            }
        }
    }

	rePosition();
}

//-----------------------------------------------------------------------------
// Function: rePosition()
//-----------------------------------------------------------------------------
void AddressSpaceScene::rePosition() 
{
    updateMaps(segmentItems_, exceedingSegments_, AddressSpaceGapItem::ALIGN_RIGHT);
    updateMaps(addrBlockItems_, exceedingAddrBlocks_, AddressSpaceGapItem::ALIGN_LEFT);

	// remember which segment and address block are being operated on
	QMultiMap<quint64, AddressSpaceVisualizationItem*>::iterator segIterator = segmentItems_.begin();
	QMultiMap<quint64, AddressSpaceVisualizationItem*>::iterator blockIterator = addrBlockItems_.begin();
	
	// the previous segment and address block
	AddressSpaceVisualizationItem* prevSeg = NULL;
	AddressSpaceVisualizationItem* prevBlock = NULL;

    // Y-coordinates for items.
	qreal segCoord = 0;
	qreal blockCoord = 0;

	// as long as there are both address blocks and segments left
	while ((segIterator != segmentItems_.end()) && (blockIterator != addrBlockItems_.end())) {

		AddressSpaceVisualizationItem* seg = segIterator.value();
		AddressSpaceVisualizationItem* block = blockIterator.value();

        // if segment is overlapped by others, it need not be positioned.
        if (seg->isCompletelyOverlapped())
        {
            ++segIterator;
            continue;
        }
        // if address block is overlapped by others, it need not be positioned.
        if (block->isCompletelyOverlapped())
        {          
            ++blockIterator;
            continue;
        }

		// if both have the same starting address
		if (seg->getOverlappingTop() == block->getOverlappingTop())
        {
			// the y-coordinate where the both items are aligned
			qreal yCoord = qMax(segCoord, blockCoord);

			// if there are previous items then make them reach the starting point for
			// these new items
			if (prevSeg) {
				prevSeg->setBottomCoordinate(yCoord);
			}
			if (prevBlock) {
				prevBlock->setBottomCoordinate(yCoord);
			}

			// update the positions
			seg->setPos(0, yCoord);
			block->setPos(VisualizerItem::DEFAULT_WIDTH, yCoord);

			// check if the items are different sizes
			
			// if block reaches further than segment
            if (seg->getOverlappingBottom() < block->getOverlappingBottom()) {
                qreal segHeight = AddressSpaceVisualizationItem::SEGMENT_HEIGHT;      
                // Segment displaying one address should be smaller.  
                if (seg->getOverlappingTop() == seg->getOverlappingBottom())
                {
                    segHeight = AddressSpaceVisualizationItem::MIN_HEIGHT;
                }
                seg->setHeight(segHeight);
                segCoord += segHeight;

                // Find the bottom of the address block.
				blockCoord = segCoord;
                // If the next block begins at the ending address of the segment,
                // they should align.
                if (block->getOverlappingBottom() == seg->getOverlappingBottom() + 1)
                {
                    blockCoord += AddressSpaceVisualizationItem::MIN_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }
                // Otherwise the segment should stretch further down.
                else
                {
                    blockCoord += AddressSpaceVisualizationItem::SEGMENT_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }                

				block->setBottomCoordinate(blockCoord);
				
				// the segment has been processed but block is needed on next loop iteration
				prevSeg = seg;
				++segIterator;
			}

			// if segment reaches further than block
			else if (seg->getOverlappingBottom() > block->getOverlappingBottom()) {                
                qreal blockHeight = AddressSpaceVisualizationItem::SEGMENT_HEIGHT;
                // Block displaying one address should be smaller.  
                if (block->getOverlappingTop() == block->getOverlappingBottom())
                {
                    blockHeight = AddressSpaceVisualizationItem::MIN_HEIGHT;
                }
                block->setHeight(blockHeight);
                blockCoord += blockHeight;

				segCoord = blockCoord;
                // If the next block begins at the ending address of the segment,
                // they should align.
                if (seg->getOverlappingBottom() == block->getOverlappingBottom() + 1)
                {
                    segCoord += AddressSpaceVisualizationItem::MIN_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }
                // Otherwise the segment should stretch further down.
                else
                {
                    segCoord += AddressSpaceVisualizationItem::SEGMENT_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }                

				seg->setBottomCoordinate(segCoord);	
				
				// the block has been processed but segment is needed on next loop iteration
				prevBlock = block;
				++blockIterator;
			}
			// if blocks are of same size
			else {
                qreal height = AddressSpaceVisualizationItem::SEGMENT_HEIGHT;
                 // Block and segment displaying one address should be smaller. 
                if (seg->getOverlappingTop() == seg->getOverlappingBottom())
                {
                    height = AddressSpaceVisualizationItem::MIN_HEIGHT;
                }
				seg->setHeight(height);
				block->setHeight(height);

				// update the y-coordinates for next items
				segCoord += height;
				blockCoord += height;
				 
				// both blocks are processed and next loop iteration starts on new items
				prevSeg = seg;
				prevBlock = block;
				++segIterator;
				++blockIterator;
			}
		}

		// if the segment starts before the block
		else if (seg->getOverlappingTop() < block->getOverlappingTop()) {

			block->setPos(VisualizerItem::DEFAULT_WIDTH, blockCoord);

			// if block reaches further than segment
			if (seg->getOverlappingBottom() < block->getOverlappingBottom()) {
                
                // Find the bottom of the segment.
                blockCoord = seg->mapToScene(seg->rect().bottomRight()).y();
                // If the next segment begins at the ending address of the block,
                // they should align.
                if (block->getOverlappingBottom() == seg->getOverlappingBottom() + 1)
                {
                    blockCoord += AddressSpaceVisualizationItem::MIN_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }
                // Otherwise the block should stretch further down.
                else
                {
                    blockCoord += AddressSpaceVisualizationItem::SEGMENT_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }
                block->setBottomCoordinate(blockCoord);

				// the segment has been processed but block is needed on next loop iteration
				prevSeg = seg;
				++segIterator;
			}

			// if segment reaches further than block
			else if (seg->getOverlappingBottom() > block->getOverlappingBottom()) {
                qreal blockHeight = AddressSpaceVisualizationItem::SEGMENT_HEIGHT;
                if (block->getOverlappingTop() == block->getOverlappingBottom())
                {
                    blockHeight = AddressSpaceVisualizationItem::MIN_HEIGHT;
                }
				block->setHeight(blockHeight);
				blockCoord += blockHeight;

				// Find the bottom of the address block.
				segCoord = blockCoord;
                // If the next block begins at the ending address of the segment,
                // they should align.
                if (seg->getOverlappingBottom() == block->getOverlappingBottom() + 1)
                {
                    segCoord += AddressSpaceVisualizationItem::MIN_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }
                // Otherwise the segment should stretch further down.
                else
                {
                    segCoord += AddressSpaceVisualizationItem::SEGMENT_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }                

				seg->setBottomCoordinate(segCoord);

				// the block has been processed but segment is needed on next loop iteration
				prevBlock = block;
				++blockIterator;
			}
			// if blocks have the same ending address
			else {
                // Block displaying one address should be smaller. 
                if ( block->getOverlappingTop() != block->getOverlappingBottom())
                {
                    blockCoord += AddressSpaceVisualizationItem::SEGMENT_HEIGHT;
                }
                else
                {
                    blockCoord += AddressSpaceVisualizationItem::MIN_HEIGHT;
                }
				// the common ending coordinate
				qreal yCoord = qMax(segCoord, blockCoord);

				seg->setBottomCoordinate(yCoord);
				segCoord = yCoord;

				block->setBottomCoordinate(yCoord);
				blockCoord = yCoord;

				// both blocks are processed and next loop iteration starts on new items
				prevSeg = seg;
				prevBlock = block;
				++segIterator;
				++blockIterator;
			}
		}

		// if the block starts before the segment
		else if (seg->getOverlappingTop() > block->getOverlappingTop()) {
			
			seg->setPos(0, segCoord);
 
 			// if block reaches further than segment
			if (seg->getOverlappingBottom() < block->getOverlappingBottom()) {
                qreal segHeight = AddressSpaceVisualizationItem::SEGMENT_HEIGHT;
                // Segment displaying one address should be smaller.
                if (seg->getOverlappingTop() == seg->getOverlappingBottom())
                {
                    segHeight = AddressSpaceVisualizationItem::MIN_HEIGHT;
                }
				seg->setHeight(segHeight);
				segCoord += segHeight;

				// Find the bottom of the segment.
                blockCoord = seg->mapToScene(seg->rect().bottomRight()).y();
                // If the next segment begins at the ending address of the address block,
                // they should align.
                if (block->getOverlappingBottom() == seg->getOverlappingBottom() + 1)
                {
                    blockCoord += AddressSpaceVisualizationItem::MIN_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }
                // Otherwise the block should stretch further down.
                else
                {
                    blockCoord += AddressSpaceVisualizationItem::SEGMENT_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }                

                block->setBottomCoordinate(blockCoord);
                // the segment has been processed but block is needed on next loop iteration
                prevSeg = seg;
				++segIterator;
			}

			// if segment reaches further than block
			else if (seg->getOverlappingBottom() > block->getOverlappingBottom()) {
                // Find the bottom of the block.
                segCoord = block->mapToScene(block->rect().bottomRight()).y();
                // If the next address block begins at the ending address of the segment,
                // they should align.
                if (seg->getOverlappingBottom() == block->getOverlappingBottom() + 1)
                {
                    segCoord += AddressSpaceVisualizationItem::MIN_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }
                // Otherwise the segment should stretch further down.
                else
                {
                    segCoord += AddressSpaceVisualizationItem::SEGMENT_HEIGHT +  AddressSpaceVisualizationItem::PEN_WIDTH;
                }
                seg->setBottomCoordinate(segCoord);

				// the block has been processed but segment is needed on next loop iteration
				prevBlock = block;
				++blockIterator;
			}
			// if blocks have the same ending address
            else {
                 // Block and segment displaying one address should be smaller.
                if (seg->getOverlappingTop() != seg->getOverlappingBottom())
                {
                    segCoord += AddressSpaceVisualizationItem::SEGMENT_HEIGHT;
                }
                else
                {
                    segCoord += AddressSpaceVisualizationItem::MIN_HEIGHT;
                }
				// the common ending coordinate
				qreal yCoord = qMax(segCoord, blockCoord);

				seg->setBottomCoordinate(yCoord);
				segCoord = yCoord;

				block->setBottomCoordinate(yCoord);
				blockCoord = yCoord;

				// both blocks are processed and next loop iteration starts on new items
				prevSeg = seg;
				prevBlock = block;
				++segIterator;
				++blockIterator;
			}
		}		
    }   

    rePositionExceeding(qMax(segCoord, blockCoord)); 
}

//-----------------------------------------------------------------------------
// Function: updateMaps()
//-----------------------------------------------------------------------------
void AddressSpaceScene::updateMaps(QMultiMap<quint64, AddressSpaceVisualizationItem*>& itemMap, 
    QMultiMap<quint64, AddressSpaceVisualizationItem*> const& exceedingItemMap, 
    AddressSpaceGapItem::AddressPosition const align) 
{
    quint64 addressSpaceEnd = addressSpaceLastAddress();

    // Conflict all items outside address space.
    foreach (AddressSpaceVisualizationItem* exceedingItem, exceedingItemMap)
    {
        exceedingItem->setConflicted(true);
        exceedingItem->refresh();
    }

    // Replacing, updated map for itemMap.
    QMultiMap<quint64, AddressSpaceVisualizationItem*> newMap;

    // go through all items and update the segment offsets and remove gaps
    foreach (AddressSpaceVisualizationItem* item, itemMap) {

        // if the item is gap item then remove it
        AddressSpaceGapItem* gap = dynamic_cast<AddressSpaceGapItem*>(item);
        if (gap)
        {
            removeItem(gap);
			delete gap;
			gap = NULL;
            continue;
        }

        // update the offset for other than gap items
        quint64 offset = item->getOffset();
        newMap.insert(offset, item);
        if (item->getLastAddress() > addressSpaceEnd)
        {
            item->setConflicted(true);
        }
        item->refresh();           
    }

    // The top-most item.
    AddressSpaceVisualizationItem* topItem = NULL;

    // Previous conflict block.
    AddressSpaceVisualizationItem* prevConflict = NULL;

    // Last processed address.    
    quint64 lastAddress = 0;

    // add gaps where a segment is not defined.
    foreach (AddressSpaceVisualizationItem* item, newMap) 
    {
        // if there is a gap between the last item and this item or first item
        // begins at address 1.
        if (item->getOffset() > lastAddress + 1 || 
            (topItem == NULL && item->getOffset() == lastAddress + 1)) {
            // create the gap item.
            AddressSpaceGapItem* gap = new AddressSpaceGapItem(align, addrSpace_->getWidthExpression(), expressionParser_);
            addItem(gap);

            // if the gap is at the start of the address space.
            if (lastAddress == 0 && !topItem) {
                gap->setStartAddress(lastAddress, true);
            }
            // if the gap is between items.
            else {
                gap->setStartAddress(lastAddress, false);
            }

            // the gap ends at the start of the next item.
            gap->setEndAddress(item->getOffset(), false);
            gap->refresh();

            // add the gap to the new map.
            newMap.insert(gap->getOffset(), gap);
        }

        // Check for overlapping memory blocks.
        resolveConflicts(item, topItem, prevConflict, align, newMap);       

		// update the last address of the item.
		lastAddress = qMax(item->getLastAddress(), lastAddress);
	}

	// add a gap to the end of the address space if last item < addr space size.
	if (addressSpaceEnd > lastAddress || 
        (newMap.empty() && newMap == segmentItems_ && !addrBlockItems_.empty() )) 
        {
		// create the gap item.
		AddressSpaceGapItem* gap = new AddressSpaceGapItem(align, addrSpace_->getWidthExpression(), expressionParser_);
		addItem(gap);

	    // set the first address of the gap.
		// if there were no address items then the end gap is also first gap.
		if (lastAddress == 0 && topItem == NULL) {
			gap->setStartAddress(lastAddress, true);
		}
		// otherwise the last gap starts after the previous ended.
		else {
			gap->setStartAddress(lastAddress, false);
		}

		// set the last address for the gap
		gap->setEndAddress(addressSpaceEnd);
		gap->refresh();

		newMap.insert(lastAddress + 1, gap);
	}       


	// finally update the original map to match the updated.
	itemMap.clear();
	itemMap = newMap;
}

//-----------------------------------------------------------------------------
// Function: rePositionExceeding()
//-----------------------------------------------------------------------------
void AddressSpaceScene::rePositionExceeding(qreal const yStart)
{
    // Add segments outside address space to the end.
    qreal yCoordinate = yStart + MARGIN;
    foreach(AddressSpaceVisualizationItem* seg, exceedingSegments_)
    {
        seg->setHeight(AddressSpaceVisualizationItem::SEGMENT_HEIGHT);
        seg->setPos(0, yCoordinate);
        yCoordinate += AddressSpaceVisualizationItem::SEGMENT_HEIGHT;
    }

    // Add address blocks outside address space to the end.
    yCoordinate = yStart + MARGIN;
    foreach(AddressSpaceVisualizationItem* block, exceedingAddrBlocks_)
    {
        block->setHeight(AddressSpaceVisualizationItem::SEGMENT_HEIGHT);
        block->setPos(VisualizerItem::DEFAULT_WIDTH, yCoordinate);
        yCoordinate += AddressSpaceVisualizationItem::SEGMENT_HEIGHT;
    }
}

//-----------------------------------------------------------------------------
// Function: resolveConflicts()
//-----------------------------------------------------------------------------
void AddressSpaceScene::resolveConflicts(AddressSpaceVisualizationItem* currentItem, 
    AddressSpaceVisualizationItem*& topItem, AddressSpaceVisualizationItem*& prevConflict,
    AddressSpaceGapItem::AddressPosition const align,
    QMultiMap<quint64, AddressSpaceVisualizationItem*>& map)
{
    Q_ASSERT(currentItem);

    // If this item overlaps the previous top-most item.
    if (topItem && currentItem->getOffset() <= topItem->getLastAddress())
    {
        currentItem->setOverlappingTop(topItem->getLastAddress() + 1);
        currentItem->setConflicted(true);
        topItem->setConflicted(true);

        // item is completely inside other items.
        if (currentItem->getLastAddress() <= topItem->getLastAddress())
        {            
            currentItem->setCompleteOverlap();
        }

        // Item partially overlaps with previous top-most item.
        else
        {
            quint64 conflictEnd = topItem->getLastAddress();

            // Previous top-most item is completely overlapped by this item and another item.
            if (prevConflict && prevConflict->getLastAddress() + 1 >= currentItem->getOffset())
            {
                topItem->setCompleteOverlap();                            
                //conflictEnd = prevConflict->getLastAddress();
            }

            // Previous top-most item is completely under this item. 
            else if (currentItem->getOffset() == topItem->getOffset())
            {                                   
                topItem->setCompleteOverlap();                  
            }

            // Previous top-most item is partially under this item. 
            else 
            {                 
                topItem->setOverlappingBottom(currentItem->getOffset() - 1);                          
            }

            // Add conflicting item.
            AddressSpaceConflictedItem* conflict = new AddressSpaceConflictedItem(
                align, addrSpace_->getWidthExpression(), expressionParser_);
            addItem(conflict);                
            conflict->setStartAddress(currentItem->getOffset(),true);
            conflict->setEndAddress(conflictEnd,true); 

            map.insert(conflict->getOffset(), conflict);

            prevConflict = conflict;
            topItem = currentItem;
        }

    }
    // No overlapping.
    else
    {
        topItem = currentItem;
    }

}
//-----------------------------------------------------------------------------
// Function: AddressSpaceScene::addressSpaceLastAddress()
//-----------------------------------------------------------------------------
quint64 AddressSpaceScene::addressSpaceLastAddress() const
{
    QString rangeExpression = addrSpace_->getRange();
    quint64 range = expressionParser_->parseExpression(rangeExpression).toUInt();
    if (rangeExpression.isEmpty() || range <= 0)
    {
        return 0;
    }

    return range - 1;
}