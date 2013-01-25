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
#include <models/memorymap.h>
#include <models/memorymapitem.h>
#include <models/addressblock.h>
#include <common/graphicsItems/visualizeritem.h>

AddressSpaceScene::AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace,
									 QObject *parent):
QGraphicsScene(parent),
addrSpace_(addrSpace),
segmentItems_(),
addrBlockItems_() {
}

AddressSpaceScene::~AddressSpaceScene() {
}

void AddressSpaceScene::refresh() {
	// remove previous items
	clear();
	segmentItems_.clear();
	addrBlockItems_.clear();

	QList<QSharedPointer<Segment> >& segs = addrSpace_->getSegments();
	foreach(QSharedPointer<Segment> seg, segs) {
		SegmentGraphItem* segItem = new SegmentGraphItem(addrSpace_, seg);
		addItem(segItem);

		segmentItems_.insert(segItem->getOffset(), segItem);
	}

	QSharedPointer<MemoryMap> localMap = addrSpace_->getLocalMemoryMap();
	QList<QSharedPointer<MemoryMapItem> >& blocks = localMap->getItems();
	foreach (QSharedPointer<MemoryMapItem> memItem, blocks) {

		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) {

			LocalAddrBlockGraphItem* blockItem = new LocalAddrBlockGraphItem(addrSpace_, addrBlock);
			addItem(blockItem);

			addrBlockItems_.insert(blockItem->getOffset(), blockItem);
		}
	}

	rePosition();
}

void AddressSpaceScene::rePosition() {
	// match the positions of address blocks to the offsets of segments and segment gaps
	updateSegments();
	updateAddrBlocks();

	// remember which segment and address block are being operated on
	QMultiMap<quint64, AddressSpaceVisualizationItem*>::iterator segIterator = segmentItems_.begin();
	QMultiMap<quint64, AddressSpaceVisualizationItem*>::iterator blockIterator = addrBlockItems_.begin();
	
	// the previous segment and address block
	AddressSpaceVisualizationItem* prevSeg = NULL;
	AddressSpaceVisualizationItem* prevBlock = NULL;

	qreal segCoord = 0;
	qreal blockCoord = 0;

	// as long as there are both address blocks and segments left
	while ((segIterator != segmentItems_.end()) && (blockIterator != addrBlockItems_.end())) {

		AddressSpaceVisualizationItem* seg = segIterator.value();
		AddressSpaceVisualizationItem* block = blockIterator.value();

		// if both have the same starting address
		if (seg->getOffset() == block->getOffset()) {

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
			block->setPos(VisualizerItem::MAX_WIDTH, yCoord);

			// check if the items are different sizes
			
			// if block reaches further than segment
			if (seg->getLastAddress() < block->getLastAddress()) {

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
			else if (seg->getLastAddress() > block->getLastAddress()) {

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
		else if (seg->getOffset() < block->getOffset()) {

			block->setPos(VisualizerItem::MAX_WIDTH, blockCoord);

			// if block reaches further than segment
			if (seg->getLastAddress() < block->getLastAddress()) {

				block->setHeight(VisualizerItem::ITEM_HEIGHT);
				blockCoord += VisualizerItem::ITEM_HEIGHT;

				// the segment has been processed but block is needed on next loop iteration
				prevSeg = seg;
				++segIterator;
				continue;
			}

			// if segment reaches further than block
			else if (seg->getLastAddress() > block->getLastAddress()) {

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
		else if (seg->getOffset() > block->getOffset()) {
			
			seg->setPos(0, segCoord);
 
 			// if block reaches further than segment
			if (seg->getLastAddress() < block->getLastAddress()) {

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
			else if (seg->getLastAddress() > block->getLastAddress()) {

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
}

void AddressSpaceScene::updateSegments() {
	QMultiMap<quint64, AddressSpaceVisualizationItem*> newMap;

	// go through all items and update the segment offsets and remove gaps
	foreach (AddressSpaceVisualizationItem* seg, segmentItems_) {

		// if the item is gap item then remove it
		AddressSpaceGapItem* gap = dynamic_cast<AddressSpaceGapItem*>(seg);
		if (gap) {
			removeItem(gap);
			delete gap;
			gap = NULL;
			continue;
		}

		// update the offset for other than gap items
		quint64 offset = seg->getOffset();

		// update the graphics of the segment
		seg->refresh();

		newMap.insert(offset, seg);
	}

	// add gaps where a segment is not defined
	quint64 prevSegEnd = 0;
	foreach (AddressSpaceVisualizationItem* seg, newMap) {
	
		// if there is a gap between the last block and this block
		if (seg->getOffset() > prevSegEnd + 1) {
			
			AddressSpaceGapItem* gap = new AddressSpaceGapItem(addrSpace_);
			addItem(gap);

			// if the gap is at the start of the address space
			if (prevSegEnd == 0) {
				gap->setStartAddress(prevSegEnd, true);
			}
			// if the gap is between segments
			else {
				gap->setStartAddress(prevSegEnd, false);
			}
			
			// the gap ends at the start of the next segment
			gap->setEndAddress(seg->getOffset(), false);

			gap->refresh();

			// add the gap to the new map
			newMap.insert(prevSegEnd, gap);
		}

		// update the last address of the block
		prevSegEnd = seg->getLastAddress();
	}

	// add a gap to the end of the address space if last segment < addr space size
	if (addrSpace_->getLastAddress() > prevSegEnd) {
		// create the gap item
		AddressSpaceGapItem* gap = new AddressSpaceGapItem(addrSpace_);
		addItem(gap);

		// if there were no segments then the end gap is also first gap
		if (prevSegEnd == 0) {
			// set the first address of the gap
			gap->setStartAddress(prevSegEnd, true);
		}
		// otherwise the last gap starts after the previous ended
		else {
			// set the first address of the gap
			gap->setStartAddress(prevSegEnd, false);
		}

		// set the last address for the gap
		gap->setEndAddress(addrSpace_->getLastAddress());

		gap->refresh();

		newMap.insert(prevSegEnd + 1, gap);
	}

	// finally update the original map to match the updated
	segmentItems_.clear();
	segmentItems_ = newMap;
}

void AddressSpaceScene::updateAddrBlocks() {
	QMultiMap<quint64, AddressSpaceVisualizationItem*> newMap;

	// go through all items and update the segment offsets and remove gaps
	foreach (AddressSpaceVisualizationItem* block, addrBlockItems_) {

		// if the item is gap item then remove it
		AddressSpaceGapItem* gap = dynamic_cast<AddressSpaceGapItem*>(block);
		if (gap) {
			removeItem(gap);
			delete gap;
			gap = NULL;
			continue;
		}

		// update the offset for other than gap items
		quint64 offset = block->getOffset();

		// update the graphics of the segment
		block->refresh();

		newMap.insert(offset, block);
	}

	// add gaps where a segment is not defined
	quint64 prevBlockEnd = 0;
	foreach (AddressSpaceVisualizationItem* block, newMap) {

		// if there is a gap between the last block and this block
		if (block->getOffset() > prevBlockEnd + 1) {

			AddressSpaceGapItem* gap = new AddressSpaceGapItem(addrSpace_, AddressSpaceGapItem::ALIGN_LEFT);
			addItem(gap);

			// if the gap is at the start of the address space
			if (prevBlockEnd == 0) {
				gap->setStartAddress(prevBlockEnd, true);
			}
			// if the gap is between segments
			else {
				gap->setStartAddress(prevBlockEnd, false);
			}

			// the gap ends at the start of the next segment
			gap->setEndAddress(block->getOffset(), false);

			gap->refresh();

			// add the gap to the new map
			newMap.insert(prevBlockEnd, gap);
		}

		// update the last address of the block
		prevBlockEnd = block->getLastAddress();
	}

	// add a gap to the end of the address space if last segment < addr space size
	if (addrSpace_->getLastAddress() > prevBlockEnd) {
		// create the gap item
		AddressSpaceGapItem* gap = new AddressSpaceGapItem(addrSpace_, AddressSpaceGapItem::ALIGN_LEFT);
		addItem(gap);

		// if there were no address blocks then the end gap is also first gap
		if (prevBlockEnd == 0) {
			// set the first address of the gap
			gap->setStartAddress(prevBlockEnd, true);
		}
		// otherwise the last gap starts after the previous ended
		else {
			// set the first address of the gap
			gap->setStartAddress(prevBlockEnd, false);
		}

		// set the last address for the gap
		gap->setEndAddress(addrSpace_->getLastAddress());

		gap->refresh();

		newMap.insert(prevBlockEnd + 1, gap);
	}

	// finally update the original map to match the updated
	addrBlockItems_.clear();
	addrBlockItems_ = newMap;
}
