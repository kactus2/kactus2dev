//-----------------------------------------------------------------------------
// File: MemoryBlockGraphItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.03.2022
//
// Description:
// The graphical item that represents one memory block.
//-----------------------------------------------------------------------------

#include "MemoryBlockGraphItem.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QStringBuilder>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: MemoryBlockGraphItem::MemoryBlockGraphItem()
//-----------------------------------------------------------------------------
MemoryBlockGraphItem::MemoryBlockGraphItem(QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem *parent):
MemoryVisualizationItem(expressionParser, parent),
addressableUnitBits_(0)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void MemoryBlockGraphItem::updateDisplay()
{
    setName(getName());

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);

    setToolTip("<b>Name: </b>" % getName() % "<br>" %
        "<b>Offset: </b>" % toHexString(offset) % "<br>" %
        "<b>Last address: </b>" % toHexString(lastAddress) % "<br>" %
        "<b>Size [AUB]: </b>" % QString::number(getRange()));
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockGraphItem::setAddressableUnitBits()
//-----------------------------------------------------------------------------
void MemoryBlockGraphItem::setAddressableUnitBits(int addressableUnitBits)
{
    addressableUnitBits_ = addressableUnitBits;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int MemoryBlockGraphItem::getAddressUnitSize() const
{
    return addressableUnitBits_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryBlockGraphItem::getLastAddress() const
{
    quint64 base = getOffset();
    quint64 range = getRange();

    quint64 lastAddr = base + range;

    if (lastAddr == 0) 
    {
        return 0;
    }

    return lastAddr -1;
}
