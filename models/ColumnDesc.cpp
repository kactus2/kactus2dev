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
// Function: ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc(const QDomNode& node)
    : name(),
      contentType(COLUMN_CONTENT_IO),
      allowedItems(CIT_NONE)
{
    // Read the column description data from the attributes.
    QDomNamedNodeMap attributeMap =  node.attributes();

    name = attributeMap.namedItem("name").nodeValue();
    contentType = static_cast<ColumnContentType>(attributeMap.namedItem("contentType").nodeValue().toInt());
    allowedItems = attributeMap.namedItem("allowedItems").nodeValue().toInt();
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc(QString const& name, ColumnContentType contentType,
                               unsigned int allowedItems) : name(name), contentType(contentType),
                               allowedItems(allowedItems)
{
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc::ColumnDesc( const ColumnDesc& other )
    : name(other.name),
      contentType(other.contentType),
      allowedItems(other.allowedItems)
{
}

//-----------------------------------------------------------------------------
// Function: ColumnDesc::ColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc& ColumnDesc::operator=(const ColumnDesc& other)
{
    if (this != &other) {
        name = other.name;
        contentType = other.contentType;
        allowedItems = other.allowedItems;
    }
    return *this;
}