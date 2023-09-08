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
// Function: DocumentWriter::writeTopComments()
//-----------------------------------------------------------------------------
void DocumentWriter::writeTopComments(QXmlStreamWriter& writer, QSharedPointer<Document> document) 
{
    for (QString const& comment : document->getTopComments())
    {
        writer.writeComment(comment);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeXmlProcessingInstructions()
//-----------------------------------------------------------------------------
void DocumentWriter::writeXmlProcessingInstructions(QXmlStreamWriter& writer, QSharedPointer<Document> document) 
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
void DocumentWriter::writeNamespaceDeclarations(QXmlStreamWriter& writer, QSharedPointer<Document> document) 
{
    QVector<QPair<QString, QString> > nameSpaces = document->getXmlNameSpaces();

    for (auto const& [name, uri] : nameSpaces)
    {
        writer.writeNamespace(uri, name);
    }

    // Also write the schema location.
    if (auto revision = document->getRevision(); revision != Document::Revision::Unknown)
    {
        QString schemaLocation = QStringLiteral("http://www.accellera.org/XMLSchema/IPXACT/%1 http://www.accellera.org/XMLSchema/IPXACT/%1/index.xsd")
            .arg(Document::toString(revision));

        writer.writeAttribute(QStringLiteral("xsi:schemaLocation"), schemaLocation);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeDocumentNameGroup()
//-----------------------------------------------------------------------------
void DocumentWriter::writeDocumentNameGroup(QXmlStreamWriter& writer, QSharedPointer<Document> document)
{
    CommonItemsWriter::writeVLNVElements(writer, document->getVlnv());

    if (auto revision = document->getRevision();
        revision == Document::Revision::Std22)
    {
        CommonItemsWriter::writeDisplayName(writer, document->getDisplayName());

        CommonItemsWriter::writeShortDescription(writer, document->getShortDescription());
        
        CommonItemsWriter::writeDescription(writer, document->getDescription());
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeParameters()
//-----------------------------------------------------------------------------
void DocumentWriter::writeParameters(QXmlStreamWriter& writer, QSharedPointer<Document> document) 
{
    CommonItemsWriter::writeParameters(writer, document->getParameters(), document->getRevision());
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeAssertions()
//-----------------------------------------------------------------------------
void DocumentWriter::writeAssertions(QXmlStreamWriter& writer, QSharedPointer<Document> document) 
{
    if (!document->getAssertions()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:assertions"));

        for (auto const& assertion : *document->getAssertions())
        {
            writer.writeStartElement(QStringLiteral("ipxact:assertion"));

            NameGroupWriter::writeNameGroup(writer, assertion);
            writer.writeTextElement(QStringLiteral("ipxact:assert"), assertion->getAssert());

            writer.writeEndElement(); // ipxact:assertion
        }

        writer.writeEndElement(); // ipxact:assertions
    }
}
