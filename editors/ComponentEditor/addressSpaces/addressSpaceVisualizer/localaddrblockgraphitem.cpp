//-----------------------------------------------------------------------------
// File: localaddrblockgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.01.2013
//
// Description:
// The graph item to visualize address blocks in local memory map.
//-----------------------------------------------------------------------------

#include "localaddrblockgraphitem.h"

#include <common/KactusColors.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/AddressBlock.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::LocalAddrBlockGraphItem()
//-----------------------------------------------------------------------------
LocalAddrBlockGraphItem::LocalAddrBlockGraphItem(QSharedPointer<AddressBlock> block,
                                                 QString const& addressSpaceWidth,
                                                 QSharedPointer<ExpressionParser> expressionParser,
												 QGraphicsItem* parent /*= 0*/ ):
AddressSpaceVisualizationItem(addressSpaceWidth, expressionParser, parent),
addrBlock_(block)
{
	Q_ASSERT(addrBlock_);

	QBrush brush(KactusColors::ADDR_BLOCK_COLOR);
	VisualizerItem::setDefaultBrush(brush);
}

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::refresh()
//-----------------------------------------------------------------------------
void LocalAddrBlockGraphItem::refresh()
{
	setName(addrBlock_->name());
    
    quint64 offset = getOffset();
    quint64 lastAddr = getLastAddress();
    setOverlappingTop(offset);    
    setOverlappingBottom(lastAddr);
	
    // Set tooltip to show addresses in hexadecimals.
    const int BIT_WIDTH = getBitWidth();
    setToolTip("<b>Name: </b>" + addrBlock_->name() + "<br>" +
        "<b>Base address: </b>" + toHexString(offset, BIT_WIDTH) + "<br>" +
        "<b>Last address: </b>" + toHexString(lastAddr, BIT_WIDTH) + "<br>" +
        "<b>Size [AUB]: </b>" + getExpressionParser()->parseExpression(addrBlock_->getRange()));

	repositionLabels();
}

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 LocalAddrBlockGraphItem::getOffset() const
{
	return getExpressionParser()->parseExpression(addrBlock_->getBaseAddress()).toULongLong();
}

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 LocalAddrBlockGraphItem::getLastAddress() const 
{
    quint64 base = getOffset();
    quint64 range = getExpressionParser()->parseExpression(addrBlock_->getRange()).toULongLong();

    quint64 lastAddr = base + range;

    if (lastAddr == 0) 
    {
        return 0;
    }

    return lastAddr -1;
}
