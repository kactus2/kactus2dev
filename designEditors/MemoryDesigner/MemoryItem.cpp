//-----------------------------------------------------------------------------
// File: MemoryItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2016
//
// Description:
// A memory in a connectivity graph.
//-----------------------------------------------------------------------------

#include "MemoryItem.h"

//-----------------------------------------------------------------------------
// Function: MemoryItem::MemoryItem()
//-----------------------------------------------------------------------------
MemoryItem::MemoryItem(QString const& name, QString const& type): name_(name), type_(type), identifier_(), 
    address_(), range_(), width_(), size_(), offset_(), childItems_()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryItem::~MemoryItem()
//-----------------------------------------------------------------------------
MemoryItem::~MemoryItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getName()
//-----------------------------------------------------------------------------
QString MemoryItem::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getType()
//-----------------------------------------------------------------------------
QString MemoryItem::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setIdentifier()
//-----------------------------------------------------------------------------
void MemoryItem::setIdentifier(QString const& identifier)
{
    identifier_ = identifier;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getIdentifier()
//-----------------------------------------------------------------------------
QString MemoryItem::getIdentifier() const
{
    return identifier_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setAUB()
//-----------------------------------------------------------------------------
void MemoryItem::setAUB(QString const& aub)
{
    aub_ = aub;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getAUB()
//-----------------------------------------------------------------------------
QString MemoryItem::getAUB() const
{
    return aub_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setAddress()
//-----------------------------------------------------------------------------
void MemoryItem::setAddress(QString const& address)
{
    address_ = address;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getAddress()
//-----------------------------------------------------------------------------
QString MemoryItem::getAddress() const
{
    return address_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setRange()
//-----------------------------------------------------------------------------
void MemoryItem::setRange(QString const& range)
{
    range_ = range;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getRange()
//-----------------------------------------------------------------------------
QString MemoryItem::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setWidth()
//-----------------------------------------------------------------------------
void MemoryItem::setWidth(QString const& width)
{
    width_ = width;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getWidth()
//-----------------------------------------------------------------------------
QString MemoryItem::getWidth() const
{
    return width_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setSize()
//-----------------------------------------------------------------------------
void MemoryItem::setSize(QString const& size)
{
    size_ = size;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getSize()
//-----------------------------------------------------------------------------
QString MemoryItem::getSize() const
{
    return size_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setOffset()
//-----------------------------------------------------------------------------
void MemoryItem::setOffset(QString const& offset)
{
    offset_ = offset;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getOffset()
//-----------------------------------------------------------------------------
QString MemoryItem::getOffset() const
{
    return offset_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::addChild()
//-----------------------------------------------------------------------------
void MemoryItem::addChild(QSharedPointer<MemoryItem> child)
{
    childItems_.append(child);
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getChildItems()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<MemoryItem> > MemoryItem::getChildItems()
{
    return childItems_;         
}
