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

#include "memorymapgraphitem.h"
#include "addressblockgraphitem.h"

#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>

#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::MemoryMapGraphItem()
//-----------------------------------------------------------------------------
MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> parentMemoryMap,    
    QSharedPointer<AbstractMemoryMap> memoryRemap,
    QGraphicsItem* parent /* = 0 */):
MemoryVisualizationItem(parent),
memoryMap_(memoryRemap),
parentMemoryMap_(parentMemoryMap)
{
    Q_ASSERT(memoryMap_);
    QBrush brush(KactusColors::MEM_MAP_COLOR);
    setDefaultBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::~MemoryMapGraphItem()
//-----------------------------------------------------------------------------
MemoryMapGraphItem::~MemoryMapGraphItem() 
{
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::refresh()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::refresh() 
{
    updateDisplay();
    reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::updateDisplay()
{
    setName(memoryMap_->getName());

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + memoryMap_->getName() + "<br>" +
        "<b>AUB: </b>" + QString::number(parentMemoryMap_->getAddressUnitBits()) + "<br>" +
        "<b>First address: </b>" + toHexString(offset) + "<br>" +
        "<b>Last address: </b>" + toHexString(lastAddress));
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getOffset() const 
{
    if (childItems_.isEmpty())
    {
        return 0;
    }
    else
    {
        return childItems_.first()->getOffset();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int MemoryMapGraphItem::getBitWidth() const 
{
    int width = 0;
    foreach (MemoryVisualizationItem* child, childItems_.values())
    {
        width = qMax(child->getBitWidth(), width);  
    }
    return width;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int MemoryMapGraphItem::getAddressUnitSize() const 
{
    return parentMemoryMap_->getAddressUnitBits();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getLastAddress() const 
{
    if (childItems_.isEmpty())
    {
        return 0;
    }
    else
    {
        return childItems_.last()->getLastAddress();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::itemTotalWidth()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphItem::itemTotalWidth() const 
{
    return VisualizerItem::DEFAULT_WIDTH;
}
