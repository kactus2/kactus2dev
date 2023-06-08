//-----------------------------------------------------------------------------
// File: NameGroupReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Reader class for name group element.
//-----------------------------------------------------------------------------

#include "NameGroupReader.h"

//-----------------------------------------------------------------------------
// Function: NameGroupReader::NameGroupReader()
//-----------------------------------------------------------------------------
NameGroupReader::NameGroupReader() 
{

}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::~NameGroupReader()
//-----------------------------------------------------------------------------
NameGroupReader::~NameGroupReader()
{

}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseNameGroup()
//-----------------------------------------------------------------------------
void NameGroupReader::parseNameGroup(QDomNode const& nameGroupNode, QSharedPointer<NameGroup> nameGroup)
{
    nameGroup->setName(parseName(nameGroupNode));

    nameGroup->setDisplayName(parseDisplayName(nameGroupNode));

    nameGroup->setShortDescription(parseShortDescription(nameGroupNode));

    nameGroup->setDescription(parseDescription(nameGroupNode));
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseName()
//-----------------------------------------------------------------------------
QString NameGroupReader::parseName(QDomNode const& nameGroupNode)
{
    return nameGroupNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseDisplayName()
//-----------------------------------------------------------------------------
QString NameGroupReader::parseDisplayName(QDomNode const& nameGroupNode)
{
    return nameGroupNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseShortDescription()
//-----------------------------------------------------------------------------
QString NameGroupReader::parseShortDescription(QDomNode const& nameGroupNode)
{
    return nameGroupNode.firstChildElement(QStringLiteral("ipxact:shortDescription")).firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseDescription()
//-----------------------------------------------------------------------------
QString NameGroupReader::parseDescription(QDomNode const& nameGroupNode)
{
    return nameGroupNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue();
}