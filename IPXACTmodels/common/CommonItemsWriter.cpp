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
#include <IPXACTmodels/common/Qualifier.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>
#include <IPXACTmodels/common/ParameterWriter.h>

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
// Function: CommonItemsWriter::writeDisplayName()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeDisplayName(QXmlStreamWriter& writer, std::string const& displayName)
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
// Function: CommonItemsWriter::writeDescription()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeDescription(QXmlStreamWriter& writer, std::string const& description)
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
// Function: CommonItemsWriter::writeShortDescription()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeShortDescription(QXmlStreamWriter& writer, std::string const& shortDescription)
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
void CommonItemsWriter::writeIsPresent(QXmlStreamWriter& writer, std::string const& isPresent)
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
// Function: CommonItemsWriter::writeQualifier()
//-----------------------------------------------------------------------------
void CommonItemsWriter::writeQualifier(QXmlStreamWriter& writer, QSharedPointer<Qualifier> qualifier)
{
    if (!qualifier->isSet())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:qualifier"));

    if (qualifier->hasType(Qualifier::Address))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isAddress"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Data))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isData"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Clock))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isClock"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Reset))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isReset"));

        if (auto level = qualifier->getResetLevel(); !level.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("level"), level);
        }
        writer.writeCharacters(QStringLiteral("true"));
        
        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::ClockEnable))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isClockEn"));

        if (auto level = qualifier->getClockEnableLevel(); !level.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("level"), level);
        }
        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::PowerEnable))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isPowerEn"));

        if (auto const& level = qualifier->getPowerEnableLevel(); !level.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("level"), level);
        }

        if (auto const& reference = qualifier->getPowerDomainRef(); !reference.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("powerDomainRef"), reference);
        }

        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::Opcode))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isOpcode"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Protection))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isProtection"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::FlowControl))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isFlowControl"));
        
        auto const& type = qualifier->getFlowType();
        if (!type.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("flowType"), type);
        }

        if (type == QStringLiteral("user"))
        {
            if (auto const& userType = qualifier->getUserFlowType(); !userType.isEmpty())
            {
                writer.writeAttribute(QStringLiteral("user"), userType);
            }
        }

        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::User))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isUser"));

        if (auto const& userDefined = qualifier->getUserDefined();
            !userDefined.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("user"), userDefined);
        }

        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::Request))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isRequest"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Response))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isResponse"), QStringLiteral("true"));
    }

    writer.writeEndElement(); // ipxact:qualifier
}
