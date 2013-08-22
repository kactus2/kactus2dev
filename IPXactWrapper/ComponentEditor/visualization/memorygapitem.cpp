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

MemoryGapItem::MemoryGapItem(QGraphicsItem* parent):
MemoryVisualizationItem(parent),
start_(0),
end_(0) {
	setBrush(QBrush(QColor(Qt::white)));
	ExpandableItem::setExpansionBrush(QBrush(QColor(Qt::white)));
	setName("...");
	setLeftTopCorner("0x0");
	setShowExpandableItem(false);
    setToolTip("This memory block is unassigned.");
}

MemoryGapItem::~MemoryGapItem() {
}


void MemoryGapItem::refresh() {
	//setLeftTopCorner(start_);
	//setLeftBottomCorner(end_);

	ExpandableItem::reorganizeChildren();
}

quint64 MemoryGapItem::getOffset() const {
	return start_;
}

int MemoryGapItem::getBitWidth() const {
	MemoryVisualizationItem* memItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(memItem);
	return memItem->getBitWidth();
}

unsigned int MemoryGapItem::getAddressUnitSize() const {
	MemoryVisualizationItem* memItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(memItem);
	return memItem->getAddressUnitSize();
}

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

quint64 MemoryGapItem::getLastAddress() const {
	return end_;
}
