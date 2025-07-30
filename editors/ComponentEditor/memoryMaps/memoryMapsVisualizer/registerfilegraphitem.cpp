//-----------------------------------------------------------------------------
// File: registerfilegraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// Graphical item to represent register file in visualization.
//-----------------------------------------------------------------------------
#include "registerfilegraphitem.h"

#include <KactusAPI/include/ExpressionParser.h>
#include <editors/ComponentEditor/visualization/memorygapitem.h>

#include <common/KactusColors.h>

#include <QStringBuilder>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::RegisterFileGraphItem()
//-----------------------------------------------------------------------------
RegisterFileGraphItem::RegisterFileGraphItem(QSharedPointer<RegisterFile> regFile,
    QSharedPointer<ExpressionParser> expressionParser,
    QGraphicsItem *parent) :
ArrayableMemoryGraphItem(expressionParser, parent),
regFile_(regFile)
{
    Q_ASSERT(regFile_);    

    setDefaultBrush(QBrush(KactusColors::REGISTER_FILE_COLOR));

    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void RegisterFileGraphItem::updateDisplay()
{
    // Get name with replica index
    QString formattedName = getReplicaName(regFile_->name());

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setName(formattedName);
    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);


    const int BIT_WIDTH = getBitWidth();
    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" % formattedName % "<br>" %
        "<b>AUB: </b>" % QString::number(getAddressUnitSize()) % "<br>" %
        "<b>First address: </b>" % toHexString(offset, BIT_WIDTH) % "<br>" %
        "<b>Last address: </b>" % toHexString(lastAddress, BIT_WIDTH));
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 RegisterFileGraphItem::getLastAddress() const
{
    quint64 base = getOffset();
    quint64 range = parseExpression(regFile_->getRange());

    quint64 lastAddr = base + range;

    if (lastAddr == 0)
    {
        return 0;
    }

    return lastAddr - 1;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int RegisterFileGraphItem::getBitWidth() const
{
    auto parent = static_cast<MemoryVisualizationItem*>(parentItem());
    return parent->getBitWidth();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int RegisterFileGraphItem::getAddressUnitSize() const
{
    auto parent = static_cast<MemoryVisualizationItem*>(parentItem());
    Q_ASSERT(parent);
    return parent->getAddressUnitSize();
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::isPresent()
//-----------------------------------------------------------------------------
bool RegisterFileGraphItem::isPresent() const
{
    return regFile_->getIsPresent().isEmpty() || parseExpression(regFile_->getIsPresent()) == 1;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::fillGapsBetweenChildren()
//-----------------------------------------------------------------------------
void RegisterFileGraphItem::fillGapsBetweenChildren()
{
    quint64 lastAddressInUse = getOffset();

    for (auto i = childItems_.begin(); i != childItems_.end(); ++i)
    {
        if (auto current = i.value(); current->isPresent())
        {
            auto thisOffset = getOffset();
            quint64 currentOffset = childItems_.key(current);
            auto relativeOffset = currentOffset - thisOffset;

            if (currentOffset > 0 && relativeOffset > 0 && i == childItems_.begin())
            {
                // Create gap immediately from start of register file, if needed (unlike memory map).
                i = createMemoryGap(lastAddressInUse, currentOffset - 1);
                lastAddressInUse = currentOffset - 1;
            }
            else if (emptySpaceBeforeChild(current, lastAddressInUse))
            {
                i = createMemoryGap(lastAddressInUse + 1, currentOffset - 1);
            }

            lastAddressInUse = qMax(current->getLastAddress(), lastAddressInUse);
        }
    }

    // Fill in any addresses left between children and the end of this item.
    if (childItems_.isEmpty() == false && getLastAddress() > lastAddressInUse)
    {
        createMemoryGap(lastAddressInUse + 1, getLastAddress());
    }
}
