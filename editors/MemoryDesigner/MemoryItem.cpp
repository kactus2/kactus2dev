//-----------------------------------------------------------------------------
// File: MemoryItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
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
MemoryItem::MemoryItem(QString const& name, QString const& type):
name_(name),
type_(type)
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
// Function: MemoryItem::setDisplayName()
//-----------------------------------------------------------------------------
void MemoryItem::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getDisplayName()
//-----------------------------------------------------------------------------
QString MemoryItem::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setDescription()
//-----------------------------------------------------------------------------
void MemoryItem::setDescription(QString const& newDescription)
{
    description_ = newDescription;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getDescription()
//-----------------------------------------------------------------------------
QString MemoryItem::getDescription() const
{
    return description_;
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
// Function: MemoryItem::setFormattedAddressExpression()
//-----------------------------------------------------------------------------
void MemoryItem::setFormattedAddressExpression(QString const& newFormattedAddressExpression)
{
    formattedAddressExpression_ = newFormattedAddressExpression;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getFormattedAddressExpression()
//-----------------------------------------------------------------------------
QString MemoryItem::getFormattedAddressExpression() const
{
    return formattedAddressExpression_;
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
// Function: MemoryItem::setFormattedRangeExpression()
//-----------------------------------------------------------------------------
void MemoryItem::setFormattedRangeExpression(QString const& newRangeExpression)
{
    formattedRangeExpressin_ = newRangeExpression;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getFormattedRangeExpression()
//-----------------------------------------------------------------------------
QString MemoryItem::getFormattedRangeExpression() const
{
    return formattedRangeExpressin_;
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
// Function: MemoryItem::setFormattedWidthExpression()
//-----------------------------------------------------------------------------
void MemoryItem::setFormattedWidthExpression(QString const& newWidthExpression)
{
    formattedWidthExpression_ = newWidthExpression;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getFormattedWidthExpression()
//-----------------------------------------------------------------------------
QString MemoryItem::getFormattedWidthExpression() const
{
    return formattedWidthExpression_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setAccess()
//-----------------------------------------------------------------------------
void MemoryItem::setAccess(AccessTypes::Access const& newAccess)
{
    access_ = newAccess;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access MemoryItem::getAccess() const
{
    return access_;
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
// Function: MemoryItem::setFormattedOffsetExpression()
//-----------------------------------------------------------------------------
void MemoryItem::setFormattedOffsetExpression(QString const& newOffsetExpression)
{
    formattedOffsetExpression_ = newOffsetExpression;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getFormattedOffsetExpression()
//-----------------------------------------------------------------------------
QString MemoryItem::getFormattedOffsetExpression() const
{
    return formattedOffsetExpression_;
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
QVector<QSharedPointer<MemoryItem> > MemoryItem::getChildItems() const
{
    return childItems_;         
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setDimension()
//-----------------------------------------------------------------------------
void MemoryItem::setDimension(QString const& dimension)
{
    dimension_ = dimension;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getWidth()
//-----------------------------------------------------------------------------
QString MemoryItem::getDimension() const
{
    return dimension_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setUsage()
//-----------------------------------------------------------------------------
void MemoryItem::setUsage(General::Usage const& newUsage)
{
    usage_ = newUsage;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getUsage()
//-----------------------------------------------------------------------------
General::Usage MemoryItem::getUsage() const
{
    return usage_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setValue()
//-----------------------------------------------------------------------------
void MemoryItem::setValue(QString const& newValue)
{
    value_ = newValue;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getValue()
//-----------------------------------------------------------------------------
QString MemoryItem::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getResetValue()
//-----------------------------------------------------------------------------
QString MemoryItem::getResetValue() const
{
    return resetValue_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setResetValue()
//-----------------------------------------------------------------------------
void MemoryItem::setResetValue(QString const& newResetValue)
{
    resetValue_ = newResetValue;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getResetMask()
//-----------------------------------------------------------------------------
QString MemoryItem::getResetMask() const
{
    return resetMask_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setResetMask()
//-----------------------------------------------------------------------------
void MemoryItem::setResetMask(QString const& newResetMask)
{
    resetMask_ = newResetMask;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getIsPresent()
//-----------------------------------------------------------------------------
QString MemoryItem::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setIsPresent()
//-----------------------------------------------------------------------------
void MemoryItem::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setAddressSpaceReference()
//-----------------------------------------------------------------------------
void MemoryItem::setInitiatorReference(QString const& newReference)
{
    initiatorReference_ = newReference;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getAddressSpaceReference()
//-----------------------------------------------------------------------------
QString MemoryItem::getInitiatorReference() const
{
    return initiatorReference_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::setSegmentReference()
//-----------------------------------------------------------------------------
void MemoryItem::setSegmentReference(QString const& newReference)
{
    segmentReference_ = newReference;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getSegmentReference()
//-----------------------------------------------------------------------------
QString MemoryItem::getSegmentReference() const
{
    return segmentReference_;
}
