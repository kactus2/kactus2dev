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

#include <common/KactusUtils.h>
#include <KactusAPI/include/KactusColors.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: MemoryGapItem()
//-----------------------------------------------------------------------------
MemoryGapItem::MemoryGapItem(QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent):
MemoryVisualizationItem(expressionParser, parent)
{
	setDefaultBrush(QBrush(KactusColors::MEM_ITEM_EXTENSION));

	// Color labels of gap items differently if dark mode is enabled
    if (KactusUtils::darkThemeEnabled())
    {
        auto topTextBrush = getTopLabel()->brush();
        topTextBrush.setColor(KactusColors::REGULAR_TEXT);
        getTopLabel()->setBrush(topTextBrush);

        auto bottomTextBrush = getBottomLabel()->brush();
        bottomTextBrush.setColor(KactusColors::REGULAR_TEXT);
        getBottomLabel()->setBrush(bottomTextBrush);

        auto nameTextBrush = getNameLabel()->brush();
        nameTextBrush.setColor(KactusColors::REGULAR_TEXT);
        getNameLabel()->setBrush(nameTextBrush);
    }

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
