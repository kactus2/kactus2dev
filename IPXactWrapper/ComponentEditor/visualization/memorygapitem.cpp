/* 
 *  	Created on: 20.11.2012
 *      Author: Antti Kamppi
 * 		filename: memorygapitem.cpp
 *		Project: Kactus 2
 */

#include "memorygapitem.h"

#include <QColor>
#include <QBrush>

#include <QDebug>

MemoryGapItem::MemoryGapItem(QGraphicsItem* parent):
MemoryVisualizationItem(parent),
start_(0),
end_(0) {
	setBrush(QBrush(QColor("grey")));
	setName("...");
	setLeftTopCorner("0x0");
	setShowExpandableItem(false);
}

MemoryGapItem::~MemoryGapItem() {
}

void MemoryGapItem::refresh() {
	QString startStr = QString::number(start_, 16);
	startStr.prepend("0x");
	setLeftTopCorner(startStr);

	QString endStr = QString::number(end_, 16);
	endStr.prepend("0x");
	setLeftBottomCorner(endStr);

	VisualizerItem::reorganizeChildren();
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
