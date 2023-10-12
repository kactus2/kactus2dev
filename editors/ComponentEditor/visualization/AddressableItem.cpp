//-----------------------------------------------------------------------------
// File: AddressableItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 11.10.2023
//
// Description:
// Common base class for address space and memory map visualization items.
//-----------------------------------------------------------------------------

#include "AddressableItem.h"
#include "memorygapitem.h"

#include <common/KactusColors.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QRectF>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: AddressableItem::AddressableItem()
//-----------------------------------------------------------------------------
AddressableItem::AddressableItem(QGraphicsItem* parent):
VisualizerItem(parent)
{
    setPen(QPen(Qt::gray));
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::setDisplayOffset()
//-----------------------------------------------------------------------------
void AddressableItem::setDisplayOffset(quint64 const& address)
{
    firstAddress_ = address;
    setTopLabelText(firstAddress_);
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::getDisplayOffset()
//-----------------------------------------------------------------------------
quint64 AddressableItem::getDisplayOffset()
{
    return firstAddress_;
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::setDisplayLastAddress()
//-----------------------------------------------------------------------------
void AddressableItem::setDisplayLastAddress(quint64 const& address)
{
    lastAddress_ = address;

    if (firstAddress_ != lastAddress_)
    {
        setBottomLabelText(lastAddress_);
    }
    else
    {
        VisualizerItem::setBottomLabelText(QString());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::getDisplayLastAddress()
//-----------------------------------------------------------------------------
quint64 AddressableItem::getDisplayLastAddress()
{
    return lastAddress_;
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::setConflicted()
//-----------------------------------------------------------------------------
void AddressableItem::setConflicted(bool conflicted)
{
    conflicted_ = conflicted;

    QBrush colorBrush = defaultBrush();
    if (conflicted)
    {
        colorBrush = KactusColors::MISSING_COMPONENT;
    }

    setBrush(colorBrush);
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::isConflicted()
//-----------------------------------------------------------------------------
bool AddressableItem::isConflicted() const
{
    return conflicted_;
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::setLeftTopCorner()
//-----------------------------------------------------------------------------
void AddressableItem::setTopLabelText(quint64 address)
{
    VisualizerItem::setTopLabelText(toHexString(address, getBitWidth()));
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::setLeftBottomCorner()
//-----------------------------------------------------------------------------
void AddressableItem::setBottomLabelText(quint64 address)
{
    VisualizerItem::setBottomLabelText(toHexString(address, getBitWidth()));
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::toHexString()
//-----------------------------------------------------------------------------
QString AddressableItem::toHexString(quint64 address, int bitWidth)
{
    QString str = QString::number(address, 16);
    str = str.toUpper();

    // one hexadecimal number accounts for four bits
    int fieldSize = bitWidth / 4;
    if (fieldSize * 4 < bitWidth)
    {
        fieldSize++; //Round upwards, e.g. 7 bits needs 4 hex digits
    }

    QString padded = QString("%1").arg(str, fieldSize, QLatin1Char('0'));
    int size = padded.size();
    for (int i = size; i > 0; i -= 4)
    {
        padded.insert(i, QLatin1Char(' '));
    }

    return padded;
}

//-----------------------------------------------------------------------------
// Function: AddressableItem::compareItems()
//-----------------------------------------------------------------------------
bool AddressableItem::compareItems(const AddressableItem* s1, const AddressableItem* s2)
{
    // Item with bigger last address precedes the other.
    return s1->getLastAddress() > s2->getLastAddress();
}
