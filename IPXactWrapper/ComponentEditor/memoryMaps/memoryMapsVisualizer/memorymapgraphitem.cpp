/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "memorymapgraphitem.h"

#include <models/memorymapitem.h>
#include <models/addressblock.h>

#include <QBrush>
#include <QColor>
#include <QPointF>

#include <QDebug>

MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> memoryMap,
									   QGraphicsItem* parent):
MemoryVisualizerItem(parent),
memoryMap_(memoryMap),
adBlocks_() {

	Q_ASSERT(memoryMap_);

	setName(memoryMap_->getName());

	setBrush(QBrush(QColor(100, 200, 255)));

	// find all sub-items of the memory map
	QList<QSharedPointer<MemoryMapItem> >* memItems = memoryMap_->getItemsPointer();
	foreach (QSharedPointer<MemoryMapItem> item, *memItems) {

		// if the sub item is an address block then create a child graph item for it
		QSharedPointer<AddressBlock> addrBlock = item.dynamicCast<AddressBlock>();
		if (addrBlock) {

			QSharedPointer<AddressBlockGraphItem> adBlock(new AddressBlockGraphItem(addrBlock, this));
			adBlocks_.append(adBlock);
		}
	}

	// set the positions for the sub items
	reorganizeChildren();
}

MemoryMapGraphItem::~MemoryMapGraphItem() {
}

void MemoryMapGraphItem::reorganizeChildren() {

	qreal width = MemoryVisualizerItem::itemTotalWidth();

	QSharedPointer<AddressBlockGraphItem> previous;
	foreach (QSharedPointer<AddressBlockGraphItem> addrBlock, adBlocks_) {

		addrBlock->setWidth(width);

		if (previous) {
			addrBlock->setPos(0, mapRectFromItem(previous.data(), previous->itemTotalRect()).bottom());
		}
		else {
			addrBlock->setPos(0, rect().bottom());
		}

		previous = addrBlock;
	}

	setRect(0, 0, width, MemoryVisualizerItem::ITEM_HEIGHT);
	MemoryVisualizerItem::reorganizeChildren();
}

