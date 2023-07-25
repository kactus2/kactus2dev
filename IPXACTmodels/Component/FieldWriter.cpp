//-----------------------------------------------------------------------------
// File: FieldWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Writer for IP-XACT field element.
//-----------------------------------------------------------------------------

#include "FieldWriter.h"
#include "Field.h"
#include "WriteValueConstraint.h"
#include "MemoryArrayWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/Component/EnumeratedValueWriter.h>
#include <IPXACTmodels/Component/FieldReset.h>

//-----------------------------------------------------------------------------
// Function: FieldWriter::writeField()
//-----------------------------------------------------------------------------
void FieldWriter::writeField(QXmlStreamWriter& writer, QSharedPointer<Field> field, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:field"));

    Details::writeID(writer, field->getId(), docRevision);

    NameGroupWriter::writeNameGroup(writer, field, docRevision);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, field->getIsPresent());
    }
    
    Details::writeMemoryArray(writer, field, docRevision);
    
    writer.writeTextElement(QStringLiteral("ipxact:bitOffset"), field->getBitOffset());

    if (docRevision == Document::Revision::Std14)
    {
        Details::writeFieldData2014(writer, field);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        Details::writeFieldData2022(writer, field);
    }

    CommonItemsWriter::writeParameters(writer, field->getParameters(), docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, field);

    writer.writeEndElement(); // ipxact:field
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeID()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeID(QXmlStreamWriter& writer, QString const& fieldID,
    Document::Revision docRevision)
{
    if (!fieldID.isEmpty() && docRevision == Document::Revision::Std14)
    {
        writer.writeAttribute(QStringLiteral("fieldID"), fieldID);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeMemoryArray()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeMemoryArray(QXmlStreamWriter& writer, QSharedPointer<Field> field,
    Document::Revision docRevision)
{
    if (auto memArray = field->getMemoryArray();
        memArray && docRevision == Document::Revision::Std22)
    {
        MemoryArrayWriter::writeMemoryArray(writer, memArray, true);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeFieldDefinitionRef()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeFieldDefinitionRef(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (auto const& fieldDefRef = field->getFieldDefinitionRef(); !fieldDefRef.isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:fieldDefinitionRef"));

        writer.writeAttribute(QStringLiteral("typeDefinitions"), field->getTypeDefinitionsRef());

        writer.writeCharacters(field->getFieldDefinitionRef());

        writer.writeEndElement(); // ixpact:fieldDefinitionRef
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeResets()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeResets(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if(field->getResets() && !field->getResets()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:resets"));
        
        foreach (QSharedPointer<FieldReset> reset, *(field->getResets()))
        {
            if (reset && (!reset->getResetTypeReference().isEmpty() || !reset->getResetValue().isEmpty() ||
                !reset->getResetMask().isEmpty()))
            {
                writer.writeStartElement(QStringLiteral("ipxact:reset"));
                
                if (!reset->getResetTypeReference().isEmpty())
                {
                    writer.writeAttribute(QStringLiteral("resetTypeRef"), reset->getResetTypeReference());
                }
              
                writer.writeTextElement(QStringLiteral("ipxact:value"), reset->getResetValue());
              
                if (!reset->getResetMask().isEmpty())
                {
                    writer.writeTextElement(QStringLiteral("ipxact:mask"), reset->getResetMask());
                }
              
                writer.writeEndElement(); // ipxact:reset
            }
        }

        writer.writeEndElement(); // ipxact:resets
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeTypeIdentifier()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (!field->getTypeIdentifier().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:typeIdentifier"), field->getTypeIdentifier());
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeVolatile()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeVolatile(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (!field->getVolatile().toString().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:volatile"), field->getVolatile().toString());
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeAccess()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeAccess(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (field->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString access = AccessTypes::access2Str(field->getAccess());
        writer.writeTextElement(QStringLiteral("ipxact:access"), access);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeEnumerations()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeEnumerations(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (!field->getEnumeratedValues()->isEmpty())
    {
        EnumeratedValueWriter enumerationWriter;
        writer.writeStartElement(QStringLiteral("ipxact:enumeratedValues"));

        foreach (QSharedPointer<EnumeratedValue> enumeration, *field->getEnumeratedValues())
        {
            enumerationWriter.writeEnumeratedValue(writer, enumeration);
        }

        writer.writeEndElement(); // ipxact:enumeratedValues
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeModifiedWriteValue()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeModifiedWriteValue(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (field->getModifiedWrite() != General::MODIFIED_WRITE_COUNT)
    {
        writer.writeStartElement(QStringLiteral("ipxact:modifiedWriteValue"));

        if (!field->getModifiedWriteModify().isEmpty())
        {
            writer.writeAttribute(QStringLiteral("modify"), field->getModifiedWriteModify());
        }

        QString modifiedWriteValue = General::modifiedWrite2Str(field->getModifiedWrite());
        writer.writeCharacters(modifiedWriteValue);

        writer.writeEndElement(); // ipxact:modifiedWriteValue
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeWriteValueConstraint(QXmlStreamWriter& writer,
    QSharedPointer<WriteValueConstraint> writeConstraint)
{
    if (!writeConstraint.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:writeValueConstraint"));

        if (writeConstraint->getType() == WriteValueConstraint::WRITE_AS_READ)
        {
            writer.writeTextElement(QStringLiteral("ipxact:writeAsRead"), QStringLiteral("true"));
        }
        else if (writeConstraint->getType() == WriteValueConstraint::USE_ENUM)
        {
            writer.writeTextElement(QStringLiteral("ipxact:useEnumeratedValues"), QStringLiteral("true"));
        }
        else if (writeConstraint->getType() == WriteValueConstraint::MIN_MAX)
        {
            writer.writeTextElement(QStringLiteral("ipxact:minimum"), writeConstraint->getMinimum());
            writer.writeTextElement(QStringLiteral("ipxact:maximum"), writeConstraint->getMaximum());
        }

        writer.writeEndElement(); // ipxact:writeValueConstraint
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeReadAction()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeReadAction(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (field->getReadAction() != General::READ_ACTION_COUNT)
    {
        QString readAction = General::readAction2Str(field->getReadAction());

        writer.writeStartElement(QStringLiteral("ipxact:readAction"));

        if (!field->getReadActionModify().isEmpty())
        {
            writer.writeAttribute(QStringLiteral("modify"), field->getReadActionModify());
        }
        writer.writeCharacters(readAction);

        writer.writeEndElement(); // ipxact:readAction
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeTestable()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeTestable(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (!field->getTestable().toString().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:testable"));

        if (field->getTestConstraint() != General::TESTCONSTRAINT_COUNT)
        {
            QString testConstraint = General::testConstraint2Str(field->getTestConstraint());
            writer.writeAttribute(QStringLiteral("testConstraint"), testConstraint);
        }

        writer.writeCharacters(field->getTestable().toString());

        writer.writeEndElement(); // ipxact:testable
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeReserved()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeReserved(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    if (!field->getReserved().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:reserved"), field->getReserved());
    }
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeFieldData2014()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeFieldData2014(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    Details::writeResets(writer, field);

    Details::writeTypeIdentifier(writer, field);

    writer.writeTextElement(QStringLiteral("ipxact:bitWidth"), field->getBitWidth());

    Details::writeVolatile(writer, field);

    Details::writeAccess(writer, field);

    Details::writeEnumerations(writer, field);

    Details::writeModifiedWriteValue(writer, field);

    Details::writeWriteValueConstraint(writer, field->getWriteConstraint());

    Details::writeReadAction(writer, field);

    Details::writeTestable(writer, field);

    Details::writeReserved(writer, field);
}

//-----------------------------------------------------------------------------
// Function: FieldWriter::Details::writeFieldData2022()
//-----------------------------------------------------------------------------
void FieldWriter::Details::writeFieldData2022(QXmlStreamWriter& writer, QSharedPointer<Field> field)
{
    Details::writeFieldDefinitionRef(writer, field);

    Details::writeTypeIdentifier(writer, field);

    if (auto const& bitWidth = field->getBitWidth(); !bitWidth.isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:bitWidth"), bitWidth);
    }

    // TODO: write field access policy

    Details::writeVolatile(writer, field);

    Details::writeResets(writer, field);

    // TODO: writeAliasOf

    Details::writeAccess(writer, field);

    Details::writeModifiedWriteValue(writer, field);

    Details::writeWriteValueConstraint(writer, field->getWriteConstraint());

    Details::writeReadAction(writer, field);

    // TODO: writeReadResponse

    // TODO: writeBroadcasts

    // TODO: writeAccessRestrictions

    Details::writeTestable(writer, field);

    Details::writeReserved(writer, field);

    // TODO: writeVendorExtensions

    Details::writeEnumerations(writer, field);
}
