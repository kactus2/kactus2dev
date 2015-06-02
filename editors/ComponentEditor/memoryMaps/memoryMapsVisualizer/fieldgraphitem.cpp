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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::FieldGraphItem()
//-----------------------------------------------------------------------------
FieldGraphItem::FieldGraphItem( QSharedPointer<Field> field, QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent):
MemoryVisualizationItem(parent),
    field_(field),
    expressionParser_(expressionParser)
{
	Q_ASSERT(field_);

    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);

	QBrush brush(KactusColors::FIELD_COLOR);
	setDefaultBrush(brush);

	setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_BOTTOM);

	setShowExpandableItem(false);
	setExpansionRectVisible(false);

    updateDisplay();
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

    quint64 leftBound = getLastAddress();
    quint64 rightBound = getOffset();

    setLeftTopCorner(QString::number(leftBound));
    setRightTopCorner(QString::number(rightBound));

    setDisplayOffset(leftBound);
    setDisplayLastAddress(rightBound);
    setToolTip("<b>" + getName() + "</b> [" + QString::number(leftBound) + ".." + 
        QString::number(rightBound) + "]");
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 FieldGraphItem::getOffset() const
{
	return expressionParser_->parseExpression(field_->getBitOffsetExpression()).toUInt();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int FieldGraphItem::getBitWidth() const
{
	return expressionParser_->parseExpression(field_->getBitWidthExpression()).toInt();
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
    const int MSB = getOffset() + getBitWidth();

    if (MSB == 0)
    {
        return 0;
    }

    return MSB -1;
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

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::isPresent()
//-----------------------------------------------------------------------------
bool FieldGraphItem::isPresent() const
{
    return expressionParser_->parseExpression(field_->getIsPresentExpression()) == "1";
}
