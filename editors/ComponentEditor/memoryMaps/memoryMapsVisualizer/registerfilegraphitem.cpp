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
    MemoryVisualizationItem(expressionParser, parent),
    ArrayableMemoryGraphItem(),
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
// Function: RegisterFileGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 RegisterFileGraphItem::getOffset() const
{    
    return offset_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::setOffset()
//-----------------------------------------------------------------------------
void RegisterFileGraphItem::setOffset(quint64 newOffset)
{
    offset_ = newOffset;
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
