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
#include <IPXACTmodels/common/ChoiceWriter.h>

#include <IPXACTmodels/Component/Choice.h>

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::CommonItemsWriter()
//-----------------------------------------------------------------------------
CommonItemsWriter::CommonItemsWriter()
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeVLNVElements()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeVLNVElements(QXmlStreamWriter& writer, VLNV const& vlnv)
{
    writer.writeTextElement(QStringLiteral("ipxact:vendor"), vlnv.getVendor());
    writer.writeTextElement(QStringLiteral("ipxact:library"), vlnv.getLibrary());
    writer.writeTextElement(QStringLiteral("ipxact:name"), vlnv.getName());
    writer.writeTextElement(QStringLiteral("ipxact:version"), vlnv.getVersion());
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeVLNVAttributes()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeVLNVAttributes(QXmlStreamWriter& writer, VLNV const& vlnv)
{
    writer.writeAttribute(QStringLiteral("vendor"), vlnv.getVendor());
    writer.writeAttribute(QStringLiteral("library"), vlnv.getLibrary());
    writer.writeAttribute(QStringLiteral("name"), vlnv.getName());
    writer.writeAttribute(QStringLiteral("version"), vlnv.getVersion());
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeDisplayName()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeDisplayName(QXmlStreamWriter& writer, QString const& displayName)
{
    writeNonEmptyElement(writer, QStringLiteral("ipxact:displayName"), displayName);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeDescription()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeDescription(QXmlStreamWriter& writer, QString const& description)
{
    writeNonEmptyElement(writer, QStringLiteral("ipxact:description"), description);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeShortDescription()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeShortDescription(QXmlStreamWriter& writer, QString const& shortDescription)
{
    writeNonEmptyElement(writer, QStringLiteral("ipxact:shortDescription"), shortDescription);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeChoices()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeChoices(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<Choice> > > choices)
{
    if (choices->isEmpty())
    {
        return;
    }
    writer.writeStartElement(QStringLiteral("ipxact:choices"));

    for (auto const& choice : *choices)
    {
        ChoiceWriter::writeChoice(writer, choice);
    }

    writer.writeEndElement(); // ipxact:choices
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeChoices()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeParameters(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters)
{
    if (!parameters->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:parameters"));

        ParameterWriter parameterWriter;
        for (auto const& parameter : *parameters)
        {
            parameterWriter.writeParameter(writer, parameter);
        }

        writer.writeEndElement(); // ipxact:parameters
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Extendable> element)
{
    if (!element->getVendorExtensions()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:vendorExtensions"));
        for (auto const& extension : *element->getVendorExtensions())
        {
            extension->write(writer);
        }
        writer.writeEndElement(); // ipxact:vendorExtensions
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeIsPresent(QXmlStreamWriter& writer, QString const& isPresent)
{
    writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), isPresent);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeIsPresent(QXmlStreamWriter& writer, QString const& isPresent)
{
    writeNonEmptyElement(writer, QStringLiteral("ipxact:isPresent"), isPresent);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeConfigurableElementValues()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeConfigurableElementValues(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements)
{
    if (!configurableElements->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:configurableElementValues"));

        for (auto const& element : *configurableElements)
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

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeNonEmptyElement()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeNonEmptyElement(QXmlStreamWriter& writer, QString const& elementName, QString const& value)
{
    if (!value.isEmpty())
    {
        writer.writeTextElement(elementName, value);
    }
}

