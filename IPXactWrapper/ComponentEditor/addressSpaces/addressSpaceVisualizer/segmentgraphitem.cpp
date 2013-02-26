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
	setBrush(brush);
	setNamePosition(VisualizerItem::NAME_CENTERED);
}

SegmentGraphItem::~SegmentGraphItem() {
}

void SegmentGraphItem::refresh() {
	setName(segment_->getName());
	setRightTopCorner(Utils::str2Uint(segment_->getAddressOffset()));
	setRightBottomCorner(segment_->getLastAddress());
	VisualizerItem::reorganizeChildren();
}

quint64 SegmentGraphItem::getOffset() const {
	QString offset = segment_->getAddressOffset();
	return Utils::str2Uint(offset);
}

quint64 SegmentGraphItem::getLastAddress() const {
	return segment_->getLastAddress();
}
