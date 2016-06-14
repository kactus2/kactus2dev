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
DocumentWriter::DocumentWriter(QObject *parent):
CommonItemsWriter(parent)
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
    foreach (QString const& comment, document->getTopComments())
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
    
    int instructionCount = instructions.count();
    for (int i = 0; i < instructionCount; i++)
    {
        writer.writeProcessingInstruction(instructions.at(i).first, instructions.at(i).second);
    }   
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeNamespaceDeclarations()
//-----------------------------------------------------------------------------
void DocumentWriter::writeNamespaceDeclarations(QXmlStreamWriter& writer) const
{
    writer.writeNamespace("http://www.w3.org/2001/XMLSchema-instance", "xsi");
    writer.writeNamespace("http://www.accellera.org/XMLSchema/IPXACT/1685-2014", "ipxact");
    writer.writeNamespace("http://kactus2.cs.tut.fi", "kactus2");
    writer.writeAttribute("xsi:schemaLocation", "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd");
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeDescription()
//-----------------------------------------------------------------------------
void DocumentWriter::writeDescription(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    if (!document->getDescription().isEmpty())
    {
        writer.writeTextElement("ipxact:description", document->getDescription());
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
        writer.writeStartElement("ipxact:assertions");

        NameGroupWriter nameGroupWriter;
        foreach (QSharedPointer<Assertion> assertion, *document->getAssertions())
        {
            writer.writeStartElement("ipxact:assertion");

            nameGroupWriter.writeNameGroup(writer, assertion);
            writer.writeTextElement("ipxact:assert", assertion->getAssert());

            writer.writeEndElement(); // ipxact:assertion
        }

        writer.writeEndElement(); // ipxact:assertions
    }
}
