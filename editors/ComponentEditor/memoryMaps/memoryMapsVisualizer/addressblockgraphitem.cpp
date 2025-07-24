//-----------------------------------------------------------------------------
// File: addressblockgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 18.4.2012
//
// Description:
// The graphical item that represents one address block.
//-----------------------------------------------------------------------------

#include "addressblockgraphitem.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <common/KactusColors.h>

#include <QStringBuilder>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::AddressBlockGraphItem()
//-----------------------------------------------------------------------------
AddressBlockGraphItem::AddressBlockGraphItem(QSharedPointer<AddressBlock> addrBlock,
    QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem *parent ):
MemoryBlockGraphItem(expressionParser, parent),
addrBlock_(addrBlock)
{
	Q_ASSERT(addrBlock_);
	
	setDefaultBrush(QBrush(KactusColors::ADDR_BLOCK_COLOR));
    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int AddressBlockGraphItem::getBitWidth() const
{
    return parseExpression(addrBlock_->getWidth());
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::isPresent()
//-----------------------------------------------------------------------------
bool AddressBlockGraphItem::isPresent() const
{
    return addrBlock_->getIsPresent().isEmpty() || parseExpression(addrBlock_->getIsPresent()) == 1;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void AddressBlockGraphItem::updateDisplay()
{
    QString formattedName = getReplicaName(getName());

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setName(formattedName);
    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);

    const int BIT_WIDTH = getBitWidth();
    setToolTip("<b>Name: </b>" % formattedName % "<br>" %
        "<b>Offset: </b>" % toHexString(offset, BIT_WIDTH) % "<br>" %
        "<b>Last address: </b>" % toHexString(lastAddress, BIT_WIDTH) % "<br>" %
        "<b>Size [AUB]: </b>" % QString::number(getRange()));
}

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::updateChildrenDisplay()
//-----------------------------------------------------------------------------
void AddressBlockGraphItem::updateChildrenDisplay() const
{
    for (auto const& child : childItems_)
    {
        child->updateDisplay();
    }
}

//-----------------------------------------------------------------------------
// Function: addressblockgraphitem::getName()
//-----------------------------------------------------------------------------
QString AddressBlockGraphItem::getName() const
{
    return addrBlock_->name();
}

//-----------------------------------------------------------------------------
// Function: addressblockgraphitem::getRange()
//-----------------------------------------------------------------------------
quint64 AddressBlockGraphItem::getRange() const
{
    return parseExpression(addrBlock_->getRange());
}
