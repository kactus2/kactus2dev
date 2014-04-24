/* 
 *  	Created on: 30.11.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgapitem.cpp
 *		Project: Kactus 2
 */

#include "fieldgapitem.h"

//-----------------------------------------------------------------------------
// Function: FieldGapItem()
//-----------------------------------------------------------------------------
FieldGapItem::FieldGapItem( QGraphicsItem* parent ):
MemoryGapItem(parent) {

	// fields show name in the middle
	setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_BOTTOM);

	// fields can never be expanded
	setExpansionRectVisible(false);
    
}

//-----------------------------------------------------------------------------
// Function: FieldGapItem()
//-----------------------------------------------------------------------------
FieldGapItem::FieldGapItem( QString name, QGraphicsItem* parent ):
MemoryGapItem(parent) {

    // fields show name in the middle
    setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_BOTTOM);
    setName(name);

    // fields can never be expanded
    setExpansionRectVisible(false);
}

//-----------------------------------------------------------------------------
// Function: ~FieldGapItem()
//-----------------------------------------------------------------------------
FieldGapItem::~FieldGapItem() {
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void FieldGapItem::refresh() {
	//setRect(0, 0, VisualizerItem::MIN_WIDTH, VisualizerItem::DEFAULT_HEIGHT);

	QString startStr = QString::number(start_);
	VisualizerItem::setRightTopCorner(startStr);

	QString endStr = QString::number(end_);
	setLeftTopCorner(endStr);
    setToolTip("<b>" + getName() + "</b> [" + endStr + ".." + startStr + "]");
	VisualizerItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: getBitWidth()
//-----------------------------------------------------------------------------
int FieldGapItem::getBitWidth() const {
	return end_ - start_ + 1;
}

//-----------------------------------------------------------------------------
// Function: itemTotalWidth()
//-----------------------------------------------------------------------------
qreal FieldGapItem::itemTotalWidth() const {
	return rect().width();
}

//-----------------------------------------------------------------------------
// Function: setStartAddress()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: setEndAddress()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: setOverlappingTop()
//-----------------------------------------------------------------------------
void FieldGapItem::setOverlappingTop(quint64 const& address)
{
    firstFreeAddress_ = address;
    setLeftTopCorner(QString::number(firstFreeAddress_));
}


//-----------------------------------------------------------------------------
// Function: setOverlappingBottom()
//-----------------------------------------------------------------------------
void FieldGapItem::setOverlappingBottom(quint64 const& address)
{
    lastFreeAddress_ = address;
    setRightTopCorner(QString::number(lastFreeAddress_));
}
