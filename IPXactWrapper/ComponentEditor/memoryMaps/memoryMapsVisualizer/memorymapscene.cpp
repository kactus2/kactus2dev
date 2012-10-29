/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapscene.cpp
 *		Project: Kactus 2
 */

#include "memorymapscene.h"
#include "memorymapgraphitem.h"

#include <QRectF>

MemoryMapScene::MemoryMapScene(QObject *parent):
QGraphicsScene(parent),
memoryMaps_() {

}

MemoryMapScene::~MemoryMapScene() {
}

void MemoryMapScene::setMemoryMaps( QList<QSharedPointer<MemoryMap> >& memoryMaps ) {
	memoryMaps_ = memoryMaps;

	// remove all previous objects
	clear();

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
			item->setPos(0, previousRect.bottom() + 10);
		}
		// if this is the first item to add
		else {
			item->setPos(0, 0);
		}

		connect(item, SIGNAL(contentChanged()),
			this, SIGNAL(contentChanged()), Qt::UniqueConnection);

		previous = item;
	}

	// tell view to draw the items
	//invalidate(QRectF(), QGraphicsScene::ItemLayer);
	invalidate();
}
