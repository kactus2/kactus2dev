//-----------------------------------------------------------------------------
// File: addressblockgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 18.4.2012
//
// Description:
// The graphical item that represents one address block.
//-----------------------------------------------------------------------------

#include "addressblockgraphitem.h"

#include <common/KactusColors.h>
#include <common/utils.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::AddressBlockGraphItem()
//-----------------------------------------------------------------------------
AddressBlockGraphItem::AddressBlockGraphItem(QSharedPointer<AddressBlock> addrBlock, 
    QGraphicsItem *parent ):
MemoryVisualizationItem(parent),
addrBlock_(addrBlock),
addrssableUnitBits_(0)
{
	Q_ASSERT(addrBlock_);
	QBrush brush(KactusColors::ADDR_BLOCK_COLOR);
	setDefaultBrush(brush);

    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::~AddressBlockGraphItem()
//-----------------------------------------------------------------------------
AddressBlockGraphItem::~AddressBlockGraphItem()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::refresh()
//-----------------------------------------------------------------------------
void AddressBlockGraphItem::refresh() 
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
    setDisplayOffset(Utils::str2Uint(addrBlock_->getBaseAddress()));
    setDisplayLastAddress(addrBlock_->getLastAddress());

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + addrBlock_->getName() + "<br>" +
        "<b>Offset: </b>" + toHexString(getOffset()) + "<br>" +
        "<b>Last address: </b>" + toHexString(addrBlock_->getLastAddress()) + "<br>" +
        "<b>Size [AUB]: </b>" + addrBlock_->getRange());
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 AddressBlockGraphItem::getOffset() const
{
	QString offset = addrBlock_->getBaseAddress();
	return Utils::str2Uint(offset);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int AddressBlockGraphItem::getBitWidth() const
{
	return addrBlock_->getWidth();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::setAddressableUnitBits()
//-----------------------------------------------------------------------------
void AddressBlockGraphItem::setAddressableUnitBits(int addressableUnitBits)
{
    addrssableUnitBits_ = addressableUnitBits;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int AddressBlockGraphItem::getAddressUnitSize() const
{
    return addrssableUnitBits_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 AddressBlockGraphItem::getLastAddress() const
{
	return addrBlock_->getLastAddress();
}
