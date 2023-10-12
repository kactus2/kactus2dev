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
	setDefaultBrush(QBrush(QColor(Qt::white)));

    setShowExpandableItem(false);

	setName(QStringLiteral("Reserved"));
    setToolTip("This memory block is reserved for future use.");
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::updateDisplay()
//-----------------------------------------------------------------------------
void MemoryGapItem::updateDisplay()
{
    // Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::getOffset()
//-----------------------------------------------------------------------------
quint64 MemoryGapItem::getOffset() const
{
	return firstAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGapItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryGapItem::getLastAddress() const
{
    return lastAddress_;
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
// Function: MemoryGapItem::redoChildLayout()
//-----------------------------------------------------------------------------
void MemoryGapItem::redoChildLayout()
{
    // Do nothing. Gaps do not have child items.
}
