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

AddressSpaceScene::AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace,
									 QObject *parent):
QGraphicsScene(parent),
addrSpace_(addrSpace),
segmentItems_() {
}

AddressSpaceScene::~AddressSpaceScene() {
}

void AddressSpaceScene::refresh() {
	// remove previous items
	clear();
	segmentItems_.clear();

	QList<QSharedPointer<Segment> >& segs = addrSpace_->getSegments();
	foreach(QSharedPointer<Segment> seg, segs) {
		SegmentGraphItem* segItem = new SegmentGraphItem(addrSpace_, seg);
		addItem(segItem);

		segmentItems_.insert(segItem->getOffset(), segItem);
	}
	rePosition();
}

void AddressSpaceScene::rePosition() {
	// match the positions of address blocks to the offsets of segments and segment gaps
	updateSegments();
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
	qreal yCoordinate = 0;
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

			// set the position and increase the y-coordinate to avoid overlapping items
			gap->setPos(0, yCoordinate);
			yCoordinate += VisualizerItem::ITEM_HEIGHT;

			gap->refresh();

			// add the gap to the new map
			newMap.insert(prevSegEnd, gap);
		}

		// update the last address of the block
		prevSegEnd = seg->getLastAddress();
		seg->setPos(0, yCoordinate);
		yCoordinate += VisualizerItem::ITEM_HEIGHT;
	}

	// add a gap to the end of the address space if last segment < addr space size
	if (addrSpace_->getLastAddress() > prevSegEnd) {
		// create the gap item
		AddressSpaceGapItem* gap = new AddressSpaceGapItem(addrSpace_);
		addItem(gap);

		// set the first address of the gap
		gap->setStartAddress(prevSegEnd, false);

		// set the last address for the gap
		gap->setEndAddress(addrSpace_->getLastAddress());

		// set the gap to the end of the last item
		gap->setPos(0, yCoordinate);

		gap->refresh();

		newMap.insert(prevSegEnd + 1, gap);
	}

	// finally update the original map to match the updated
	segmentItems_.clear();
	segmentItems_ = newMap;
}
