//-----------------------------------------------------------------------------
// File: localmemorymapgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 17.12.2012
//
// Description:
// The graph item that visualizes a local memory map within address space.
//-----------------------------------------------------------------------------

#include "localmemorymapgraphitem.h"

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapGraphItem::LocalMemoryMapGraphItem()
//-----------------------------------------------------------------------------
LocalMemoryMapGraphItem::LocalMemoryMapGraphItem(QSharedPointer<AddressSpace> addrSpace, 
												 QSharedPointer<MemoryMap> localMemoryMap,
												 QGraphicsItem* parent /*= 0*/ ):
MemoryMapGraphItem(localMemoryMap, localMemoryMap, parent),
addrSpace_(addrSpace)
{   
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapGraphItem::~LocalMemoryMapGraphItem()
//-----------------------------------------------------------------------------
LocalMemoryMapGraphItem::~LocalMemoryMapGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int LocalMemoryMapGraphItem::getAddressUnitSize() const
{
	return addrSpace_->getAddressUnitBits();
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapGraphItem::refresh()
//-----------------------------------------------------------------------------
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
}
