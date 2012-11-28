/* 
 *  	Created on: 18.4.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockgraphitem.cpp
 *		Project: Kactus 2
 */

#include "addressblockgraphitem.h"
#include "registergraphitem.h"
#include <models/register.h>
#include <common/utils.h>
#include "memorymapgraphitem.h"

#include <QBrush>
#include <QColor>

AddressBlockGraphItem::AddressBlockGraphItem( QSharedPointer<AddressBlock> addrBlock, 
											 QGraphicsItem *parent ):
MemoryVisualizationItem(parent),
addrBlock_(addrBlock) {

	Q_ASSERT(addrBlock_);
	setBrush(QBrush(QColor(80, 180, 255)));

	// Add this child to the parent
	MemoryMapGraphItem* parentGraphItem = static_cast<MemoryMapGraphItem*>(parent);
	Q_ASSERT(parentGraphItem);
}

AddressBlockGraphItem::~AddressBlockGraphItem() {
}

void AddressBlockGraphItem::refresh() {
	setName(addrBlock_->getName());
	setLeftTopCorner(addrBlock_->getBaseAddress());
	setLeftBottomCorner(addrBlock_->getLastAddressStr());

// 	childItems_.clear();
// 
// 	QList<QSharedPointer<RegisterModel> >& regItems = addrBlock_->getRegisterData();
// 	foreach (QSharedPointer<RegisterModel> regItem, regItems) {
// 
// 		QSharedPointer<Register> reg = regItem.dynamicCast<Register>();
// 		if (reg) {
// 
// 			// create the item
// 			RegisterGraphItem* regGraph = new RegisterGraphItem(reg, this);
// 
// 			// get the offset of the item
// 			int offset = regGraph->getOffset();
// 
// 			// make sure the items are in correct order for the offset
// 			childItems_.insert(offset, regGraph);
// 
// 			// tell child to check its children
// 			regGraph->refresh();
// 			regGraph->hide();
// 		}
// 	}

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();

	MemoryMapGraphItem* parentGraphItem = static_cast<MemoryMapGraphItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

quint64 AddressBlockGraphItem::getOffset() const {
	QString offset = addrBlock_->getBaseAddress();
	return Utils::str2Int(offset);
}

int AddressBlockGraphItem::getBitWidth() const {
	return 0;
}

unsigned int AddressBlockGraphItem::getAddressUnitSize() const {
	MemoryMapGraphItem* memMap = static_cast<MemoryMapGraphItem*>(parentItem());
	Q_ASSERT(memMap);
	return memMap->getAddressUnitSize();
}

quint64 AddressBlockGraphItem::getLastAddress() const {
	return addrBlock_->getLastAddress();
}
