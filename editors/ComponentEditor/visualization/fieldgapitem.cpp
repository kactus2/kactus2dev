//-----------------------------------------------------------------------------
// File: fieldgapitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 30.11.2012
//
// Description:
// The graphical item that represents a gap between fields.
//-----------------------------------------------------------------------------

#include "fieldgapitem.h"

//-----------------------------------------------------------------------------
// Function: FieldGapItem()
//-----------------------------------------------------------------------------
FieldGapItem::FieldGapItem(QString const& name, QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent):
MemoryGapItem(expressionParser, parent)
{
    // fields show name in the middle
    setLayoutType(LABELS_TOP);
    setClipText(true);

    setName(name);

    // fields can never be expanded
    setExpansionRectVisible(false);
}

//-----------------------------------------------------------------------------
// Function: updateDisplay()
//-----------------------------------------------------------------------------
void FieldGapItem::updateDisplay()
{
	QString startStr = QString::number(firstFreeAddress_);
    VisualizerItem::setBottomLabelText(startStr);

	QString endStr = QString::number(lastFreeAddress_);
    VisualizerItem::setTopLabelText(endStr);
    setToolTip("<b>" + name() + "</b> [" + endStr + ".." + startStr + "]");
}

//-----------------------------------------------------------------------------
// Function: getBitWidth()
//-----------------------------------------------------------------------------
int FieldGapItem::getBitWidth() const
{
	return lastFreeAddress_ - firstFreeAddress_ + 1;
}

//-----------------------------------------------------------------------------
// Function: itemTotalWidth()
//-----------------------------------------------------------------------------
qreal FieldGapItem::itemTotalWidth() const
{
	return rect().width();
}

//-----------------------------------------------------------------------------
// Function: FieldGapItem::setConflicted()
//-----------------------------------------------------------------------------
void FieldGapItem::setConflicted(bool /*conflicted*/)
{
    // Do nothing.
}
