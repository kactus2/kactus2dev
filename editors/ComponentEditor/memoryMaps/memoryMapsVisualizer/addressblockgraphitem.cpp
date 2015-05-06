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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <common/KactusColors.h>
#include <common/utils.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::AddressBlockGraphItem()
//-----------------------------------------------------------------------------
AddressBlockGraphItem::AddressBlockGraphItem(QSharedPointer<AddressBlock> addrBlock, 
    QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem *parent ):
MemoryVisualizationItem(parent),
addrBlock_(addrBlock),
addrssableUnitBits_(0),
expressionParser_(expressionParser)
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

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + addrBlock_->getName() + "<br>" +
        "<b>Offset: </b>" + toHexString(offset) + "<br>" +
        "<b>Last address: </b>" + toHexString(lastAddress) + "<br>" +
        "<b>Size [AUB]: </b>" + expressionParser_->parseExpression(addrBlock_->getRange()));
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 AddressBlockGraphItem::getOffset() const
{
	return expressionParser_->parseExpression(addrBlock_->getBaseAddress()).toUInt();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int AddressBlockGraphItem::getBitWidth() const
{
	return expressionParser_->parseExpression(addrBlock_->getWidthExpression()).toInt();
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
    quint64 base = getOffset();
    quint64 range = expressionParser_->parseExpression(addrBlock_->getRange()).toUInt();

    quint64 lastAddr = base + range;

    if (lastAddr == 0) 
    {
        return 0;
    }
    return lastAddr -1;
}
