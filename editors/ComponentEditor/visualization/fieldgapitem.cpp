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
MemoryGapItem(parent)
{
    // fields show name in the middle
    setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_BOTTOM);
    setName(name);

    // fields can never be expanded
    setExpansionRectVisible(false);
}

//-----------------------------------------------------------------------------
// Function: ~FieldGapItem()
//-----------------------------------------------------------------------------
FieldGapItem::~FieldGapItem()
{
}

//-----------------------------------------------------------------------------
// Function: refresh()
//-----------------------------------------------------------------------------
void FieldGapItem::refresh()
{
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
int FieldGapItem::getBitWidth() const
{
	return end_ - start_ + 1;
}

//-----------------------------------------------------------------------------
// Function: itemTotalWidth()
//-----------------------------------------------------------------------------
qreal FieldGapItem::itemTotalWidth() const
{
	return rect().width();
}

//-----------------------------------------------------------------------------
// Function: setStartAddress()
//-----------------------------------------------------------------------------
void FieldGapItem::setStartAddress(quint64 address)
{
    start_ = address;
 
    setDisplayLastAddress(start_);
	refresh();
}

//-----------------------------------------------------------------------------
// Function: setEndAddress()
//-----------------------------------------------------------------------------
void FieldGapItem::setEndAddress(quint64 address)
{
    end_ = address;

    setDisplayOffset(end_);
	refresh();
}

//-----------------------------------------------------------------------------
// Function: FieldGapItem::setDisplayOffset()
//-----------------------------------------------------------------------------
void FieldGapItem::setDisplayOffset(quint64 const& address)
{
    firstFreeAddress_ = address;
    setLeftTopCorner(QString::number(firstFreeAddress_));
}


//-----------------------------------------------------------------------------
// Function: FieldGapItem::setDisplayLastAddress()
//-----------------------------------------------------------------------------
void FieldGapItem::setDisplayLastAddress(quint64 const& address)
{
    lastFreeAddress_ = address;
    setRightTopCorner(QString::number(lastFreeAddress_));
}
