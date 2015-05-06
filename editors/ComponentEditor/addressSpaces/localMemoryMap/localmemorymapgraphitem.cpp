/* 
 *  	Created on: 17.12.2012
 *      Author: Antti Kamppi
 * 		filename: localmemorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "localmemorymapgraphitem.h"
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>

LocalMemoryMapGraphItem::LocalMemoryMapGraphItem(QSharedPointer<AddressSpace> addrSpace, 
												 QSharedPointer<MemoryMap> localMemoryMap,
												 QGraphicsItem* parent /*= 0*/ ):
MemoryMapGraphItem(localMemoryMap, localMemoryMap, parent),
addrSpace_(addrSpace)
{   
}

LocalMemoryMapGraphItem::~LocalMemoryMapGraphItem()
{
}

unsigned int LocalMemoryMapGraphItem::getAddressUnitSize() const
{
	return addrSpace_->getAddressUnitBits();
}

void LocalMemoryMapGraphItem::refresh()
{
    QString name = memoryMap_->getName();
    if (name.isEmpty())
    {
        name = "Local memory map";
    }

    setName(name);

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setLeftTopCorner(offset);
    setLeftBottomCorner(lastAddress);

    setToolTip("<b>Name: </b>" + name + "<br>" +
        "<b>AUB: </b>" + QString::number(getAddressUnitSize()) + "<br>" +
        "<b>First address: </b>" + toHexString(offset) + "<br>" +
        "<b>Last address: </b>" + toHexString(lastAddress));

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();

	MemoryMapScene* memScene = static_cast<MemoryMapScene*>(scene());
	Q_ASSERT(memScene);
	memScene->rePosition();
}
