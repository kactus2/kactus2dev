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

#include <IPXACTmodels/common/Choice.h>
#include <IPXACTmodels/Component/ModeReference.h>
#include <IPXACTmodels/Component/FileSetRef.h>

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/PartSelect.h>
#include <IPXACTmodels/common/Qualifier.h>


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
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, Document::Revision docRevision)
{
    if (!parameters->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:parameters"));

        for (auto const& parameter : *parameters)
        {
            ParameterWriter::writeParameter(writer, parameter, docRevision);
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
// Function: CommonItemsWriter::writeAttributes()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeAttributes(QXmlStreamWriter& writer, QMap<QString, QString> const& attributes)
{
    for (auto i = attributes.cbegin(); i != attributes.cend(); ++i)
    {
        writer.writeAttribute(i.key(), i.value());
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

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeNonEmptyElement()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeNonEmptyElement(QXmlStreamWriter& writer, QString const& elementName, std::string const& value)
{
    if (value.empty() == false)
    {
        writer.writeTextElement(elementName, QString::fromStdString(value));
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeNonEmptyAttribute()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeNonEmptyAttribute(QXmlStreamWriter& writer, QString const& attributeName, QString const& value)
{
    if (!value.isEmpty())
    {
        writer.writeAttribute(attributeName, value);
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeNonEmptyAttribute()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeNonEmptyAttribute(QXmlStreamWriter& writer, QString const& attributeName, std::string const& value)
{
    if (!value.empty())
    {
        writer.writeAttribute(attributeName, QString::fromStdString(value));
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writePartSelect()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writePartSelect(QXmlStreamWriter& writer, QSharedPointer<PartSelect> partSelect)
{
    if (partSelect.isNull())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:partSelect"));

    writeRange(writer, partSelect->getRange());


    // Write all indices of the part select.
    if (partSelect->getIndices()->isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:indices"));

        for (QString const& index : *partSelect->getIndices())
        {
            writer.writeTextElement(QStringLiteral("ipxact:index"), index);
        }

        writer.writeEndElement(); // ipxact:indices
    }

    writer.writeEndElement(); // ipxact:partSelect
}


//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeRange()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeRange(QXmlStreamWriter& writer, Range const& range)
{
    if (range.getLeft().isEmpty() == false || range.getRight().isEmpty() == false)
    {
        writer.writeStartElement(QStringLiteral("ipxact:range"));
        writer.writeTextElement(QStringLiteral("ipxact:left"), range.getLeft());
        writer.writeTextElement(QStringLiteral("ipxact:right"), range.getRight());
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsWriter::writeModeReferences()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeModeReferences(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeReferences)
{
    if (!modeReferences || modeReferences->isEmpty())
    {
        return;
    }

    for (auto const& modeRef : *modeReferences)
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
// Function: CommonItemsWriter::writeFileSetReferences()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeFileSetReferences(QXmlStreamWriter& writer, 
    QSharedPointer<QList<QSharedPointer<FileSetRef> > > fileSetRefs, Document::Revision docRevision)
{
    for (auto const& fileSetRef : *fileSetRefs)
    {
        writer.writeStartElement(QStringLiteral("ipxact:fileSetRef"));

        writer.writeTextElement(QStringLiteral("ipxact:localName"), fileSetRef->getReference());

        if (docRevision == Document::Revision::Std14)
        {
            CommonItemsWriter::writeIsPresent(writer, fileSetRef->getIsPresent());
        }
        else if (docRevision == Document::Revision::Std22)
        {
            CommonItemsWriter::writeVendorExtensions(writer, fileSetRef);
        }

        writer.writeEndElement(); // ipxact:fileSetRef
    }
}
