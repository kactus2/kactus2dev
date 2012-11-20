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

#include <QBrush>
#include <QColor>

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

	QList<QSharedPointer<Field> >& fields = register_->getFields();
	foreach (QSharedPointer<Field> field, fields) {

		// create the item
		FieldGraphItem* fieldGraph = new FieldGraphItem(field, this);

		// get the offset of the item
		int offset = fieldGraph->getOffset();

		// make sure the items are in correct order for the offset
		childItems_.insert(offset, fieldGraph);

		// tell child to check its children
		fieldGraph->refresh();
		fieldGraph->hide();
	}

	// set the positions for the children
	reorganizeChildren();
}

int RegisterGraphItem::getOffset() const {
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

	for (QMap<int, MemoryVisualizationItem*>::iterator i = --childItems_.end();
		i != --childItems_.begin(); --i) {
			
			Q_ASSERT(i.value());
			//i.value()->setWidth(width);

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
