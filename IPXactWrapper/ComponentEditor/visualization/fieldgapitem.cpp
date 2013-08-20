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
	setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_BOTTOM);

	// fields can never be expanded
	setExpansionRectVisible(false);
    
}

FieldGapItem::FieldGapItem( QString name, QGraphicsItem* parent ):
MemoryGapItem(parent) {

    // fields show name in the middle
    setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_BOTTOM);
    setName(name);

    // fields can never be expanded
    setExpansionRectVisible(false);
}

FieldGapItem::~FieldGapItem() {
}

void FieldGapItem::refresh() {
	//setRect(0, 0, VisualizerItem::MIN_WIDTH, VisualizerItem::ITEM_HEIGHT);

	QString startStr = QString::number(start_);
	VisualizerItem::setRightTopCorner(startStr);

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

void FieldGapItem::setStartAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		start_ = address;
	}
	else {
		start_ = ++address;
	}
    setOverlappingBottom(start_);
	refresh();
}

void FieldGapItem::setEndAddress( quint64 address, bool contains /*= true*/ ) {
	if (contains) {
		end_ = address;
	}
	else {
		end_ = --address;
	}
    setOverlappingTop(end_);
	refresh();
}

void FieldGapItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;
    setLeftTopCorner(QString::number(firstFreeAddress_));
}

void FieldGapItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;
    setRightTopCorner(QString::number(lastFreeAddress_));
}
