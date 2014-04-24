/* 
 *  	Created on: 17.12.2012
 *      Author: Antti Kamppi
 * 		filename: localmemorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "localmemorymapgraphitem.h"
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>

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

    QString name = memoryMap_->getName();
    if (name.isEmpty())
    {
        name = "Local memory map";
    }

    setName(name);
    setLeftTopCorner(memoryMap_->getFirstAddress());
    setLeftBottomCorner(memoryMap_->getLastAddress());

    setToolTip("<b>Name: </b>" + memoryMap_->getName() + "<br>" +
        "<b>AUB: </b>" + QString::number(getAddressUnitSize()) + "<br>" +
        "<b>First address: </b>" + AddressSpaceVisualizationItem::addr2Str(getOffset(), getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + AddressSpaceVisualizationItem::addr2Str(memoryMap_->getLastAddress(), 
        getBitWidth()));

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();

	MemoryMapScene* memScene = static_cast<MemoryMapScene*>(scene());
	Q_ASSERT(memScene);
	memScene->rePosition();
}
