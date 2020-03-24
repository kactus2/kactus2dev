//-----------------------------------------------------------------------------
// File: memorygapitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 20.11.2012
//
// Description:
// The graphical item that represents a gap in the memory between objects.
//-----------------------------------------------------------------------------

#include "memorygapitem.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: MemoryGapItem()
//-----------------------------------------------------------------------------
MemoryGapItem::MemoryGapItem(QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent):
MemoryVisualizationItem(expressionParser, parent)
{
	setBrush(QBrush(QColor(Qt::white)));
	ExpandableItem::setExpansionBrush(QBrush(QColor(Qt::white)));

    setShowExpandableItem(false);
    setAddressPosition(LABELS_LEFT);

	setName("Reserved");	
    setToolTip("This memory block is reserved for future use.");
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::refresh()
//-----------------------------------------------------------------------------
void MemoryGapItem::refresh() 
{
	ExpandableItem::reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::updateDisplay()
//-----------------------------------------------------------------------------
void MemoryGapItem::updateDisplay()
{
    setLeftTopCorner(QString::number(firstFreeAddress_));

    setLeftBottomCorner(QString::number(lastFreeAddress_));
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::getOffset()
//-----------------------------------------------------------------------------
quint64 MemoryGapItem::getOffset() const
{
	return firstFreeAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryGapItem::getLastAddress() const
{
    return lastFreeAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::getBitWidth()
//-----------------------------------------------------------------------------
int MemoryGapItem::getBitWidth() const
{
	auto memItem = static_cast<MemoryVisualizationItem const*>(parentItem());
	Q_ASSERT(memItem);
	return memItem->getBitWidth();
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int MemoryGapItem::getAddressUnitSize() const
{
	auto memItem = static_cast<MemoryVisualizationItem const*>(parentItem());
	Q_ASSERT(memItem);
	return memItem->getAddressUnitSize();
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::setConflicted()
//-----------------------------------------------------------------------------
void MemoryGapItem::setConflicted(bool conflicted)
{
    // Nothing to do.
}
