/* 
 *  	Created on: 14.1.2013
 *      Author: Antti Kamppi
 * 		filename: localaddrblockgraphitem.cpp
 *		Project: Kactus 2
 */

#include "localaddrblockgraphitem.h"
#include <common/utils.h>
#include <common/KactusColors.h>

#include <QBrush>

LocalAddrBlockGraphItem::LocalAddrBlockGraphItem(QSharedPointer<AddressSpace> addrSpace,
												 QSharedPointer<AddressBlock> block,
												 QGraphicsItem* parent /*= 0*/ ):
AddressSpaceVisualizationItem(addrSpace, parent),
addrBlock_(block) {

	Q_ASSERT(addrBlock_);

	QBrush brush(KactusColors::ADDR_BLOCK_COLOR);
	setBrush(brush);
	setNamePosition(VisualizerItem::NAME_INDENTED);
}

LocalAddrBlockGraphItem::~LocalAddrBlockGraphItem() {
}

void LocalAddrBlockGraphItem::refresh() {
	setName(addrBlock_->getName());
	setLeftTopCorner(Utils::str2Uint(addrBlock_->getBaseAddress()));
	setLeftBottomCorner(addrBlock_->getLastAddress());
	VisualizerItem::reorganizeChildren();
}

quint64 LocalAddrBlockGraphItem::getOffset() const {
	QString offset = addrBlock_->getBaseAddress();
	return Utils::str2Uint(offset);
}

quint64 LocalAddrBlockGraphItem::getLastAddress() const {
	return addrBlock_->getLastAddress();
}
