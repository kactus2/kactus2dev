//-----------------------------------------------------------------------------
// File: FieldReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.09.2015
//
// Description:
// Reader class for IP-XACT field element.
//-----------------------------------------------------------------------------

#include "FieldReader.h"
#include "Field.h"
#include "EnumeratedValueReader.h"
#include "EnumeratedValue.h"
#include "WriteValueConstraint.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: FieldReader::FieldReader()
//-----------------------------------------------------------------------------
FieldReader::FieldReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FieldReader::~FieldReader()
//-----------------------------------------------------------------------------
FieldReader::~FieldReader()
{

}

//-----------------------------------------------------------------------------
// Function: FieldReader::createFieldFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Field> FieldReader::createFieldFrom(QDomNode const& fieldNode) const
{
    QDomElement fieldElement = fieldNode.toElement();

    QSharedPointer<Field> newField (new Field());

    parseID(fieldElement, newField);

    parseNameGroup(fieldNode, newField);

    parsePresence(fieldElement, newField);

    parseBitOffset(fieldElement, newField);

    parseReset(fieldElement, newField);

    parseTypeIdentifier(fieldElement, newField);

    parseBitWidth(fieldElement, newField);

    parseVolatile(fieldElement, newField);

    parseAccess(fieldElement, newField);

    parseEnumeratedValues(fieldElement, newField);

    parseModifiedWriteValue(fieldElement, newField);

    parseWriteValueConstraint(fieldElement, newField);

    parseReadAction(fieldElement, newField);

    parseTestable(fieldElement, newField);

    parseReserved(fieldElement, newField);

    parseParameters(fieldElement, newField);

    parseVendorExtensions(fieldNode, newField);

    return newField;
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseID()
//-----------------------------------------------------------------------------
void FieldReader::parseID(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    if (fieldElement.hasAttribute(QStringLiteral("fieldID")))
    {
        QString fieldID = fieldElement.attribute(QStringLiteral("fieldID"));
        newField->setId(fieldID);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseNameGroup()
//-----------------------------------------------------------------------------
void FieldReader::parseNameGroup(QDomNode const& fieldNode, QSharedPointer<Field> newField) const
{
    NameGroupReader nameGroupReader;
    nameGroupReader.parseNameGroup(fieldNode, newField);
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parsePresence()
//-----------------------------------------------------------------------------
void FieldReader::parsePresence(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QString newIsPresent = parseIsPresent(fieldElement.firstChildElement(QStringLiteral("ipxact:isPresent")));
    if (!newIsPresent.isEmpty())
    {
        newField->setIsPresent(newIsPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseBitOffset()
//-----------------------------------------------------------------------------
void FieldReader::parseBitOffset(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement bitOffsetElement = fieldElement.firstChildElement(QStringLiteral("ipxact:bitOffset"));
    QString bitOffset = bitOffsetElement.firstChild().nodeValue();
    newField->setBitOffset(bitOffset);
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseReset()
//-----------------------------------------------------------------------------
void FieldReader::parseReset(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement resetsElement = fieldElement.firstChildElement(QStringLiteral("ipxact:resets"));
    if (!resetsElement.isNull())
    {
        QDomNodeList resetNodeList = resetsElement.elementsByTagName(QStringLiteral("ipxact:reset"));
        for (int resetIndex = 0; resetIndex < resetNodeList.count(); ++resetIndex)
        {
            QDomElement resetElement = resetNodeList.at(resetIndex).toElement();
            
            parseResetTypeRef(resetElement, newField);

            parseResetValue(resetElement, newField);

            parseResetMask(resetElement, newField);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseResetTypeRef()
//-----------------------------------------------------------------------------
void FieldReader::parseResetTypeRef(QDomElement const& resetElement, QSharedPointer<Field> newField) const
{
    if (resetElement.hasAttribute(QStringLiteral("resetTypeRef")))
    {
        newField->setResetTypeReference(resetElement.attribute(QStringLiteral("resetTypeRef")));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseResetValue()
//-----------------------------------------------------------------------------
void FieldReader::parseResetValue(QDomElement const& resetElement, QSharedPointer<Field> newField) const
{
    QString resetValue = resetElement.firstChildElement(QStringLiteral("ipxact:value")).firstChild().nodeValue();
    newField->setResetValue(resetValue);
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseResetMask()
//-----------------------------------------------------------------------------
void FieldReader::parseResetMask(QDomElement const& resetElement, QSharedPointer<Field> newField) const
{
    QDomElement resetMaskElement = resetElement.firstChildElement(QStringLiteral("ipxact:mask"));
    if (!resetMaskElement.isNull())
    {
        QString resetMask = resetMaskElement.firstChild().nodeValue();
        newField->setResetMask(resetMask);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseTypeIdentifier()
//-----------------------------------------------------------------------------
void FieldReader::parseTypeIdentifier(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement typeIdentifierElement = fieldElement.firstChildElement(QStringLiteral("ipxact:typeIdentifier"));
    if (!typeIdentifierElement.isNull())
    {
        QString typeIdentifier = typeIdentifierElement.firstChild().nodeValue();
        newField->setTypeIdentifier(typeIdentifier);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseBitWidth()
//-----------------------------------------------------------------------------
void FieldReader::parseBitWidth(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement bitWidthElement = fieldElement.firstChildElement(QStringLiteral("ipxact:bitWidth"));
    QString bitWidth = bitWidthElement.firstChild().nodeValue();
    newField->setBitWidth(bitWidth);
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseVolatile()
//-----------------------------------------------------------------------------
void FieldReader::parseVolatile(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement volatileElement = fieldElement.firstChildElement(QStringLiteral("ipxact:volatile"));
    if (!volatileElement.isNull())
    {
        if (volatileElement.firstChild().nodeValue() == QLatin1String("true"))
        {
            newField->setVolatile(true);
        }
        else
        {
            newField->setVolatile(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseAccess()
//-----------------------------------------------------------------------------
void FieldReader::parseAccess(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement accessElement = fieldElement.firstChildElement(QStringLiteral("ipxact:access"));
    if (!accessElement.isNull())
    {
        QString accessString = accessElement.firstChild().nodeValue();
        AccessTypes::Access access = AccessTypes::str2Access(accessString, AccessTypes::ACCESS_COUNT);
        newField->setAccess(access);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseEnumeratedValues()
//-----------------------------------------------------------------------------
void FieldReader::parseEnumeratedValues(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement enumeratedValuesElement = fieldElement.firstChildElement(QStringLiteral("ipxact:enumeratedValues"));
    if (!enumeratedValuesElement.isNull())
    {
        EnumeratedValueReader enumerationReader;

        QDomNodeList enumeratedValueNodes = enumeratedValuesElement.elementsByTagName(QStringLiteral("ipxact:enumeratedValue"));
        for (int enumerationIndex = 0; enumerationIndex < enumeratedValueNodes.count(); ++enumerationIndex)
        {
            QDomNode enumerationNode = enumeratedValueNodes.at(enumerationIndex);
            
            QSharedPointer<EnumeratedValue> enumeration =
                enumerationReader.createEnumeratedValueFrom(enumerationNode);

            newField->getEnumeratedValues()->append(enumeration);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseModifiedWriteValue()
//-----------------------------------------------------------------------------
void FieldReader::parseModifiedWriteValue(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement modifiedWriteElement = fieldElement.firstChildElement(QStringLiteral("ipxact:modifiedWriteValue"));
    if (!modifiedWriteElement.isNull())
    {
        QString modifiedWriteValueString = modifiedWriteElement.firstChild().nodeValue();
        General::ModifiedWrite modifiedWriteValue = General::str2ModifiedWrite(modifiedWriteValueString);
        newField->setModifiedWrite(modifiedWriteValue);

        if (modifiedWriteElement.hasAttribute(QStringLiteral("modify")))
        {
            newField->setModifiedWriteModify(modifiedWriteElement.attribute(QStringLiteral("modify")));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldReader::parseWriteValueConstraint(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement writeConstraintElement = fieldElement.firstChildElement(QStringLiteral("ipxact:writeValueConstraint"));
    if (!writeConstraintElement.isNull())
    {
        QSharedPointer<WriteValueConstraint> writeConstraint (new WriteValueConstraint());

        QDomElement writeAsReadElement = writeConstraintElement.firstChildElement(QStringLiteral("ipxact:writeAsRead"));
        QDomElement useEnumerationElement = writeConstraintElement.firstChildElement(QStringLiteral("ipxact:useEnumeratedValues"));
        if (!writeAsReadElement.isNull())
        {
            writeConstraint->setType(WriteValueConstraint::WRITE_AS_READ);
        }
        else if (!useEnumerationElement.isNull())
        {
            writeConstraint->setType(WriteValueConstraint::USE_ENUM);
        }
        else
        {
            QString minimum = writeConstraintElement.firstChildElement(QStringLiteral("ipxact:minimum")).firstChild().nodeValue();
            writeConstraint->setMinimum(minimum);

            QString maximum = writeConstraintElement.firstChildElement(QStringLiteral("ipxact:maximum")).firstChild().nodeValue();
            writeConstraint->setMaximum(maximum);
        }

        newField->setWriteConstraint(writeConstraint);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseReadAction()
//-----------------------------------------------------------------------------
void FieldReader::parseReadAction(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement readActionElement = fieldElement.firstChildElement(QStringLiteral("ipxact:readAction"));
    if (!readActionElement.isNull())
    {
        QString readActionString = readActionElement.firstChild().nodeValue();
        General::ReadAction readAction = General::str2ReadAction(readActionString);

        newField->setReadAction(readAction);

        if (readActionElement.hasAttribute(QStringLiteral("modify")))
        {
            newField->setReadActionModify(readActionElement.attribute(QStringLiteral("modify")));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseTestable()
//-----------------------------------------------------------------------------
void FieldReader::parseTestable(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement testableElement = fieldElement.firstChildElement(QStringLiteral("ipxact:testable"));
    if (!testableElement.isNull())
    {
        QString testableString = testableElement.firstChild().nodeValue();
        if (testableString == QLatin1String("true"))
        {
            newField->setTestable(true);
        }
        else
        {
            newField->setTestable(false);
        }

        if (testableElement.hasAttribute(QStringLiteral("testConstraint")))
        {
            QString testConstraintString = testableElement.attribute(QStringLiteral("testConstraint"));
            General::TestConstraint testConstraint = General::str2TestConstraint(testConstraintString);
            newField->setTestConstraint(testConstraint);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseReserved()
//-----------------------------------------------------------------------------
void FieldReader::parseReserved(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement reservedElement = fieldElement.firstChildElement(QStringLiteral("ipxact:reserved"));
    if (!reservedElement.isNull())
    {
        QString reserved = reservedElement.firstChild().nodeValue();
        newField->setReserved(reserved);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::parseParameters()
//-----------------------------------------------------------------------------
void FieldReader::parseParameters(QDomElement const& fieldElement, QSharedPointer<Field> newField) const
{
    QDomElement parametersElement = fieldElement.firstChildElement(QStringLiteral("ipxact:parameters"));
    if (!parametersElement.isNull())
    {
        QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = parseAndCreateParameters(fieldElement);
        foreach (QSharedPointer<Parameter> parameter, *newParameters)
        {
            newField->getParameters()->append(parameter);
        }
    }
}