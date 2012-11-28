/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.cpp
 *		Project: Kactus 2
 */

#include "registergraphitem.h"
#include "fieldgraphitem.h"
#include <models/field.h>
#include <common/utils.h>
#include "addressblockgraphitem.h"
#include <IPXactWrapper/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <QBrush>
#include <QColor>

#include <QDebug>

RegisterGraphItem::RegisterGraphItem(QSharedPointer<Register> reg,
									 QGraphicsItem* parent):
MemoryVisualizationItem(parent),
register_(reg) {

	Q_ASSERT(register_);
	setBrush(QBrush(QColor(60, 160, 255)));
}

RegisterGraphItem::~RegisterGraphItem() {
}

void RegisterGraphItem::refresh() {
	setName(register_->getName());
	
	// get the base of the parent address block
	int base = 0;
	MemoryVisualizationItem* addrBlock = dynamic_cast<MemoryVisualizationItem*>(parentItem());
	if (addrBlock) {
		base = addrBlock->getOffset();
	}

	// get the offset of the register
	quint64 offset = Utils::str2Int(register_->getAddressOffset());
	// calculate the start address of the register
	quint64 startAddress = base + offset;

	// show the start address of the register
	QString startStr = QString::number(startAddress, 16);
	startStr.prepend("0x");
	setLeftTopCorner(startStr);

	// show the end address of the register
	unsigned int addrUnits = register_->getSize() / getAddressUnitSize();
	quint64 endAddress = base + offset + addrUnits -1;
	QString endStr = QString::number(endAddress, 16);
	endStr.prepend("0x");
	setLeftBottomCorner(endStr);

	// set the positions for the children
	reorganizeChildren();

	MemoryVisualizationItem* parentGraphItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

quint64 RegisterGraphItem::getOffset() const {
	Q_ASSERT(register_);
	QString offset = register_->getAddressOffset();
	return Utils::str2Int(offset);
}

int RegisterGraphItem::getBitWidth() const {
	Q_ASSERT(register_);
	return register_->getWidth();
}

void RegisterGraphItem::reorganizeChildren() {
	// first find out the width for all items
	qreal width = VisualizerItem::itemTotalWidth();

	updateChildMap();

	// if there are no children then this can not be expanded or collapsed
	if (childItems_.isEmpty()) {
		ExpandableItem::setShowExpandableItem(false);
	}
	// if there are children then display the expand/collapse item
	else {
		ExpandableItem::setShowExpandableItem(true);
	}

	qreal xCoordinate = rect().left();
	MemoryVisualizationItem* previous = NULL;

	for (QMap<quint64, MemoryVisualizationItem*>::iterator i = --childItems_.end();
		i != --childItems_.begin(); --i) {
			
			Q_ASSERT(i.value());

			if (previous) {

				QRectF previousRect = mapRectFromItem(previous, previous->itemTotalRect());
				xCoordinate = previousRect.right();
			}

			i.value()->setPos(xCoordinate, rect().bottom());
			previous = i.value();
	}

	// update the width of this item to match the width of all
	setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);

	// reorganize the text blocks of this item
	VisualizerItem::reorganizeChildren();
}

unsigned int RegisterGraphItem::getAddressUnitSize() const {
	AddressBlockGraphItem* addrBlock = static_cast<AddressBlockGraphItem*>(parentItem());
	Q_ASSERT(addrBlock);
	return addrBlock->getAddressUnitSize();
}

quint64 RegisterGraphItem::getLastAddress() const {
	AddressBlockGraphItem* addrBlock = static_cast<AddressBlockGraphItem*>(parentItem());
	Q_ASSERT(addrBlock);

	// how many bits one address unit takes
	unsigned int addrUnit = addrBlock->getAddressUnitSize();

	// the offset of the start of register
	quint64 offset = getOffset();

	// how many address unit are contained in the register
	unsigned int size = register_->getWidth() / addrUnit;

	// if size is not defined then never return a negative number
	if (size == 0) {
		return 0;
	}

	// the last address contained in the register
	return offset + size - 1;
}
