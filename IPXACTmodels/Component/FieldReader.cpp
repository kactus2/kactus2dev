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
#include "MemoryArrayReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: FieldReader::createFieldFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Field> FieldReader::createFieldFrom(QDomNode const& fieldNode, Document::Revision docRevision)
{
    QDomElement fieldElement = fieldNode.toElement();

    QSharedPointer<Field> newField (new Field());

    NameGroupReader::parseNameGroup(fieldNode, newField);
    
    if (docRevision == Document::Revision::Std14)
    {
        Details::parseID(fieldElement, newField);
        Details::parsePresence(fieldElement, newField);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::parseMemoryArray(fieldElement, newField);
        Details::parseFieldDefinitionRef(fieldElement, newField);
    }

    Details::parseBitOffset(fieldElement, newField);

    Details::parseResets(fieldElement, newField);

    Details::parseTypeIdentifier(fieldElement, newField);

    Details::parseBitWidth(fieldElement, newField);

    Details::parseVolatile(fieldElement, newField);

    Details::parseAccess(fieldElement, newField);

    Details::parseEnumeratedValues(fieldElement, newField);

    Details::parseModifiedWriteValue(fieldElement, newField);

    Details::parseWriteValueConstraint(fieldElement, newField);

    Details::parseReadAction(fieldElement, newField);

    Details::parseTestable(fieldElement, newField);

    Details::parseReserved(fieldElement, newField);

    Details::parseParameters(fieldElement, newField);

    CommonItemsReader::parseVendorExtensions(fieldNode, newField);

    return newField;
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseID()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseID(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    if (fieldElement.hasAttribute(QStringLiteral("fieldID")))
    {
        QString fieldID = fieldElement.attribute(QStringLiteral("fieldID"));
        newField->setId(fieldID);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parsePresence()
//-----------------------------------------------------------------------------
void FieldReader::Details::parsePresence(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    QString newIsPresent = CommonItemsReader::parseIsPresent(fieldElement.firstChildElement(QStringLiteral("ipxact:isPresent")));
    if (!newIsPresent.isEmpty())
    {
        newField->setIsPresent(newIsPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseMemoryArray()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseMemoryArray(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    auto newArray = MemoryArrayReader::createMemoryArrayFrom(
        fieldElement.firstChildElement(QStringLiteral("ipxact:array")), true);

    newField->setMemoryArray(newArray);
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseBitOffset()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseBitOffset(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    QDomElement bitOffsetElement = fieldElement.firstChildElement(QStringLiteral("ipxact:bitOffset"));
    QString bitOffset = bitOffsetElement.firstChild().nodeValue();
    newField->setBitOffset(bitOffset);
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseFieldDefinitionRef()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseFieldDefinitionRef(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    QDomElement fieldDefElement = fieldElement.firstChildElement(QStringLiteral("ipxact:fieldDefinitionRef"));
    if (!fieldDefElement.isNull())
    {
        newField->setFieldDefinitionRef(fieldDefElement.firstChild().nodeValue());
        newField->setTypeDefinitionsRef(fieldDefElement.attribute(QStringLiteral("typeDefinitions"), QString()));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseResets()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseResets(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    QDomElement resetsElement = fieldElement.firstChildElement(QStringLiteral("ipxact:resets"));
    if (!resetsElement.isNull())
    {
        QDomNodeList resetNodeList = resetsElement.elementsByTagName(QStringLiteral("ipxact:reset"));
        for (int resetIndex = 0; resetIndex < resetNodeList.count(); ++resetIndex)
        {
            QDomElement resetElement = resetNodeList.at(resetIndex).toElement();

            QSharedPointer<FieldReset> fldReset = QSharedPointer<FieldReset>(new FieldReset());

            parseResetTypeRef(resetElement, fldReset);
            parseResetValue(resetElement, fldReset);
            parseResetMask(resetElement, fldReset);

            newField->getResets()->append(fldReset);

        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseResetTypeRef()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseResetTypeRef(QDomElement const& resetElement, QSharedPointer<FieldReset> fieldReset)
{
  if (resetElement.hasAttribute(QStringLiteral("resetTypeRef")))
  {
      fieldReset->setResetTypeReference(resetElement.attribute(QStringLiteral("resetTypeRef")));
  }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseResetValue()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseResetValue(QDomElement const& resetElement, QSharedPointer<FieldReset> fieldReset)
{
  QString resetValue = resetElement.firstChildElement(QStringLiteral("ipxact:value")).firstChild().nodeValue();
  fieldReset->setResetValue(resetValue);
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseResetMask()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseResetMask(QDomElement const& resetElement, QSharedPointer<FieldReset> fieldReset)
{
  QDomElement resetMaskElement = resetElement.firstChildElement(QStringLiteral("ipxact:mask"));
  if (!resetMaskElement.isNull())
  {
      QString resetMask = resetMaskElement.firstChild().nodeValue();
      fieldReset->setResetMask(resetMask);
  }
}


//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseTypeIdentifier()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseTypeIdentifier(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    QDomElement typeIdentifierElement = fieldElement.firstChildElement(QStringLiteral("ipxact:typeIdentifier"));
    if (!typeIdentifierElement.isNull())
    {
        QString typeIdentifier = typeIdentifierElement.firstChild().nodeValue();
        newField->setTypeIdentifier(typeIdentifier);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseBitWidth()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseBitWidth(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    QDomElement bitWidthElement = fieldElement.firstChildElement(QStringLiteral("ipxact:bitWidth"));
    QString bitWidth = bitWidthElement.firstChild().nodeValue();
    newField->setBitWidth(bitWidth);
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseVolatile()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseVolatile(QDomElement const& fieldElement, QSharedPointer<Field> newField)
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
// Function: FieldReader::Details::parseAccess()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseAccess(QDomElement const& fieldElement, QSharedPointer<Field> newField)
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
// Function: FieldReader::Details::parseEnumeratedValues()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseEnumeratedValues(QDomElement const& fieldElement, QSharedPointer<Field> newField)
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
// Function: FieldReader::Details::parseModifiedWriteValue()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseModifiedWriteValue(QDomElement const& fieldElement, QSharedPointer<Field> newField)
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
// Function: FieldReader::Details::parseWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseWriteValueConstraint(QDomElement const& fieldElement, QSharedPointer<Field> newField)
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
// Function: FieldReader::Details::parseReadAction()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseReadAction(QDomElement const& fieldElement, QSharedPointer<Field> newField)
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
// Function: FieldReader::Details::parseTestable()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseTestable(QDomElement const& fieldElement, QSharedPointer<Field> newField)
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
// Function: FieldReader::Details::parseReserved()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseReserved(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    QDomElement reservedElement = fieldElement.firstChildElement(QStringLiteral("ipxact:reserved"));
    if (!reservedElement.isNull())
    {
        QString reserved = reservedElement.firstChild().nodeValue();
        newField->setReserved(reserved);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReader::Details::parseParameters()
//-----------------------------------------------------------------------------
void FieldReader::Details::parseParameters(QDomElement const& fieldElement, QSharedPointer<Field> newField)
{
    QDomElement parametersElement = fieldElement.firstChildElement(QStringLiteral("ipxact:parameters"));
    if (!parametersElement.isNull())
    {
        QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = CommonItemsReader::parseAndCreateParameters(fieldElement);
        for (auto parameter : *newParameters)
        {
            newField->getParameters()->append(parameter);
        }
    }
}
