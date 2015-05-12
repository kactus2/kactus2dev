/* 
 *  	Created on: 20.12.2012
 *      Author: Antti Kamppi
 * 		filename: segmentgraphitem.cpp
 *		Project: Kactus 2
 */

#include "segmentgraphitem.h"

#include <common/utils.h>
#include <common/KactusColors.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::SegmentGraphItem()
//-----------------------------------------------------------------------------
SegmentGraphItem::SegmentGraphItem(QSharedPointer<Segment> segment,
                                   QString const& addressSpaceWidth,
                                   QSharedPointer<ExpressionParser> expressionParser,
								   QGraphicsItem* parent):
AddressSpaceVisualizationItem(addressSpaceWidth, expressionParser, parent),
segment_(segment)
{
	Q_ASSERT(segment_);

	QBrush brush(KactusColors::ADDRESS_SEGMENT);
	setDefaultBrush(brush);
	setNamePosition(VisualizerItem::NAME_RIGHT_ALIGN, VisualizerItem::NAME_MIDDLE);
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::~SegmentGraphItem()
//-----------------------------------------------------------------------------
SegmentGraphItem::~SegmentGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::refresh()
//-----------------------------------------------------------------------------
void SegmentGraphItem::refresh()
{
    setName(segment_->getName());

    quint64 offset = getOffset();
    quint64 lastAddr = getLastAddress();
    setOverlappingTop(offset);
    setOverlappingBottom(lastAddr);

    setToolTip("<b>Name: </b>" + segment_->getName() + "<br>" +
        "<b>Offset: </b>" + addr2Str(offset, getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + addr2Str(lastAddr, getBitWidth())  + "<br>" +
        "<b>Size [AUB]: </b>" + getExpressionParser()->parseExpression(segment_->getRange()));

    VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 SegmentGraphItem::getOffset() const
{
	return getExpressionParser()->parseExpression(segment_->getAddressOffset()).toUInt();
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 SegmentGraphItem::getLastAddress() const
{
    quint64 base = getOffset();
    quint64 range = getExpressionParser()->parseExpression(segment_->getRange()).toUInt();

    quint64 lastAddr = base + range;

    // if the base and range are undefined then return 0
    if (lastAddr == 0)
    {
        return 0;
    }

    // if they are defined then return actual last address
    if (range != 0)
    {
        return lastAddr - 1;
    }
    else
    {
        return lastAddr;
    }
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::setOverlappingTop()
//-----------------------------------------------------------------------------
void SegmentGraphItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;
    setRightTopCorner(firstFreeAddress_);

    if (firstFreeAddress_ == lastFreeAddress_){        
        VisualizerItem::setRightBottomCorner("");
    }
    else
    {
        setRightBottomCorner(lastFreeAddress_);
    }
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::setOverlappingBottom()
//-----------------------------------------------------------------------------
void SegmentGraphItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;

    if (firstFreeAddress_ == lastFreeAddress_)
    {
        VisualizerItem::setRightBottomCorner("");
    }
    else
    {
        setRightBottomCorner(lastFreeAddress_);
    }
}