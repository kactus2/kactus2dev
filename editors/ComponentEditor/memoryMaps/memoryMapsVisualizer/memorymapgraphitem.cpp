/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "memorymapgraphitem.h"
#include "addressblockgraphitem.h"

#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>

#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::MemoryMapGraphItem()
//-----------------------------------------------------------------------------
MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> parentMemoryMap,
    QSharedPointer<AbstractMemoryMap> memoryRemap, QGraphicsItem* parent /* = 0 */):
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
    setDisplayOffset(memoryMap_->getFirstAddress());
    setDisplayLastAddress(memoryMap_->getLastAddress());

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + memoryMap_->getName() + "<br>" +
        "<b>AUB: </b>" + QString::number(parentMemoryMap_->getAddressUnitBits()) + "<br>" +
        "<b>First address: </b>" + toHexString(getOffset()) + "<br>" +
        "<b>Last address: </b>" + toHexString(memoryMap_->getLastAddress()));
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getOffset() const 
{
	return memoryMap_->getFirstAddress();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int MemoryMapGraphItem::getBitWidth() const 
{
	return memoryMap_->getMaxWidth();
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
	return memoryMap_->getLastAddress();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::itemTotalWidth()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphItem::itemTotalWidth() const 
{
    return VisualizerItem::DEFAULT_WIDTH;
}
