//-----------------------------------------------------------------------------
// File: FieldReferenceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 1.8.2023
//
// Description:
// Writer for the fieldReferenceGroup group.
//-----------------------------------------------------------------------------

#include "FieldReferenceWriter.h"

//-----------------------------------------------------------------------------
// Function: FieldReferenceWriter::writeFieldReference()
//-----------------------------------------------------------------------------
void FieldReferenceWriter::writeFieldReference(QXmlStreamWriter& writer, 
    QSharedPointer<FieldReference> fieldReference)
{
    if (!fieldReference)
    {
        return;
    }

    for (auto i = 0; i < FieldReference::Type::REFERENCE_TYPE_COUNT; ++i)
    {
        if (i == FieldReference::BANK || i == FieldReference::REGISTER_FILE)
        {
            Details::writeMultipleFieldReference(writer, fieldReference, static_cast<FieldReference::Type>(i));
        }
        else
        {
            if (auto reference = fieldReference->getReference(static_cast<FieldReference::Type>(i));
                reference != nullptr)
            {
                Details::writeSingleFieldReference(writer, reference, static_cast<FieldReference::Type>(i));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReferenceWriter::Details::writeSingleFieldReference()
//-----------------------------------------------------------------------------
void FieldReferenceWriter::Details::writeSingleFieldReference(QXmlStreamWriter& writer, 
    QSharedPointer<FieldReference::IndexedReference> referenceElement, FieldReference::Type refType)
{
    if (referenceElement)
    {
        auto typeAsString = FieldReference::type2Str(refType);
        auto elementName = QStringLiteral("ipxact:") + typeAsString;

        writer.writeStartElement(elementName); // ipxact:____Ref
        writer.writeAttribute(typeAsString, referenceElement->reference_);

        if (!referenceElement->indices_.isEmpty())
        {
            Details::writeFieldReferenceIndices(writer, referenceElement);
        }

        writer.writeEndElement(); // ipxact:____Ref
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReferenceWriter::Details::writeMultipleFieldReference()
//-----------------------------------------------------------------------------
void FieldReferenceWriter::Details::writeMultipleFieldReference(QXmlStreamWriter& writer, 
    QSharedPointer<FieldReference> fieldReference, FieldReference::Type refType)
{
    auto references = fieldReference->getMultipleReference(refType);

    if (!references->isEmpty())
    {
        for (auto const& reference : *references)
        {
            writeSingleFieldReference(writer, reference, refType);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldReferenceWriter::Details::writeFieldReferenceIndices()
//-----------------------------------------------------------------------------
void FieldReferenceWriter::Details::writeFieldReferenceIndices(QXmlStreamWriter& writer, 
    QSharedPointer<FieldReference::IndexedReference> referenceElement)
{
    writer.writeStartElement(QStringLiteral("ipxact:indices"));

    for (auto const& index : referenceElement->indices_)
    {
        writer.writeTextElement(QStringLiteral("ipxact:index"), index);
    }

    writer.writeEndElement(); // ipxact:indices
}
