//-----------------------------------------------------------------------------
// File: CommonItemsWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Writer class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------

#include "CommonItemsWriter.h"
#include "Extendable.h"

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::CommonItemsWriter()
//-----------------------------------------------------------------------------
CommonItemsWriter::CommonItemsWriter(QObject* parent /* = 0 */) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::~CommonItemsWriter()
//-----------------------------------------------------------------------------
CommonItemsWriter::~CommonItemsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeVLNVElements()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeVLNVElements(QXmlStreamWriter& writer, VLNV const& vlnv) const
{
    writer.writeTextElement(QStringLiteral("ipxact:vendor"), vlnv.getVendor());
    writer.writeTextElement(QStringLiteral("ipxact:library"), vlnv.getLibrary());
    writer.writeTextElement(QStringLiteral("ipxact:name"), vlnv.getName());
    writer.writeTextElement(QStringLiteral("ipxact:version"), vlnv.getVersion());
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeVLNVAttributes()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeVLNVAttributes(QXmlStreamWriter& writer, VLNV const& vlnv) const
{
    writer.writeAttribute(QStringLiteral("vendor"), vlnv.getVendor());
    writer.writeAttribute(QStringLiteral("library"), vlnv.getLibrary());
    writer.writeAttribute(QStringLiteral("name"), vlnv.getName());
    writer.writeAttribute(QStringLiteral("version"), vlnv.getVersion());
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeParameters()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeParameters(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters) const
{
    if (!parameters->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:parameters"));

        ParameterWriter parameterWriter;
        foreach (QSharedPointer<Parameter> parameter, *parameters)
        {
            parameterWriter.writeParameter(writer, parameter);
        }

        writer.writeEndElement(); // ipxact:parameters
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Extendable> element) const
{
    if (!element->getVendorExtensions()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:vendorExtensions"));
        foreach (QSharedPointer<VendorExtension> extension, *element->getVendorExtensions())
        {
            extension->write(writer);
        }
        writer.writeEndElement(); // ipxact:vendorExtensions
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeIsPresent(QXmlStreamWriter& writer, QString const& isPresent) const
{
    if (!isPresent.isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:isPresent"), isPresent);
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeConfigurableElementValues()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeConfigurableElementValues(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements) const
{
    if (!configurableElements->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:configurableElementValues"));

        foreach (QSharedPointer<ConfigurableElementValue> element, *configurableElements)
        {
            writer.writeStartElement(QStringLiteral("ipxact:configurableElementValue"));

            QMapIterator<QString, QString> attributeIterator(element->getAttributes());
            while (attributeIterator.hasNext())
            {
                attributeIterator.next();
                writer.writeAttribute(attributeIterator.key(), attributeIterator.value());
            }

            writer.writeCharacters(element->getConfigurableValue());

            writer.writeEndElement(); // ipxact:configurableElementValue
        }

        writer.writeEndElement(); // ipxact:configurableElementValues
    }
}