//-----------------------------------------------------------------------------
// File: localmemorymapgraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 17.12.2012
//
// Description:
// The graph item that visualizes a local memory map within address space.
//-----------------------------------------------------------------------------

#include "memorymapgraphitem.h"

#include <editors/ComponentEditor/visualization/memorygapitem.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <common/KactusColors.h>

#include <QBrush>
#include <QStringBuilder>

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::MemoryMapGraphItem()
//-----------------------------------------------------------------------------
MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> parentMemoryMap,
    QSharedPointer<MemoryMapBase> memoryRemap, QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem* parent):
MemoryVisualizationItem(expressionParser, parent),
    memoryMap_(memoryRemap),
    parentMemoryMap_(parentMemoryMap)
{
    Q_ASSERT(memoryMap_);

    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::refresh()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::refresh() 
{
    updateDisplay();
    reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::updateDisplay()
{
    setName(memoryMap_->name());

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);
  
    if (isPresent())
    {
        QBrush defaultBrush(KactusColors::MEM_MAP_COLOR);
        setDefaultBrush(defaultBrush);
    }
    else
    {
        QBrush nonPresentBrush(Qt::lightGray);
        setDefaultBrush(nonPresentBrush);
    }

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" % memoryMap_->name() % "<br>" %
        "<b>AUB: </b>" % QString::number(getAddressUnitSize()) % "<br>" %
        "<b>First address: </b>" % toHexString(offset) % "<br>" %
        "<b>Last address: </b>" % toHexString(lastAddress));
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getOffset() const 
{
    if (childItems_.isEmpty())
    {
        return 0;
    }

    quint64 offset = childItems_.last()->getOffset();
    for (MemoryVisualizationItem const* child : childItems_)
    {
        if (dynamic_cast<MemoryGapItem const*>(child) == 0)
        {
            offset = qMin(child->getOffset(), offset);  
        }
    }

    return offset;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int MemoryMapGraphItem::getBitWidth() const 
{
    int width = 0;
    for (MemoryVisualizationItem const* child : childItems_)
    {
        if (dynamic_cast<MemoryGapItem const*>(child) == 0)
        {
            width = qMax(child->getBitWidth(), width);  
        }
    }
    return width;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int MemoryMapGraphItem::getAddressUnitSize() const 
{
    return parseExpression(parentMemoryMap_->getAddressUnitBits());
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphItem::getLastAddress() const 
{
    quint64 lastAddress = 0;
    for (MemoryVisualizationItem const* child : childItems_)
    {
        if (dynamic_cast<MemoryGapItem const*>(child) == 0)
        {
            lastAddress = qMax(child->getLastAddress(), lastAddress);  
        }
    }

    return lastAddress;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::itemTotalWidth()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphItem::itemTotalWidth() const 
{
    return VisualizerItem::DEFAULT_WIDTH;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphItem::isPresent()
//-----------------------------------------------------------------------------
bool MemoryMapGraphItem::isPresent() const
{
    return memoryMap_->getIsPresent().isEmpty() || parseExpression(memoryMap_->getIsPresent()) == 1;
}

//-----------------------------------------------------------------------------
// Function: memorymapgraphitem::getMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMapBase> MemoryMapGraphItem::getMemoryMap() const
{
    return memoryMap_;
}

//-----------------------------------------------------------------------------
// Function: memorymapgraphitem::redoChildLayout()
//-----------------------------------------------------------------------------
void MemoryMapGraphItem::redoChildLayout()
{
    // TODO.
}

