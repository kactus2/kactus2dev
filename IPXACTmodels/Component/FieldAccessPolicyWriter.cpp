//-----------------------------------------------------------------------------
// File: FieldAccessPolicyWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 2.8.2023
//
// Description:
// Writer for ipxact:fieldAccessPolicy.
//-----------------------------------------------------------------------------

#include "FieldAccessPolicyWriter.h"
#include "FieldReferenceWriter.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::writeFieldAccessPolicy()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::writeFieldAccessPolicy(QXmlStreamWriter& writer,
    QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    writer.writeStartElement(QStringLiteral("ipxact:fieldAccessPolicy"));

    Details::writeModeRefs(writer, fieldAccessPolicy->getModeReferences());

    Details::writeFieldAccessPolicyDefinitionRef(writer, fieldAccessPolicy);

    Details::writeAccess(writer, fieldAccessPolicy);

    Details::writeValueConstraint(writer, fieldAccessPolicy);

    Details::writeModifiedWriteValue(writer, fieldAccessPolicy);

    Details::writeReadAction(writer, fieldAccessPolicy);

    Details::writeReadResponse(writer, fieldAccessPolicy);

    Details::writeBroadCasts(writer, fieldAccessPolicy);

    Details::writeAccessRestrictions(writer, fieldAccessPolicy);

    Details::writeTestable(writer, fieldAccessPolicy);

    Details::writeReserved(writer, fieldAccessPolicy);

    CommonItemsWriter::writeVendorExtensions(writer, fieldAccessPolicy);

    writer.writeEndElement(); // ipxact:fieldAccessPolicy
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeModeRefs()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeModeRefs(QXmlStreamWriter& writer, 
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs)
{
    if (modeRefs->isEmpty())
    {
        return;
    }

    for (auto const& modeRef : *modeRefs)
    {
        writer.writeStartElement(QStringLiteral("ipxact:modeRef"));

        if (auto const& priority = modeRef->getPriority(); !priority.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("priority"), priority);
        }

        writer.writeCharacters(modeRef->getReference());
        writer.writeEndElement(); // ipxact:modeRef
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeFieldAccessPolicyDefinitionRef()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeFieldAccessPolicyDefinitionRef(QXmlStreamWriter& writer,
    QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    if (auto const& definitionRef = fieldAccessPolicy->getFieldAccessPolicyDefinitionRef();
        !definitionRef.isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:fieldAccessPolicyDefinitionRef"));

        if (auto const& typeDefs = fieldAccessPolicy->getFieldAccessPolicyTypeDefinitionRef();
            !typeDefs.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("typeDefinitions"), typeDefs);
        }

        writer.writeCharacters(definitionRef);
        writer.writeEndElement(); // ipxact:fieldAccessPolicyDefinitionRef
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeAccess()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeAccess(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    if (fieldAccessPolicy->getAccess() != AccessTypes::ACCESS_COUNT)
    {
        QString access = AccessTypes::access2Str(fieldAccessPolicy->getAccess());
        writer.writeTextElement(QStringLiteral("ipxact:access"), access);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeModifiedWriteValue()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeModifiedWriteValue(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    if (fieldAccessPolicy->getModifiedWrite() != General::MODIFIED_WRITE_COUNT)
    {
        writer.writeStartElement(QStringLiteral("ipxact:modifiedWriteValue"));

        if (!fieldAccessPolicy->getModifiedWriteModify().isEmpty())
        {
            writer.writeAttribute(QStringLiteral("modify"), fieldAccessPolicy->getModifiedWriteModify());
        }

        QString modifiedWriteValue = General::modifiedWrite2Str(fieldAccessPolicy->getModifiedWrite());
        writer.writeCharacters(modifiedWriteValue);

        writer.writeEndElement(); // ipxact:modifiedWriteValue
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeValueConstraint()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeValueConstraint(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    auto writeConstraint = fieldAccessPolicy->getWriteValueConstraint();

    if (!writeConstraint.isNull())
    {
        auto constraintType = writeConstraint->getType();

        if (constraintType == WriteValueConstraint::TYPE_COUNT)
        {
            return;
        }

        writer.writeStartElement(QStringLiteral("ipxact:writeValueConstraint"));

        if (constraintType == WriteValueConstraint::WRITE_AS_READ)
        {
            writer.writeTextElement(QStringLiteral("ipxact:writeAsRead"), QStringLiteral("true"));
        }
        else if (constraintType == WriteValueConstraint::USE_ENUM)
        {
            writer.writeTextElement(QStringLiteral("ipxact:useEnumeratedValues"), QStringLiteral("true"));
        }
        else if (constraintType == WriteValueConstraint::MIN_MAX)
        {
            writer.writeTextElement(QStringLiteral("ipxact:minimum"), writeConstraint->getMinimum());
            writer.writeTextElement(QStringLiteral("ipxact:maximum"), writeConstraint->getMaximum());
        }

        writer.writeEndElement(); // ipxact:writeValueConstraint
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeReadAction()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeReadAction(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    if (fieldAccessPolicy->getReadAction() != General::READ_ACTION_COUNT)
    {
        QString readAction = General::readAction2Str(fieldAccessPolicy->getReadAction());

        writer.writeStartElement(QStringLiteral("ipxact:readAction"));

        if (!fieldAccessPolicy->getReadActionModify().isEmpty())
        {
            writer.writeAttribute(QStringLiteral("modify"), fieldAccessPolicy->getReadActionModify());
        }
        writer.writeCharacters(readAction);

        writer.writeEndElement(); // ipxact:readAction
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeReadResponse()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeReadResponse(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    if (auto const& readResponse = fieldAccessPolicy->getReadResponse();
        readResponse.isEmpty() == false)
    {
        writer.writeTextElement(QStringLiteral("ipxact:readResponse"), readResponse);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeBroadCasts()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeBroadCasts(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    if (auto const& broadcasts = fieldAccessPolicy->getBroadcasts();
        broadcasts->isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:broadcasts"));

        for (auto const& fieldReference : *broadcasts)
        {
            writer.writeStartElement(QStringLiteral("ipxact:broadcastTo"));

            FieldReferenceWriter::writeFieldReference(writer, fieldReference);

            writer.writeEndElement(); // ipxact:broadcastTo
        }

        writer.writeEndElement(); // ipxact:broadcasts
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeAccessRestrictions()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeAccessRestrictions(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    auto accessRestrictions = fieldAccessPolicy->getAccessRestrictions();

    if (accessRestrictions->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:accessRestrictions"));

    for (auto const& accessRestriction : *accessRestrictions)
    {
        writer.writeStartElement(QStringLiteral("ipxact:accessRestriction"));

        writeModeRefs(writer, accessRestriction->modeRefs_);

        if (!accessRestriction->readAccessMask_.isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:readAccessMask"), accessRestriction->readAccessMask_);
        }

        if (!accessRestriction->writeAccessMask_.isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:writeAccessMask"), accessRestriction->writeAccessMask_);
        }

        writer.writeEndElement(); // ipxact:accessRestriction
    }

    writer.writeEndElement(); // ipxact:accessRestrictions
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeTestable()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeTestable(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    if (!fieldAccessPolicy->getTestable().toString().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:testable"));

        if (fieldAccessPolicy->getTestConstraint() != General::TESTCONSTRAINT_COUNT)
        {
            QString testConstraint = General::testConstraint2Str(fieldAccessPolicy->getTestConstraint());
            writer.writeAttribute(QStringLiteral("testConstraint"), testConstraint);
        }

        writer.writeCharacters(fieldAccessPolicy->getTestable().toString());

        writer.writeEndElement(); // ipxact:testable
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyWriter::Details::writeReserved()
//-----------------------------------------------------------------------------
void FieldAccessPolicyWriter::Details::writeReserved(QXmlStreamWriter& writer, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy)
{
    if (!fieldAccessPolicy->getReserved().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:reserved"), fieldAccessPolicy->getReserved());
    }
}
