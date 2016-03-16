//-----------------------------------------------------------------------------
// File: EnumeratedValueReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Reader class for IP-XACT enumerated value element.
//-----------------------------------------------------------------------------

#include "EnumeratedValueReader.h"
#include "EnumeratedValue.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: EnumeratedValueReader::EnumeratedValueReader()
//-----------------------------------------------------------------------------
EnumeratedValueReader::EnumeratedValueReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueReader::~EnumeratedValueReader()
//-----------------------------------------------------------------------------
EnumeratedValueReader::~EnumeratedValueReader()
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueReader::createEnumeratedValueFrom()
//-----------------------------------------------------------------------------
QSharedPointer<EnumeratedValue> EnumeratedValueReader::createEnumeratedValueFrom
    (QDomNode const& enumeratedValueNode) const
{
    QSharedPointer<EnumeratedValue> newEnumeratedValue (new EnumeratedValue());

    parseUsage(enumeratedValueNode, newEnumeratedValue);

    parseNameGroup(enumeratedValueNode, newEnumeratedValue);

    parseValue(enumeratedValueNode, newEnumeratedValue);

    parseVendorExtensions(enumeratedValueNode, newEnumeratedValue);

    return newEnumeratedValue;
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueReader::parseNameGroup()
//-----------------------------------------------------------------------------
void EnumeratedValueReader::parseNameGroup(QDomNode const& enumerationNode,
    QSharedPointer<EnumeratedValue> newEnumeration) const
{
    NameGroupReader nameGroupReader;
    nameGroupReader.parseNameGroup(enumerationNode, newEnumeration);
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueReader::parseUsage()
//-----------------------------------------------------------------------------
void EnumeratedValueReader::parseUsage(QDomNode const& enumerationNode,
    QSharedPointer<EnumeratedValue> newEnumeration) const
{
    QDomElement enumerationElement = enumerationNode.toElement();
    if (enumerationElement.hasAttribute("usage"))
    {
        QString usageString = enumerationElement.attribute("usage");
        EnumeratedValue::EnumeratedUsage usage =
            EnumeratedValue::str2Usage(usageString, EnumeratedValue::UNKNOWNUSAGE);

        newEnumeration->setUsage(usage);
    }
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueReader::parseValue()
//-----------------------------------------------------------------------------
void EnumeratedValueReader::parseValue(QDomNode const& enumerationNode,
    QSharedPointer<EnumeratedValue> newEnumeration) const
{
    QString value = enumerationNode.firstChildElement("ipxact:value").firstChild().nodeValue();
    newEnumeration->setValue(value);
}