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
NameGroupReader::NameGroupReader(QObject* parent /* = 0 */) :
QObject(parent)
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
void NameGroupReader::parseNameGroup(QDomNode const& nameGroupNode, QSharedPointer<NameGroup> nameGroup) const
{
    QString newName = parseName(nameGroupNode);
    nameGroup->setName(newName);

    QString newDisplayName = parseDisplayName(nameGroupNode);
    if (!newDisplayName.isEmpty())
    {
        nameGroup->setDisplayName(newDisplayName);
    }

    QString newDescription = parseDescription(nameGroupNode);
    if (!newDescription.isEmpty())
    {
        nameGroup->setDescription(newDescription);
    }
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseName()
//-----------------------------------------------------------------------------
QString NameGroupReader::parseName(QDomNode const& nameGroupNode) const
{
    return nameGroupNode.firstChildElement("ipxact:name").firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseDisplayName()
//-----------------------------------------------------------------------------
QString NameGroupReader::parseDisplayName(QDomNode const& nameGroupNode) const
{
    return nameGroupNode.firstChildElement("ipxact:displayName").firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: NameGroupReader::parseDescription()
//-----------------------------------------------------------------------------
QString NameGroupReader::parseDescription(QDomNode const& nameGroupNode) const
{
    return nameGroupNode.firstChildElement("ipxact:description").firstChild().nodeValue();
}