/* 
 *  	Created on: 20.11.2012
 *      Author: Antti Kamppi
 * 		filename: memorygapitem.cpp
 *		Project: Kactus 2
 */

#include "memorygapitem.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>
#include <common/KactusColors.h>

#include <QColor>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: MemoryGapItem()
//-----------------------------------------------------------------------------
MemoryGapItem::MemoryGapItem(QGraphicsItem* parent):
MemoryVisualizationItem(parent),
start_(0),
end_(0) {
	setBrush(QBrush(QColor(Qt::white)));
	ExpandableItem::setExpansionBrush(QBrush(QColor(Qt::white)));
	setName("Reserved");
	setLeftTopCorner("0x0");
	setShowExpandableItem(false);
    setToolTip("This memory block is reserved for future use.");
}

//-----------------------------------------------------------------------------
// Function: ~MemoryGapItem()
//-----------------------------------------------------------------------------
MemoryGapItem::~MemoryGapItem() {
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void MemoryGapItem::refresh() 
{
	ExpandableItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: getOffset()
//-----------------------------------------------------------------------------
quint64 MemoryGapItem::getOffset() const {
	return start_;
}

//-----------------------------------------------------------------------------
// Function: getBitWidth()
//-----------------------------------------------------------------------------
int MemoryGapItem::getBitWidth() const {
	MemoryVisualizationItem* memItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(memItem);
	return memItem->getBitWidth();
}

//-----------------------------------------------------------------------------
// Function: getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int MemoryGapItem::getAddressUnitSize() const {
	MemoryVisualizationItem* memItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(memItem);
	return memItem->getAddressUnitSize();
}

//-----------------------------------------------------------------------------
// Function: setStartAddress()
//-----------------------------------------------------------------------------
void MemoryGapItem::setStartAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		start_ = address;
	}
	else {
		start_ = ++address;
	}
    setOverlappingTop(start_);
	refresh();
}

//-----------------------------------------------------------------------------
// Function: setEndAddress()
//-----------------------------------------------------------------------------
void MemoryGapItem::setEndAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		end_ = address;
	}
	else {
		end_ = --address;
	}
    setOverlappingBottom(end_);
	refresh();
}

//-----------------------------------------------------------------------------
// Function: getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryGapItem::getLastAddress() const {
	return end_;
}
