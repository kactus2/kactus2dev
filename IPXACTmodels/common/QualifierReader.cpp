//-----------------------------------------------------------------------------
// File: QualifierReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.11.2023
//
// Description:
// Reader class for ipxact:qualifier.
//-----------------------------------------------------------------------------

#include "QualifierReader.h"

//-----------------------------------------------------------------------------
// Function: QualifierReader::parseQualifier()
//-----------------------------------------------------------------------------
void QualifierReader::parseQualifier(QDomNode const& qualifierNode, QSharedPointer<Qualifier> qualifier,
    Document::Revision revision)
{
    if (qualifierNode.isNull())
    {
        return;
    }

    bool isStd22 = revision == Document::Revision::Std22;

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isAddress")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Address);
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isData")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Data);

    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isClock")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Clock);

    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isReset"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Reset);
        
        if (isStd22)
        {
            qualifier->setAttribute(Qualifier::Attribute::ResetLevel, 
                node.attributes().namedItem(QStringLiteral("level")).nodeValue());
        }
    }

    if (!isStd22)
    {
        return;
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isValid")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Valid);
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isInterrupt")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Interrupt);
    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isClockEn"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::ClockEnable);

        qualifier->setAttribute(Qualifier::Attribute::ClockEnableLevel, 
            node.attributes().namedItem(QStringLiteral("level")).nodeValue());
    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isPowerEn"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::PowerEnable);

        qualifier->setAttribute(Qualifier::Attribute::PowerEnableLevel, 
            node.attributes().namedItem(QStringLiteral("level")).nodeValue());
        qualifier->setAttribute(Qualifier::Attribute::PowerDomainReference, 
            node.attributes().namedItem(QStringLiteral("powerDomainRef")).nodeValue());
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isOpcode")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Opcode);

    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isProtection")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Protection);

    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isFlowControl"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::FlowControl);

        auto attributes = node.attributes();

        auto flowType = attributes.namedItem(QStringLiteral("flowType")).nodeValue();
        qualifier->setAttribute(Qualifier::Attribute::FlowType, flowType);

        if (flowType == QStringLiteral("user"))
        {
            qualifier->setAttribute(Qualifier::Attribute::UserFlowType, 
                attributes.namedItem(QStringLiteral("user")).nodeValue());
        }
    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isUser"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::User);

        qualifier->setAttribute(Qualifier::Attribute::UserDefined, 
            node.attributes().namedItem(QStringLiteral("user")).nodeValue());
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isRequest")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Request);

    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isResponse")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Type::Response);

    }
}
