//-----------------------------------------------------------------------------
// File: fieldgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 25.4.2012
//
// Description:
// The graphical item that represents a bit field within a register.
//-----------------------------------------------------------------------------

#include "fieldgraphitem.h"

#include <common/KactusColors.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QStringBuilder>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::FieldGraphItem()
//-----------------------------------------------------------------------------
FieldGraphItem::FieldGraphItem( QSharedPointer<Field> field, QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent):
ArrayableMemoryGraphItem(expressionParser, parent),
field_(field)
{
	Q_ASSERT(field_);

    setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
	
	VisualizerItem::setDefaultBrush(QBrush(KactusColors::FIELD_COLOR));

    setLayoutType(VisualizerItem::LABELS_TOP);
    setClipText(true);

	setShowExpandableItem(false);
	setExpansionRectVisible(false);

    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void FieldGraphItem::updateDisplay()
{
    // Get name with replica index
    QString formattedName = getReplicaName(field_->name());

    quint64 leftBound = getLastAddress();
    quint64 rightBound = getOffset();

    setName(formattedName);
    setDisplayOffset(leftBound);
    setDisplayLastAddress(rightBound);
    setToolTip("<b>" % formattedName % "</b> [" % QString::number(leftBound) % ".." % QString::number(rightBound) % "]");
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int FieldGraphItem::getBitWidth() const
{
    // Force field minimum width to 1 bit in visualizer
    return qMax(quint64(1), parseExpression(field_->getBitWidth()));
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
    return qMax(0, MSB - 1);
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setDisplayOffset()
//-----------------------------------------------------------------------------
void FieldGraphItem::setDisplayOffset(quint64 const& address)
{
    // Fields show decimal number offsets.
    firstAddress_ = address;
    VisualizerItem::setTopLabelText(QString::number(firstAddress_));
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::setDisplayLastAddress()
//-----------------------------------------------------------------------------
void FieldGraphItem::setDisplayLastAddress(quint64 const& address)
{
    // Fields show decimal number offsets.
    lastAddress_ = address;
    VisualizerItem::setBottomLabelText(QString::number(lastAddress_));
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::setConflicted()
//-----------------------------------------------------------------------------
void FieldGraphItem::setConflicted(bool conflicted)
{
    MemoryVisualizationItem::setConflicted(conflicted);
    if (conflicted)
    {
        setOpacity(0.7);
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
    return field_->getIsPresent().isEmpty() || parseExpression(field_->getIsPresent()) == 1;
}

//-----------------------------------------------------------------------------
// Function: FieldGraphItem::redoChildLayout()
//-----------------------------------------------------------------------------
void FieldGraphItem::redoChildLayout()
{
    // Do nothing.
}
