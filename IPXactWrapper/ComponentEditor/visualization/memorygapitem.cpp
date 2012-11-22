/* 
 *  	Created on: 20.11.2012
 *      Author: Antti Kamppi
 * 		filename: memorygapitem.cpp
 *		Project: Kactus 2
 */

#include "memorygapitem.h"

#include <QColor>
#include <QBrush>

MemoryGapItem::MemoryGapItem(QGraphicsItem* parent):
MemoryVisualizationItem(parent) {
	setBrush(QBrush(QColor(60, 160, 255)));
	setName("...");
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
}

int MemoryGapItem::getOffset() const {
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
