//-----------------------------------------------------------------------------
// File: segmentgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.12.2012
//
// Description:
// The graph item to visualize segments in address space editor.
//-----------------------------------------------------------------------------

#include "segmentgraphitem.h"

#include <common/KactusColors.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/Region.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::SegmentGraphItem()
//-----------------------------------------------------------------------------
SegmentGraphItem::SegmentGraphItem(QSharedPointer<Region> segment, QString const& addressSpaceWidth,
                                   QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent):
AddressSpaceVisualizationItem(addressSpaceWidth, expressionParser, parent),
segment_(segment)
{
	Q_ASSERT(segment_);

	QBrush brush(KactusColors::ADDRESS_SEGMENT);
	setDefaultBrush(brush);
    setLayoutType(LABELS_RIGHT);
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::refresh()
//-----------------------------------------------------------------------------
void SegmentGraphItem::refresh()
{
    setName(segment_->name());

    quint64 offset = getOffset();
    quint64 lastAddr = getLastAddress();
    setOverlappingTop(offset);
    setOverlappingBottom(lastAddr);

    setToolTip("<b>Name: </b>" + segment_->name() + "<br>" +
        "<b>Offset: </b>" + addr2Str(offset, getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + addr2Str(lastAddr, getBitWidth())  + "<br>" +
        "<b>Size [AUB]: </b>" + getExpressionParser()->parseExpression(segment_->getRange()));

    repositionLabels(); 
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 SegmentGraphItem::getOffset() const
{
	return getExpressionParser()->parseExpression(segment_->getAddressOffset()).toULongLong();
}

//-----------------------------------------------------------------------------
// Function: SegmentGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 SegmentGraphItem::getLastAddress() const
{
    quint64 base = getOffset();
    quint64 range = getExpressionParser()->parseExpression(segment_->getRange()).toULongLong();

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

    if (firstFreeAddress_ == lastFreeAddress_)
    {
        VisualizerItem::setBottomLabelText("");
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
        VisualizerItem::setBottomLabelText("");
    }
    else
    {
        setRightBottomCorner(lastFreeAddress_);
    }
}
