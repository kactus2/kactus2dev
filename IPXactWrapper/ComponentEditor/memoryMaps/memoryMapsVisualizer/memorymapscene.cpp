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

#include <QDebug>

MemoryMapScene::MemoryMapScene(QObject *parent):
QGraphicsScene(parent),
memoryMaps_(),
memGraphItems_() {

}

MemoryMapScene::~MemoryMapScene() {
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
        memMap->setWidth(400);
		previous = memMap;
	}

	// tell view to draw the items
	invalidate();
}
