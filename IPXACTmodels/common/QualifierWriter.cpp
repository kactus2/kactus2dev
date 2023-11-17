//-----------------------------------------------------------------------------
// File: QualifierWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.11.2023
//
// Description:
// XML writer for ipxact:qualifier element.
//-----------------------------------------------------------------------------

#include "QualifierWriter.h"
#include "Qualifier.h"

//-----------------------------------------------------------------------------
// Function: QualifierWriter::writeQualifier()
//-----------------------------------------------------------------------------
void QualifierWriter::writeQualifier(QXmlStreamWriter& writer, QSharedPointer<Qualifier> qualifier)
{
    if (!qualifier->isSet())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:qualifier"));

    if (qualifier->hasType(Qualifier::Type::Address))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isAddress"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Type::Data))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isData"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Type::Clock))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isClock"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Type::Reset))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isReset"));

        if (auto level = qualifier->getAttribute(Qualifier::Attribute::ResetLevel); !level.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("level"), level);
        }
        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::Type::Valid))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isValid"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Type::Interrupt))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isInterrupt"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Type::ClockEnable))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isClockEn"));

        if (auto level = qualifier->getAttribute(Qualifier::Attribute::ClockEnableLevel); !level.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("level"), level);
        }
        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::Type::PowerEnable))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isPowerEn"));

        if (auto level = qualifier->getAttribute(Qualifier::Attribute::PowerEnableLevel); !level.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("level"), level);
        }

        if (auto const& reference = qualifier->getAttribute(Qualifier::Attribute::PowerDomainReference);
            !reference.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("powerDomainRef"), reference);
        }

        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::Type::Opcode))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isOpcode"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Type::Protection))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isProtection"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Type::FlowControl))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isFlowControl"));

        auto const& type = qualifier->getAttribute(Qualifier::Attribute::FlowType);
        if (!type.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("flowType"), type);
        }

        if (type == QStringLiteral("user"))
        {
            if (auto const& userType = qualifier->getAttribute(Qualifier::Attribute::UserFlowType);
                !userType.isEmpty())
            {
                writer.writeAttribute(QStringLiteral("user"), userType);
            }
        }

        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::Type::User))
    {
        writer.writeStartElement(QStringLiteral("ipxact:isUser"));

        if (auto const& userDefined = qualifier->getAttribute(Qualifier::Attribute::UserDefined);
            !userDefined.isEmpty())
        {
            writer.writeAttribute(QStringLiteral("user"), userDefined);
        }

        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement();
    }
    if (qualifier->hasType(Qualifier::Type::Request))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isRequest"), QStringLiteral("true"));
    }
    if (qualifier->hasType(Qualifier::Type::Response))
    {
        writer.writeTextElement(QStringLiteral("ipxact:isResponse"), QStringLiteral("true"));
    }

    writer.writeEndElement(); // ipxact:qualifier
}
