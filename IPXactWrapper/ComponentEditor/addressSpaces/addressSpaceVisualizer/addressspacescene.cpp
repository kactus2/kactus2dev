/* 
 *  	Created on: 22.12.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacescene.cpp
 *		Project: Kactus 2
 */

#include "addressspacescene.h"
#include "segmentgraphitem.h"
#include "addressspacevisualizationitem.h"
#include "addressspacegapitem.h"
#include "localaddrblockgraphitem.h"
#include "AddressSpaceConflictedItem.h"
#include <models/memorymap.h>
#include <models/memorymapitem.h>
#include <models/addressblock.h>
#include <common/graphicsItems/visualizeritem.h>

//-----------------------------------------------------------------------------
// Function: AddressSpaceScene()
//-----------------------------------------------------------------------------
AddressSpaceScene::AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace,
									 QObject *parent):
QGraphicsScene(parent),
addrSpace_(addrSpace),
segmentItems_(),
addrBlockItems_(),
exceedingSegments_(),
exceedingAddrBlocks_()
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

    QList<QSharedPointer<Segment> >& segs = addrSpace_->getSegments();
    foreach(QSharedPointer<Segment> seg, segs) {
        SegmentGraphItem* segItem = new SegmentGraphItem(addrSpace_, seg);
        addItem(segItem);

        if ( segItem->getOffset() > addrSpace_->getLastAddress())
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
    foreach (QSharedPointer<MemoryMapItem> memItem, blocks) {

        QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
        if (addrBlock) {

            LocalAddrBlockGraphItem* blockItem = new LocalAddrBlockGraphItem(addrSpace_, addrBlock);
            addItem(blockItem);

            if (blockItem->getOffset() > addrSpace_->getLastAddress())
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
		if (seg->getOverlappingTop() == block->getOverlappingTop()) {

			// the y-coordinate where the both items are aligned
			qreal yCoord = qMax(segCoord, blockCoord);

			// if there are previous items then make them reach the starting point for
			// these new items
			if (prevSeg) {
				prevSeg->setBottomCoordinate(yCoord);
			}
			if (prevBlock) {
				prevBlock->setBottomCoordinate(yCoord); //!
			}

			// update the positions
			seg->setPos(0, yCoord);
			block->setPos(VisualizerItem::DEFAULT_WIDTH, yCoord);

			// check if the items are different sizes
			
			// if block reaches further than segment
			if (seg->getOverlappingBottom() < block->getOverlappingBottom()) {

				seg->setHeight(VisualizerItem::ITEM_HEIGHT);
				block->setHeight(VisualizerItem::ITEM_HEIGHT * 1.5);

				// update the y-coordinates for next items
				segCoord += VisualizerItem::ITEM_HEIGHT;
				blockCoord += VisualizerItem::ITEM_HEIGHT * 1.5;
				
				// the segment has been processed but block is needed on next loop iteration
				prevSeg = seg;
				++segIterator;
				continue;
			}

			// if segment reaches further than block
			else if (seg->getOverlappingBottom() > block->getOverlappingBottom()) {

				seg->setHeight(VisualizerItem::ITEM_HEIGHT * 1.5);
				block->setHeight(VisualizerItem::ITEM_HEIGHT);

				// update the y-coordinates for next items
				segCoord += VisualizerItem::ITEM_HEIGHT * 1.5;
				blockCoord += VisualizerItem::ITEM_HEIGHT;
				
				// the block has been processed but segment is needed on next loop iteration
				prevBlock = block;
				++blockIterator;
				continue;
			}
			// if blocks are of same size
			else {

				seg->setHeight(VisualizerItem::ITEM_HEIGHT);
				block->setHeight(VisualizerItem::ITEM_HEIGHT);

				// update the y-coordinates for next items
				segCoord += VisualizerItem::ITEM_HEIGHT;
				blockCoord += VisualizerItem::ITEM_HEIGHT;
				 
				// both blocks are processed and next loop iteration starts on new items
				prevSeg = seg;
				prevBlock = block;
				++segIterator;
				++blockIterator;
				continue;
			}
		}

		// if the segment starts before the block
		else if (seg->getOverlappingTop() < block->getOverlappingTop()) {

			block->setPos(VisualizerItem::DEFAULT_WIDTH, blockCoord);

			// if block reaches further than segment
			if (seg->getOverlappingBottom() < block->getOverlappingBottom()) {

				block->setHeight(VisualizerItem::ITEM_HEIGHT);
				blockCoord += VisualizerItem::ITEM_HEIGHT;

				// the segment has been processed but block is needed on next loop iteration
				prevSeg = seg;
				++segIterator;
				continue;
			}

			// if segment reaches further than block
			else if (seg->getOverlappingBottom() > block->getOverlappingBottom()) {

				block->setHeight(VisualizerItem::ITEM_HEIGHT);
				blockCoord += VisualizerItem::ITEM_HEIGHT;

				// segment reaches past the block
				segCoord = block->mapToScene(block->rect().bottomRight()).y() + (VisualizerItem::ITEM_HEIGHT / 2);
				seg->setBottomCoordinate(segCoord);

				// the block has been processed but segment is needed on next loop iteration
				prevBlock = block;
				++blockIterator;
				continue;
			}
			// if blocks have the same ending address
			else {

				blockCoord += VisualizerItem::ITEM_HEIGHT;

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
				continue;
			}
		}

		// if the block starts before the segment
		else if (seg->getOverlappingTop() > block->getOverlappingTop()) {
			
			seg->setPos(0, segCoord);
 
 			// if block reaches further than segment
			if (seg->getOverlappingBottom() < block->getOverlappingBottom()) {

				seg->setHeight(VisualizerItem::ITEM_HEIGHT);
				segCoord += VisualizerItem::ITEM_HEIGHT;

				// block reaches past the segment
				blockCoord = seg->mapToScene(seg->rect().bottomRight()).y() + (VisualizerItem::ITEM_HEIGHT / 2);
				block->setBottomCoordinate(blockCoord);

				// the segment has been processed but block is needed on next loop iteration
				prevSeg = seg;
				++segIterator;
				continue;
			}

			// if segment reaches further than block
			else if (seg->getOverlappingBottom() > block->getOverlappingBottom()) {

				seg->setHeight(VisualizerItem::ITEM_HEIGHT);
				segCoord += VisualizerItem::ITEM_HEIGHT;

				// the block has been processed but segment is needed on next loop iteration
				prevBlock = block;
				++blockIterator;
				continue;
			}
			// if blocks have the same ending address
			else {

				segCoord += VisualizerItem::ITEM_HEIGHT;

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
				continue;
			}
		}		
    }   

    rePositionExceeding(qMax(segCoord,blockCoord)); 
}

//-----------------------------------------------------------------------------
// Function: updateMaps()
//-----------------------------------------------------------------------------
void AddressSpaceScene::updateMaps(QMultiMap<quint64, AddressSpaceVisualizationItem*>& itemMap, 
    QMultiMap<quint64, AddressSpaceVisualizationItem*> const& exceedingItemMap, 
    AddressSpaceGapItem::AddressPosition const align) 
{
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
        if (gap) {
            removeItem(gap);
			delete gap;
			gap = NULL;
            continue;
        }

        // update the offset for other than gap items
        quint64 offset = item->getOffset();
        newMap.insert(offset, item);
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
            AddressSpaceGapItem* gap = new AddressSpaceGapItem(addrSpace_, align);
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
	if (addrSpace_->getLastAddress() > lastAddress) {
		// create the gap item.
		AddressSpaceGapItem* gap = new AddressSpaceGapItem(addrSpace_, align);
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
		gap->setEndAddress(addrSpace_->getLastAddress());
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
        seg->setHeight(VisualizerItem::ITEM_HEIGHT);
        seg->setPos(0, yCoordinate);
        yCoordinate += VisualizerItem::ITEM_HEIGHT;
    }

    // Add address blocks outside address space to the end.
    yCoordinate = yStart + MARGIN;
    foreach(AddressSpaceVisualizationItem* block, exceedingAddrBlocks_)
    {
        block->setHeight(VisualizerItem::ITEM_HEIGHT);
        block->setPos(VisualizerItem::DEFAULT_WIDTH, yCoordinate);
        yCoordinate += VisualizerItem::ITEM_HEIGHT;
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
            AddressSpaceConflictedItem* conflict = new AddressSpaceConflictedItem(addrSpace_, 
                align);
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
