/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: memoryvisualizationitem.cpp
 *		Project: Kactus 2
 */

#include "memoryvisualizationitem.h"

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

	qreal yCoordinate = rect().bottom();
	MemoryVisualizationItem* previous = NULL;
	foreach (MemoryVisualizationItem* item, childItems_) {

		Q_ASSERT(item);
		item->setWidth(width);

		// if there has been previous item drawn
		if (previous) {

			// set the next item to start after the previous
			QRectF previousRect = mapRectFromItem(previous, previous->itemTotalRect());
			
			// update the y coordinate to avoid setting items on top of each other
			yCoordinate = previousRect.bottom();
		}

		qDebug() << "Adding:" << item->getName() << " to:" << yCoordinate;

		item->setPos(0, yCoordinate);
		previous = item;
	}

	// update the width of this item to match the width of all
	setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);
	
	// reorganize the text blocks of this item
	VisualizerItem::reorganizeChildren();
}
