/* 
 *  	Created on: 18.4.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockgraphitem.cpp
 *		Project: Kactus 2
 */

#include "addressblockgraphitem.h"
#include <models/register.h>

#include <QBrush>
#include <QColor>
#include <QPointF>

#include <QDebug>

AddressBlockGraphItem::AddressBlockGraphItem( QSharedPointer<AddressBlock> addrBlock, 
											 QGraphicsItem *parent ):
MemoryVisualizerItem(parent),
addrBlock_(addrBlock),
registers_() {

	Q_ASSERT(addrBlock_);

	setName(addrBlock_->getName());
	maxWidth_ = addrBlock_->getWidth();

	setLeftCornerText(addrBlock_->getBaseAddress());

	setBrush(QBrush(QColor(80, 180, 255)));

	QList<QSharedPointer<RegisterModel> >& regElements = addrBlock_->getRegisterData();
	foreach (QSharedPointer<RegisterModel> regModel, regElements) {

		QSharedPointer<Register> reg = regModel.dynamicCast<Register>();

		if (reg) {
			QSharedPointer<RegisterGraphItem> regItem(new RegisterGraphItem(reg, this, maxWidth_));
			registers_.append(regItem);
		}
	}
	reorganizeChildren();
}

AddressBlockGraphItem::~AddressBlockGraphItem() {
}

void AddressBlockGraphItem::reorganizeChildren() {

	qreal width = MemoryVisualizerItem::itemTotalWidth();

	QSharedPointer<RegisterGraphItem> previous;
	foreach (QSharedPointer<RegisterGraphItem> regItem, registers_) {

		regItem->setWidth(width);

		if (previous) {
			regItem->setPos(0, mapRectFromItem(previous.data(), previous->itemTotalRect()).bottom());
		}
		else {
			regItem->setPos(0, rect().bottom());
		}
		previous = regItem;
	}
}

void AddressBlockGraphItem::setWidth( qreal width ) {
	
	// set the width for this item
	MemoryVisualizerItem::setWidth(width);

	// set width for child items
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		// set the width for all children that are memory visualization objects
		MemoryVisualizerItem* childMemItem = dynamic_cast<MemoryVisualizerItem*>(child);
		if (childMemItem) {
			childMemItem->setWidth(width);
		}
	}
}

int AddressBlockGraphItem::getOffset() const {
	return addrBlock_->getBaseAddress().toInt();
}

int AddressBlockGraphItem::getWidth() const {
	return addrBlock_->getWidth();
}
