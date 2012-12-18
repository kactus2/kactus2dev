/* 
 *  	Created on: 20.11.2012
 *      Author: Antti Kamppi
 * 		filename: memorygapitem.cpp
 *		Project: Kactus 2
 */

#include "memorygapitem.h"

#include <QColor>
#include <QBrush>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>
#include <common/KactusColors.h>

MemoryGapItem::MemoryGapItem(QGraphicsItem* parent):
MemoryVisualizationItem(parent),
start_(0),
end_(0) {
	QBrush brush(KactusColors::DRAFT_COMPONENT);
	setBrush(brush);
	ExpandableItem::setExpansionBrush(brush);
	setName("...");
	setLeftTopCorner("0x0");
	setShowExpandableItem(false);
}

MemoryGapItem::~MemoryGapItem() {
}

void MemoryGapItem::refresh() {
	setLeftTopCorner(start_);
	setLeftBottomCorner(end_);

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
	refresh();
}

void MemoryGapItem::setEndAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		end_ = address;
	}
	else {
		end_ = --address;
	}
	refresh();
}

quint64 MemoryGapItem::getLastAddress() const {
	return end_;
}

void MemoryGapItem::setWidth( qreal width ) {
	QGraphicsItem* parentGraph = parentItem();

	MemoryMapGraphItem* parentMap = dynamic_cast<MemoryMapGraphItem*>(parentGraph);
	// gaps within memory maps must be wider because they are indented
	if (parentMap) {
		setRect(0, 0, width + VisualizerItem::ITEM_HEIGHT, VisualizerItem::ITEM_HEIGHT);
	}
	else {
		setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);
	}

	// update the text fields
	VisualizerItem::reorganizeChildren();
}
