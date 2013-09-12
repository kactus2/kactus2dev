/* 
 *  	Created on: 20.12.2012
 *      Author: Antti Kamppi
 * 		filename: segmentgraphitem.cpp
 *		Project: Kactus 2
 */

#include "segmentgraphitem.h"
#include <common/utils.h>
#include <common/KactusColors.h>

#include <QBrush>

SegmentGraphItem::SegmentGraphItem(QSharedPointer<AddressSpace> addrSpace,
								   QSharedPointer<Segment> segment,
								   QGraphicsItem* parent):
AddressSpaceVisualizationItem(addrSpace, parent),
segment_(segment) {

	Q_ASSERT(segment_);

	QBrush brush(KactusColors::ADDRESS_SEGMENT);
	setDefaultBrush(brush);
	setNamePosition(VisualizerItem::NAME_RIGHT_ALIGN, VisualizerItem::NAME_MIDDLE);
}

SegmentGraphItem::~SegmentGraphItem() {
}

void SegmentGraphItem::refresh() {
    setName(segment_->getName());

    quint64 offset = Utils::str2Uint(segment_->getAddressOffset());
    quint64 lastAddr = segment_->getLastAddress();
    setOverlappingTop(offset);
    setOverlappingBottom(lastAddr);

    setToolTip("<b>Name: </b>" + segment_->getName() + "<br>" +
        "<b>Offset: </b>" + addr2Str(getOffset(), getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + addr2Str(getLastAddress(), getBitWidth())  + "<br>" +
        "<b>Size [AUB]: </b>" + segment_->getRange());

    VisualizerItem::reorganizeChildren();
}

quint64 SegmentGraphItem::getOffset() const {
	QString offset = segment_->getAddressOffset();
	return Utils::str2Uint(offset);
}

quint64 SegmentGraphItem::getLastAddress() const {
	return segment_->getLastAddress();
}

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