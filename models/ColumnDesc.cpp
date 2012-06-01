//-----------------------------------------------------------------------------
// File: ColumnDesc.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.4.2012
//
// Description:
// Column description structure for designs.
//-----------------------------------------------------------------------------

#include "ColumnDesc.h"

//-----------------------------------------------------------------------------
// Function: ColumnDesc::ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc()
    : name_(),
      contentType_(COLUMN_CONTENT_IO),
      allowedItems_(CIT_NONE),
      width_(119),
      minWidth_(119)
{
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc(const QDomNode& node)
    : name_(),
      contentType_(COLUMN_CONTENT_IO),
      allowedItems_(CIT_NONE),
      width_(259),
      minWidth_(259)
{

    // Read the column description data from the attributes.
    QDomNamedNodeMap attributeMap =  node.attributes();

    name_ = attributeMap.namedItem("name").nodeValue();
    contentType_ = static_cast<ColumnContentType>(attributeMap.namedItem("contentType").nodeValue().toInt());
    allowedItems_ = attributeMap.namedItem("allowedItems").nodeValue().toUInt();

    if (attributeMap.contains("width"))
    {
        width_ = attributeMap.namedItem("width").nodeValue().toUInt();
    }
    else if (contentType_ == COLUMN_CONTENT_IO)
    {
        width_ = 119;
    }

    if (attributeMap.contains("minWidth"))
    {
        minWidth_ = attributeMap.namedItem("minWidth").nodeValue().toUInt();
    }
    else if (contentType_ == COLUMN_CONTENT_IO)
    {
        minWidth_ = 119;
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc(QString const& name, ColumnContentType contentType,
                       unsigned int allowedItems, unsigned int minWidth)
    : name_(name),
      contentType_(contentType),
      allowedItems_(allowedItems),
      width_(minWidth),
      minWidth_(minWidth)
{
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
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc(const ColumnDesc& other)
    : name_(other.name_),
      contentType_(other.contentType_),
      allowedItems_(other.allowedItems_),
      width_(other.width_),
      minWidth_(other.minWidth_)
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
void ColumnDesc::setContentType(ColumnContentType contentType)
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
    width_ = std::max(minWidth_, width);
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::getName()
//-----------------------------------------------------------------------------
QString const& ColumnDesc::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::getContentType()
//-----------------------------------------------------------------------------
ColumnContentType ColumnDesc::getContentType() const
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
