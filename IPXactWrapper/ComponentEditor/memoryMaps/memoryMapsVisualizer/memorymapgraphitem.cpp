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

#include <QBrush>
#include <QColor>

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

// 	QList<QSharedPointer<MemoryMapItem> >& memItems = memoryMap_->getItems();
// 	foreach (QSharedPointer<MemoryMapItem> item, memItems) {
// 
// 		// if the sub item is an address block then create a child graph item for it
// 		QSharedPointer<AddressBlock> addrBlock = item.dynamicCast<AddressBlock>();
// 		if (addrBlock) {
// 			// create the item
// 			AddressBlockGraphItem* adGraphItem = new AddressBlockGraphItem(addrBlock, this);
// 			
// 			// get the offset of the item
// 			int offset = adGraphItem->getOffset();
// 
// 			// make sure the items are in correct order for the offset
// 			childItems_.insert(offset, adGraphItem);
// 
// 			// tell child to check its children
// 			adGraphItem->refresh();
// 		}
// 	}

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();
}

int MemoryMapGraphItem::getOffset() const {
	return 0;
}

int MemoryMapGraphItem::getBitWidth() const {
	return 0;
}
