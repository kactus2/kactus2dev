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
    QString name = regFile_->name();
    
    int dimension = parseExpression(regFile_->getDimension());
    if (dimension > 0)
    {
        name.append("[" % QString::number(dimension - 1) % ":0]");
    }

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setName(name);
    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" % regFile_->name() % "<br>" %
        "<b>AUB: </b>" % QString::number(getAddressUnitSize()) % "<br>" %
        "<b>First address: </b>" % toHexString(offset) % "<br>" %
        "<b>Last address: </b>" % toHexString(lastAddress));
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 RegisterFileGraphItem::getOffset() const
{    
    // the address block's offset
    auto parent = static_cast<MemoryVisualizationItem*>(parentItem());
    Q_ASSERT(parent);
    quint64 parentOffset = parent->getOffset();
    
    // the register offset from the address block
    quint64 regFileOffset = parseExpression(regFile_->getAddressOffset());

    // the total offset is the address block's offset added with register's offset
    return parentOffset + regFileOffset;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 RegisterFileGraphItem::getLastAddress() const
{
    quint64 base = getOffset();
    quint64 range = parseExpression(regFile_->getRange());
    quint64 dimension = qMax(quint64(1), parseExpression(regFile_->getDimension()));

    quint64 lastAddr = base + range * dimension;

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
