//-----------------------------------------------------------------------------
// File: fieldgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.4.2012
//
// Description:
// The graphical item that represents a bit field within a register.
//-----------------------------------------------------------------------------

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

    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);

	QBrush brush(KactusColors::FIELD_COLOR);
	setDefaultBrush(brush);

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
// Function: RegisterGraphItem::setConflicted()
//-----------------------------------------------------------------------------
void FieldGraphItem::setConflicted(bool conflicted)
{
    MemoryVisualizationItem::setConflicted(conflicted);
    if (conflicted)
    {
        setOpacity(0.5);
    }
    else
    {
        setOpacity(1);
    }
}
