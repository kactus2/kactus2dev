//-----------------------------------------------------------------------------
// File: MemoryArrayWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.7.2023
//
// Description:
// Writer for ipxact:array memory array elements.
//-----------------------------------------------------------------------------

#include "MemoryArrayWriter.h"
#include "MemoryArray.h"

//-----------------------------------------------------------------------------
// Function: MemoryArrayWriter::writeMemoryArray()
//-----------------------------------------------------------------------------
void MemoryArrayWriter::writeMemoryArray(QXmlStreamWriter& writer, QSharedPointer<MemoryArray> memoryArray, Document::Revision docRevision, bool isField)
{
    if (!memoryArray)
    {
        return;
    }

    if (docRevision == Document::Revision::Std14)
    {
        Details::writeDimensions(writer, memoryArray, docRevision);
    }
    else if (docRevision == Document::Revision::Std22)
    {
        writer.writeStartElement(QStringLiteral("ipxact:array"));

        Details::writeDimensions(writer, memoryArray, docRevision);

        Details::writeStride(writer, memoryArray, isField);

        writer.writeEndElement(); // ipxact:array
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayWriter::Details::writeDimensions()
//-----------------------------------------------------------------------------
void MemoryArrayWriter::Details::writeDimensions(QXmlStreamWriter& writer, QSharedPointer<MemoryArray> memoryArray, 
    Document::Revision docRevision)
{
    for (auto const& dimension : *memoryArray->getDimensions())
    {
        writer.writeStartElement(QStringLiteral("ipxact:dim"));

        if (dimension->indexVar_.isEmpty() == false && docRevision == Document::Revision::Std22)
        {
            writer.writeAttribute(QStringLiteral("indexVar"), dimension->indexVar_);
        }

        writer.writeCharacters(dimension->value_);
        writer.writeEndElement(); // ipxact:dim
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryArrayWriter::Details::writeStride()
//-----------------------------------------------------------------------------
void MemoryArrayWriter::Details::writeStride(QXmlStreamWriter& writer, QSharedPointer<MemoryArray> memoryArray, bool isField)
{
    QString elementName = isField ? QStringLiteral("ipxact:bitStride") : QStringLiteral("ipxact:stride");

    if (auto const& stride = memoryArray->getStride(); stride.isEmpty() == false)
    {
        writer.writeTextElement(elementName, stride);
    }
}
