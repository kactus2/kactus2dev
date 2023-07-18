//-----------------------------------------------------------------------------
// File: NameGroupReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Reader for name group element.
//-----------------------------------------------------------------------------

#include "NameGroupReader.h"

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseNameGroup()
//-----------------------------------------------------------------------------
void NameGroupReader::parseNameGroup(QDomNode const& nameGroupNode, QSharedPointer<NameGroup> nameGroup)
{
    nameGroup->setName(Details::parseName(nameGroupNode));

    nameGroup->setDisplayName(Details::parseDisplayName(nameGroupNode));

    nameGroup->setShortDescription(Details::parseShortDescription(nameGroupNode));

    nameGroup->setDescription(Details::parseDescription(nameGroupNode));
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::Details::parseName()
//-----------------------------------------------------------------------------
QString NameGroupReader::Details::parseName(QDomNode const& nameGroupNode)
{
    return nameGroupNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::Details::parseDisplayName()
//-----------------------------------------------------------------------------
QString NameGroupReader::Details::parseDisplayName(QDomNode const& nameGroupNode)
{
    return nameGroupNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::Details::parseShortDescription()
//-----------------------------------------------------------------------------
QString NameGroupReader::Details::parseShortDescription(QDomNode const& nameGroupNode)
{
    return nameGroupNode.firstChildElement(QStringLiteral("ipxact:shortDescription")).firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::Details::parseDescription()
//-----------------------------------------------------------------------------
QString NameGroupReader::Details::parseDescription(QDomNode const& nameGroupNode)
{
    return nameGroupNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue();
}