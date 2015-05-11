//-----------------------------------------------------------------------------
// File: localaddrblockgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.01.2013
//
// Description:
// The graph item to visualize address blocks in local memory map.
//-----------------------------------------------------------------------------

#include "localaddrblockgraphitem.h"
#include <common/utils.h>
#include <common/KactusColors.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::LocalAddrBlockGraphItem()
//-----------------------------------------------------------------------------
LocalAddrBlockGraphItem::LocalAddrBlockGraphItem(QSharedPointer<AddressBlock> block,
                                                 QString const& addressSpaceWidth,
                                                 QSharedPointer<ExpressionParser> expressionParser,
												 QGraphicsItem* parent /*= 0*/ ):
AddressSpaceVisualizationItem(addressSpaceWidth, expressionParser, parent),
addrBlock_(block),
expressionParser_(expressionParser)
{
	Q_ASSERT(addrBlock_);

	QBrush brush(KactusColors::ADDR_BLOCK_COLOR);
	setDefaultBrush(brush);
	setNamePosition(VisualizerItem::NAME_LEFT_ALIGN, VisualizerItem::NAME_MIDDLE);
}

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::~LocalAddrBlockGraphItem()
//-----------------------------------------------------------------------------
LocalAddrBlockGraphItem::~LocalAddrBlockGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::refresh()
//-----------------------------------------------------------------------------
void LocalAddrBlockGraphItem::refresh()
{
	setName(addrBlock_->getName());
    
    quint64 offset = getOffset();
    quint64 lastAddr = getLastAddress();
    setOverlappingTop(offset);    
    setOverlappingBottom(lastAddr);
	
    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + addrBlock_->getName() + "<br>" +
        "<b>Base address: </b>" + addr2Str(offset, getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + addr2Str(lastAddr, getBitWidth()) + "<br>" +
        "<b>Size [AUB]: </b>" + addrBlock_->getRange());

	VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 LocalAddrBlockGraphItem::getOffset() const
{
	return expressionParser_->parseExpression(addrBlock_->getBaseAddress()).toUInt();
}

//-----------------------------------------------------------------------------
// Function: LocalAddrBlockGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 LocalAddrBlockGraphItem::getLastAddress() const 
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
