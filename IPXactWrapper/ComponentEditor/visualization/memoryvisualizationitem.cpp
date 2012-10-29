/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: memoryvisualizationitem.cpp
 *		Project: Kactus 2
 */

#include "memoryvisualizationitem.h"

MemoryVisualizationItem::MemoryVisualizationItem( QGraphicsItem* parent /*= 0*/ ):
ExpandableItem(parent) {

}

MemoryVisualizationItem::~MemoryVisualizationItem() {
}

void MemoryVisualizationItem::reorganizeChildren() {
	// first find out the width for all items
	qreal width = VisualizerItem::itemTotalWidth();

	MemoryVisualizationItem* previous = NULL;
	foreach (MemoryVisualizationItem* item, childItems_) {

		Q_ASSERT(item);
		item->setWidth(width);

		// if there has been previous item drawn
		if (previous) {

			// set the next item to start after the previous
			item->setPos(0, mapRectFromItem(previous, previous->itemTotalRect()).bottom());
		}
		// if this is the first item to draw
		else {
			// draw in straight under this item
			item->setPos(0, rect().bottom());
		}

		previous = item;
	}

	// update the width of this item to match the width of all
	setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);
	
	// reorganize the text blocks of this item
	VisualizerItem::reorganizeChildren();
}
