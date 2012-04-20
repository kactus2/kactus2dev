/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapscene.cpp
 *		Project: Kactus 2
 */

#include "memorymapscene.h"
#include "memorymapgraphitem.h"

#include <QRectF>

#include <QDebug>

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
	foreach (QSharedPointer<MemoryMap> memMap, memoryMaps) {

		MemoryMapGraphItem* item = new MemoryMapGraphItem(memMap);
		addItem(item);

		// if there is a previous then add the new after it
		if (previous) {
			item->setPos(0, previous->boundingRect().bottom() + 10);

			qDebug() << "bottom: " << previous->boundingRect().bottom() + 10;
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
