/* 
 *  	Created on: 17.12.2012
 *      Author: Antti Kamppi
 * 		filename: localmemorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "localmemorymapgraphitem.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>

LocalMemoryMapGraphItem::LocalMemoryMapGraphItem(QSharedPointer<AddressSpace> addrSpace, 
												 QSharedPointer<MemoryMap> localMemoryMap,
												 QGraphicsItem* parent /*= 0*/ ):
MemoryMapGraphItem(localMemoryMap, parent),
addrSpace_(addrSpace) {
}

LocalMemoryMapGraphItem::~LocalMemoryMapGraphItem() {
}

unsigned int LocalMemoryMapGraphItem::getAddressUnitSize() const {
	return addrSpace_->getAddressUnitBits();
}

void LocalMemoryMapGraphItem::refresh() {
	setName(tr("Local memory map"));
	setLeftTopCorner(memoryMap_->getFirstAddress());
	setLeftBottomCorner(memoryMap_->getLastAddress());

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();

	MemoryMapScene* memScene = static_cast<MemoryMapScene*>(scene());
	Q_ASSERT(memScene);
	memScene->rePosition();
}
