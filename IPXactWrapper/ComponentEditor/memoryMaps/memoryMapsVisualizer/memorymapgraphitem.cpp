/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "memorymapgraphitem.h"
#include "addressblockgraphitem.h"

#include <models/memorymapitem.h>
#include <models/addressblock.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>

#include <QBrush>
#include <QColor>

#include <QDebug>

MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> memoryMap,
									   QGraphicsItem* parent):
MemoryVisualizationItem(parent),
memoryMap_(memoryMap) {

	Q_ASSERT(memoryMap_);
	setBrush(QBrush(QColor(100, 200, 255)));
}

MemoryMapGraphItem::~MemoryMapGraphItem() {
}

void MemoryMapGraphItem::refresh() {
	
	setName(memoryMap_->getName());
	setLeftTopCorner(memoryMap_->getFirstAddressStr());
	setLeftBottomCorner(memoryMap_->getLastAddressStr());

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();

	MemoryMapScene* memScene = static_cast<MemoryMapScene*>(scene());
	Q_ASSERT(memScene);
	memScene->rePosition();
}

quint64 MemoryMapGraphItem::getOffset() const {
	return memoryMap_->getFirstAddress();
}

int MemoryMapGraphItem::getBitWidth() const {
	return 0;
}

unsigned int MemoryMapGraphItem::getAddressUnitSize() const {
	return memoryMap_->getAddressUnitBits();
}

quint64 MemoryMapGraphItem::getLastAddress() const {
	return memoryMap_->getLastAddress();
}
