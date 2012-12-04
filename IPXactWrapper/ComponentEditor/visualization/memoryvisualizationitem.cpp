/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: memoryvisualizationitem.cpp
 *		Project: Kactus 2
 */

#include "memoryvisualizationitem.h"
#include "memorygapitem.h"

#include <QRectF>

#include <QDebug>

MemoryVisualizationItem::MemoryVisualizationItem( QGraphicsItem* parent /*= 0*/ ):
ExpandableItem(parent) {

}

MemoryVisualizationItem::~MemoryVisualizationItem() {
}

void MemoryVisualizationItem::reorganizeChildren() {
	// first find out the width for all items
	qreal width = VisualizerItem::itemTotalWidth();

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

	qreal yCoordinate = rect().bottom();
	MemoryVisualizationItem* previous = NULL;
	foreach (MemoryVisualizationItem* item, childItems_) {

		Q_ASSERT(item);
		item->setWidth(width);

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
		previousBlockEnd = item->getLastAddress();
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
	setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);
	
	// reorganize the text blocks of this item
	ExpandableItem::reorganizeChildren();
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
	// update the original map
	childItems_ = newMap;
}

void MemoryVisualizationItem::setWidth( qreal width ) {
	setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);
	ExpandableItem::reorganizeChildren();
}
