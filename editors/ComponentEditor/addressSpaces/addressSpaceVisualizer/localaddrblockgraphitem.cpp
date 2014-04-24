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
	setDefaultBrush(brush);
	setNamePosition(VisualizerItem::NAME_LEFT_ALIGN, VisualizerItem::NAME_MIDDLE);
}

LocalAddrBlockGraphItem::~LocalAddrBlockGraphItem() {
}

void LocalAddrBlockGraphItem::refresh() {
	setName(addrBlock_->getName());
    
    quint64 offset = Utils::str2Uint(addrBlock_->getBaseAddress());
    quint64 lastAddr = addrBlock_->getLastAddress();
    setOverlappingTop(offset);    
    setOverlappingBottom(lastAddr);
	
    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + addrBlock_->getName() + "<br>" +
        "<b>Base address: </b>" + addr2Str(getOffset(), getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + addr2Str(addrBlock_->getLastAddress(), getBitWidth()) + "<br>" +
        "<b>Size [AUB]: </b>" + addrBlock_->getRange());

	VisualizerItem::reorganizeChildren();
}

quint64 LocalAddrBlockGraphItem::getOffset() const {
	QString offset = addrBlock_->getBaseAddress();
	return Utils::str2Uint(offset);
}

quint64 LocalAddrBlockGraphItem::getLastAddress() const {
	return addrBlock_->getLastAddress();
}
