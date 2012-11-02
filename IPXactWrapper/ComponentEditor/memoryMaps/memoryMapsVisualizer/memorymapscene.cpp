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
memoryMaps_() {

}

MemoryMapScene::~MemoryMapScene() {
}

void MemoryMapScene::setMemoryMaps( QList<QSharedPointer<MemoryMap> >& memoryMaps ) {
	memoryMaps_ = memoryMaps;
	refresh();
}

void MemoryMapScene::refresh() {
	// remove all previous objects
	clear();

	qreal yCoordinate = 0;
	MemoryMapGraphItem* previous = 0;
	foreach (QSharedPointer<MemoryMap> memMap, memoryMaps_) {

		MemoryMapGraphItem* item = new MemoryMapGraphItem(memMap);
		addItem(item);

		// tell item to update itself and its children
		item->refresh();

		// if there is a previous then add the new after it
		if (previous) {
			QRectF previousRect = previous->childrenBoundingRect();
			previousRect |= previous->boundingRect();

			// update the y coordinate to avoid setting items on top of each other
			yCoordinate += previousRect.bottom() + 10;
		}
		
		item->setPos(0, yCoordinate);

		connect(item, SIGNAL(contentChanged()),
			this, SIGNAL(contentChanged()), Qt::UniqueConnection);

		previous = item;
	}

	// tell view to draw the items
	invalidate();
}
