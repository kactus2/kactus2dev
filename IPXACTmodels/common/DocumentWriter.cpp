//-----------------------------------------------------------------------------
// File: DocumentWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 06.08.2015
//
// Description:
// Base class for IP-XACT document writers.
//-----------------------------------------------------------------------------

#include "DocumentWriter.h"
#include "Document.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <QPair>

//-----------------------------------------------------------------------------
// Function: DocumentWriter::DocumentWriter()
//-----------------------------------------------------------------------------
DocumentWriter::DocumentWriter() : CommonItemsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::~DocumentWriter()
//-----------------------------------------------------------------------------
DocumentWriter::~DocumentWriter()
{

}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeTopComments()
//-----------------------------------------------------------------------------
void DocumentWriter::writeTopComments(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    for (auto const& comment : document->getTopComments())
    {
        writer.writeComment(comment);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeXmlProcessingInstructions()
//-----------------------------------------------------------------------------
void DocumentWriter::writeXmlProcessingInstructions(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    QVector<QPair<QString, QString> > instructions = document->getXmlProcessingInstructions();

    for (auto const& [target, data] : instructions)
    {
        writer.writeProcessingInstruction(target, data);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeNamespaceDeclarations()
//-----------------------------------------------------------------------------
void DocumentWriter::writeNamespaceDeclarations(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    QVector<QPair<QString, QString> > nameSpaces = document->getXmlNameSpaces();

    for (auto const& [name, uri] : nameSpaces)
    {
        writer.writeNamespace(uri, name);
    }

    // Also write the schema location.
    writer.writeAttribute(QStringLiteral("xsi:schemaLocation"), document->getSchemaLocation());
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeDocumentNameGroup()
//-----------------------------------------------------------------------------
void DocumentWriter::writeDocumentNameGroup(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    writeVLNVElements(writer, document->getVlnv());

    if (auto revision = document->getRevision();
        revision != Document::Revision::Std14 &&
        revision != Document::Revision::Unknown)
    {
        if (auto const& displayName = document->getDisplayName(); !displayName.isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:displayName"), displayName);
        }

        if (auto const& shortDescription = document->getShortDescription(); !shortDescription.isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:shortDescription"), shortDescription);
        }
        
        if (auto const& description = document->getDescription(); !description.isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:description"), description);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeDescription()
//-----------------------------------------------------------------------------
void DocumentWriter::writeDescription(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    if (document->getRevision() == Document::Revision::Std14 &&
        !document->getDescription().isEmpty())    
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), document->getDescription());
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeParameters()
//-----------------------------------------------------------------------------
void DocumentWriter::writeParameters(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    CommonItemsWriter::writeParameters(writer, document->getParameters());
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeAssertions()
//-----------------------------------------------------------------------------
void DocumentWriter::writeAssertions(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    if (!document->getAssertions()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:assertions"));

        NameGroupWriter nameGroupWriter;
        for (auto const& assertion : *document->getAssertions())
        {
            writer.writeStartElement(QStringLiteral("ipxact:assertion"));

            nameGroupWriter.writeNameGroup(writer, assertion);
            writer.writeTextElement(QStringLiteral("ipxact:assert"), assertion->getAssert());

            writer.writeEndElement(); // ipxact:assertion
        }

        writer.writeEndElement(); // ipxact:assertions
    }
}
