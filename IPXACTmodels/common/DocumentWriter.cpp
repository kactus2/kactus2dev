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

#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/vlnv.h>

#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: DocumentWriter::DocumentWriter()
//-----------------------------------------------------------------------------
DocumentWriter::DocumentWriter(QObject *parent): QObject(parent)
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
    foreach (QString comment, document->getTopComments())
    {
        writer.writeComment(comment);
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
// Function: DocumentWriter::writeVLNVElements()
//-----------------------------------------------------------------------------
void DocumentWriter::writeVLNVElements(QXmlStreamWriter& writer, VLNV const& vlnv) const
{
    writer.writeTextElement("ipxact:vendor", vlnv.getVendor());
    writer.writeTextElement("ipxact:library", vlnv.getLibrary());
    writer.writeTextElement("ipxact:name", vlnv.getName());
    writer.writeTextElement("ipxact:version", vlnv.getVersion());
}

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeVLNVAttributes()
//-----------------------------------------------------------------------------
void DocumentWriter::writeVLNVAttributes(QXmlStreamWriter& writer, VLNV const& vlnv) const
{
    writer.writeAttribute("vendor", vlnv.getVendor());
    writer.writeAttribute("library", vlnv.getLibrary());
    writer.writeAttribute("name", vlnv.getName());
    writer.writeAttribute("version", vlnv.getVersion());
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
    if (!document->getParameters()->isEmpty())
    {
        writer.writeStartElement("ipxact:parameters");

        ParameterWriter parameterWriter;
        foreach (QSharedPointer<Parameter> parameter, *document->getParameters())
        {
            parameterWriter.writeParameter(writer, parameter);
        }
        writer.writeEndElement();
    }
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

//-----------------------------------------------------------------------------
// Function: DocumentWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void DocumentWriter::writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Document> document) const
{
    if (!document->getVendorExtensions()->isEmpty())
    {
        writer.writeStartElement("ipxact:vendorExtensions");
        foreach (QSharedPointer<VendorExtension> extension, *document->getVendorExtensions())
        {
            extension->write(writer);
        }
        writer.writeEndElement(); // ipxact:vendorExtensions
    }
}
