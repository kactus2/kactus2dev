/* 
 *  	Created on: 18.4.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockgraphitem.cpp
 *		Project: Kactus 2
 */

#include "addressblockgraphitem.h"
#include "registergraphitem.h"
#include <IPXACTmodels/register.h>
#include <common/utils.h>
#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::AddressBlockGraphItem()
//-----------------------------------------------------------------------------
AddressBlockGraphItem::AddressBlockGraphItem( QSharedPointer<AddressBlock> addrBlock, 
											 QGraphicsItem *parent ):
MemoryVisualizationItem(parent),
addrBlock_(addrBlock) {

	Q_ASSERT(addrBlock_);
	QBrush brush(KactusColors::ADDR_BLOCK_COLOR);
	setDefaultBrush(brush);

    setOverlappingTop(addrBlock_->getBaseAddress().toLongLong());
    setOverlappingBottom(addrBlock_->getLastAddress());
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::~AddressBlockGraphItem()
//-----------------------------------------------------------------------------
AddressBlockGraphItem::~AddressBlockGraphItem() {
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::refresh()
//-----------------------------------------------------------------------------
void AddressBlockGraphItem::refresh() {
    
    updateDisplay();

	MemoryVisualizationItem* parentGraphItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::refreshItem()
//-----------------------------------------------------------------------------
void AddressBlockGraphItem::refreshItem()
{
    updateDisplay();
    reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void AddressBlockGraphItem::updateDisplay()
{
    setName(addrBlock_->getName());
    setOverlappingTop(Utils::str2Uint(addrBlock_->getBaseAddress()));
    setOverlappingBottom(addrBlock_->getLastAddress());

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + addrBlock_->getName() + "<br>" +
        "<b>Offset: </b>" + AddressSpaceVisualizationItem::addr2Str(getOffset(), getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + AddressSpaceVisualizationItem::addr2Str(addrBlock_->getLastAddress(), 
        getBitWidth()) + "<br>" +
        "<b>Size [AUB]: </b>" + addrBlock_->getRange());
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 AddressBlockGraphItem::getOffset() const {
	QString offset = addrBlock_->getBaseAddress();
	return Utils::str2Uint(offset);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int AddressBlockGraphItem::getBitWidth() const {
	return addrBlock_->getWidth();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int AddressBlockGraphItem::getAddressUnitSize() const {
	MemoryVisualizationItem* memMap = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(memMap);
	return memMap->getAddressUnitSize();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 AddressBlockGraphItem::getLastAddress() const {
	return addrBlock_->getLastAddress();
}

