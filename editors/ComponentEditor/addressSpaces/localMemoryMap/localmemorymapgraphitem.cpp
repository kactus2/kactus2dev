//-----------------------------------------------------------------------------
// File: localmemorymapgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 17.12.2012
//
// Description:
// The graph item that visualizes a local memory map within address space.
//-----------------------------------------------------------------------------

#include "localmemorymapgraphitem.h"

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressSpace.h>

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapGraphItem::LocalMemoryMapGraphItem()
//-----------------------------------------------------------------------------
LocalMemoryMapGraphItem::LocalMemoryMapGraphItem(QSharedPointer<AddressSpace> addrSpace,
    QSharedPointer<MemoryMapBase> localMemoryMap, QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent):
MemoryMapGraphItem(QSharedPointer<MemoryMap> (new MemoryMap()), localMemoryMap, expressionParser, parent),
addrSpace_(addrSpace)
{

}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int LocalMemoryMapGraphItem::getAddressUnitSize() const
{
    return parseExpression(addrSpace_->getAddressUnitBits());
}
