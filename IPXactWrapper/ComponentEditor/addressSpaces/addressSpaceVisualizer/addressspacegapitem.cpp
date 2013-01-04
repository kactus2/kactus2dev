/* 
 *  	Created on: 4.1.2013
 *      Author: Antti Kamppi
 * 		filename: addressspacegapitem.cpp
 *		Project: Kactus 2
 */

#include "addressspacegapitem.h"
#include <common/KactusColors.h>

#include <QColor>
#include <QBrush>

AddressSpaceGapItem::AddressSpaceGapItem(QSharedPointer<AddressSpace> addrSpace,
										 QGraphicsItem* parent):
AddressSpaceVisualizationItem(addrSpace, parent),
start_(0),
end_(0) {

	QBrush brush(KactusColors::DRAFT_COMPONENT);
	setBrush(brush);
	setNamePosition(VisualizerItem::NAME_CENTERED);
	setName("...");
}

AddressSpaceGapItem::~AddressSpaceGapItem() {
}

void AddressSpaceGapItem::refresh() {
	setRightTopCorner(start_);
	setRightBottomCorner(end_);
	VisualizerItem::reorganizeChildren();
}

quint64 AddressSpaceGapItem::getOffset() const {
	return start_;
}

quint64 AddressSpaceGapItem::getLastAddress() const {
	return end_;
}

void AddressSpaceGapItem::setStartAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		start_ = address;
	}
	else {
		start_ = ++address;
	}
	refresh();
}

void AddressSpaceGapItem::setEndAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		end_ = address;
	}
	else {
		end_ = --address;
	}
	refresh();
}
