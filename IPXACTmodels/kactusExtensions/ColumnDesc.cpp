//-----------------------------------------------------------------------------
// File: ColumnDesc.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.4.2012
//
// Description:
// ColumnDesc structure describes the kactus2:column vendor extension element in an IP-XACT design.
//-----------------------------------------------------------------------------

#include "ColumnDesc.h"

//-----------------------------------------------------------------------------
// Function: ColumnDesc::ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc()
    : name_(),
      contentType_(ColumnTypes::IO),
      allowedItems_(ColumnTypes::NONE),
      width_(119),
      minWidth_(119),
      xPosition_(0)
{
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc(QString const& name, ColumnTypes::ColumnContentType contentType,
                       unsigned int allowedItems, unsigned int minWidth)
    : name_(name),
      contentType_(contentType),
      allowedItems_(allowedItems),
      width_(minWidth),
      minWidth_(minWidth),
      xPosition_(0)
{
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::clone()
//-----------------------------------------------------------------------------
ColumnDesc* ColumnDesc::clone() const
{
    return new ColumnDesc(*this);
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::type()
//-----------------------------------------------------------------------------
QString ColumnDesc::type() const
{
    return QString("kactus2:column");
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::write()
//-----------------------------------------------------------------------------
void ColumnDesc::write(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement("kactus2:column");
    writer.writeAttribute("name", name_);
    writer.writeAttribute("contentType", QString::number(contentType_));
    writer.writeAttribute("allowedItems", QString::number(allowedItems_));
    writer.writeAttribute("minWidth", QString::number(minWidth_));
    writer.writeAttribute("width", QString::number(width_));
    writer.writeAttribute("x", QString::number(xPosition_));
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc(const ColumnDesc& other)
    : name_(other.name_),
      contentType_(other.contentType_),
      allowedItems_(other.allowedItems_),
      width_(other.width_),
      minWidth_(other.minWidth_),
      xPosition_(other.xPosition_)
{
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc& ColumnDesc::operator=(const ColumnDesc& other)
{
    if (this != &other)
    {
        name_ = other.name_;
        contentType_ = other.contentType_;
        allowedItems_ = other.allowedItems_;
        width_ = other.width_;
        minWidth_ = other.minWidth_;
        xPosition_ = other.xPosition_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::setName()
//-----------------------------------------------------------------------------
void ColumnDesc::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::setContentType()
//-----------------------------------------------------------------------------
void ColumnDesc::setContentType(ColumnTypes::ColumnContentType contentType)
{
    contentType_ = contentType;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::setAllowedItems()
//-----------------------------------------------------------------------------
void ColumnDesc::setAllowedItems(unsigned int allowedItems)
{
    allowedItems_ = allowedItems;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::setWidth()
//-----------------------------------------------------------------------------
void ColumnDesc::setWidth(unsigned int width)
{
    width_ = qMax(minWidth_, width);
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::setMinimumWidth()
//-----------------------------------------------------------------------------
void ColumnDesc::setMinimumWidth(int minimum)
{
    minWidth_ = minimum;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::name()
//-----------------------------------------------------------------------------
QString ColumnDesc::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::getContentType()
//-----------------------------------------------------------------------------
ColumnTypes::ColumnContentType ColumnDesc::getContentType() const
{
    return contentType_;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::getAllowedItems()
//-----------------------------------------------------------------------------
unsigned int ColumnDesc::getAllowedItems() const
{
    return allowedItems_;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::getWidth()
//-----------------------------------------------------------------------------
unsigned int ColumnDesc::getWidth() const
{
    return width_;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::getPosition()
//-----------------------------------------------------------------------------
unsigned int ColumnDesc::getPosition() const
{
    return xPosition_;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::setPosition()
//-----------------------------------------------------------------------------
void ColumnDesc::setPosition(unsigned int x)
{
    xPosition_ = x;
}
