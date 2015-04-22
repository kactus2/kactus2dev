/* 
 *  	Created on: 25.4.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgraphitem.cpp
 *		Project: Kactus 2
 */

#include "fieldgraphitem.h"

#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::FieldGraphItem()
//-----------------------------------------------------------------------------
FieldGraphItem::FieldGraphItem( QSharedPointer<Field> field, QGraphicsItem* parent):
MemoryVisualizationItem(parent),
    field_(field)
{
	Q_ASSERT(field_);
	QBrush brush(KactusColors::FIELD_COLOR);
	setDefaultBrush(brush);

	// fields show name in the middle
	setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_BOTTOM);

	setShowExpandableItem(false);
	setExpansionRectVisible(false);

    refresh();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::~FieldGraphItem()
//-----------------------------------------------------------------------------
FieldGraphItem::~FieldGraphItem()
{	
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::refresh()
//-----------------------------------------------------------------------------
void FieldGraphItem::refresh()
{
    updateDisplay();

    ExpandableItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void FieldGraphItem::updateDisplay()
{
    setName(field_->getName());
    setLeftTopCorner(QString::number(field_->getMSB()));
    setRightTopCorner(QString::number(field_->getBitOffset()));

    setDisplayOffset(field_->getMSB());
    setDisplayLastAddress(field_->getBitOffset());
    setToolTip("<b>" + getName() + "</b> [" + QString::number(field_->getMSB()) + ".." + 
        QString::number(field_->getBitOffset()) + "]");
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 FieldGraphItem::getOffset() const
{
	return field_->getBitOffset();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int FieldGraphItem::getBitWidth() const
{
	return field_->getBitWidth();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int FieldGraphItem::getAddressUnitSize() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setWidth()
//-----------------------------------------------------------------------------
void FieldGraphItem::setWidth(qreal width)
{
    VisualizerItem::setWidth(width);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 FieldGraphItem::getLastAddress() const
{
	return field_->getMSB();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setDisplayOffset()
//-----------------------------------------------------------------------------
void FieldGraphItem::setDisplayOffset(quint64 const& address)
{
    firstFreeAddress_ = address;
    setLeftTopCorner(QString::number(firstFreeAddress_));
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setDisplayLastAddress()
//-----------------------------------------------------------------------------
void FieldGraphItem::setDisplayLastAddress(quint64 const& address)
{
    lastFreeAddress_ = address;
    setRightTopCorner(QString::number(lastFreeAddress_));
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setCompleteOverlap()
//-----------------------------------------------------------------------------
void FieldGraphItem::setCompleteOverlap()
{
    MemoryVisualizationItem::setCompleteOverlap();
    setWidth(0);
    ExpandableItem::reorganizeChildren();
}
