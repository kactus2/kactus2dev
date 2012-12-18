/* 
 *  	Created on: 30.11.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgapitem.cpp
 *		Project: Kactus 2
 */

#include "fieldgapitem.h"

FieldGapItem::FieldGapItem( QGraphicsItem* parent ):
MemoryGapItem(parent) {

	// fields show name in the middle
	setNamePosition(VisualizerItem::NAME_CENTERED);

	// fields can never be expanded
	setExpansionRectVisible(false);
}

FieldGapItem::~FieldGapItem() {
}

void FieldGapItem::refresh() {
	setRect(0, 0, VisualizerItem::MIN_WIDTH, VisualizerItem::ITEM_HEIGHT);

	QString startStr = QString::number(start_);
	setRightTopCorner(startStr);

	QString endStr = QString::number(end_);
	setLeftTopCorner(endStr);

	VisualizerItem::reorganizeChildren();
}

int FieldGapItem::getBitWidth() const {
	return end_ - start_ + 1;
}

qreal FieldGapItem::itemTotalWidth() const {
	return rect().width();
}
