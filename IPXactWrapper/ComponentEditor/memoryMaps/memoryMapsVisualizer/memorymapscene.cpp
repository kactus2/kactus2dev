/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapscene.cpp
 *		Project: Kactus 2
 */

#include "memorymapscene.h"
#include "memorymapgraphitem.h"

#include <common/graphicsItems/graphicsexpandcollapseitem.h>

#include <QRectF>

MemoryMapScene::MemoryMapScene(QObject *parent):
QGraphicsScene(parent),
memoryMaps_(),
memGraphItems_() {

}

MemoryMapScene::~MemoryMapScene() {
}

void MemoryMapScene::setMemoryMaps( QList<QSharedPointer<MemoryMap> >& memoryMaps ) {
	memoryMaps_ = memoryMaps;
	refresh();
}

void MemoryMapScene::refresh() {
	// remove all previous objects
// 	clear();
// 
// 	qreal yCoordinate = 0;
// 	MemoryMapGraphItem* previous = 0;
// 	foreach (QSharedPointer<MemoryMap> memMap, memoryMaps_) {
// 
// 		MemoryMapGraphItem* item = new MemoryMapGraphItem(memMap);
// 		addItem(item);
// 
// 		// tell item to update itself and its children
// 		item->refresh();
// 
// 		// if there is a previous then add the new after it
// 		if (previous) {
// 			QRectF previousRect = previous->itemTotalRect();
// 			previousRect |= previous->boundingRect();
// 
// 			// update the y coordinate to avoid setting items on top of each other
// 			yCoordinate += previousRect.bottom() + 10;
// 		}
// 		
// 		item->setPos(0, yCoordinate);
// 
// 		connect(item, SIGNAL(contentChanged()),
// 			this, SIGNAL(contentChanged()), Qt::UniqueConnection);
// 
// 		previous = item;
// 	}
// 
// 	// tell view to draw the items
// 	invalidate();
}

void MemoryMapScene::addMemGraphItem( MemoryMapGraphItem* memGraphItem ) {
	Q_ASSERT(memGraphItem);
	addItem(memGraphItem);
	memGraphItems_.append(memGraphItem);
	connect(memGraphItem, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

void MemoryMapScene::removeMemGraphItem( MemoryMapGraphItem* memGraphItem ) {
	Q_ASSERT(memGraphItem);
	removeItem(memGraphItem);
	memGraphItems_.removeAll(memGraphItem);
}

void MemoryMapScene::rePosition() {
	qreal yCoordinate = 0;
	MemoryMapGraphItem* previous = 0;
	foreach (MemoryMapGraphItem* memMap, memGraphItems_) {

		// if there is a previous then add the new after it
		if (previous) {
			QRectF previousRect = previous->itemTotalRect();
			previousRect |= previous->boundingRect();

			// update the y coordinate to avoid setting items on top of each other
			yCoordinate += previousRect.bottom() + 10;
		}

		memMap->setPos(0, yCoordinate);
		previous = memMap;
	}

	// tell view to draw the items
	invalidate();
}
