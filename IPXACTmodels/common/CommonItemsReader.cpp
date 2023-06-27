//-----------------------------------------------------------------------------
// File: CommonItemsReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------

#include "CommonItemsReader.h"
#include "ParameterReader.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/Component/ChoiceReader.h>

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::CommonItemsReader()
//-----------------------------------------------------------------------------
CommonItemsReader::CommonItemsReader()
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::createVLNVFrom()
//-----------------------------------------------------------------------------
VLNV CommonItemsReader::createVLNVFrom(QDomNode const& vlnvNode, VLNV::IPXactType type)
{
    QString vendor = vlnvNode.firstChildElement(QStringLiteral("ipxact:vendor")).firstChild().nodeValue();
    QString library = vlnvNode.firstChildElement(QStringLiteral("ipxact:library")).firstChild().nodeValue();
    QString name = vlnvNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();
    QString version = vlnvNode.firstChildElement(QStringLiteral("ipxact:version")).firstChild().nodeValue();
    
    return VLNV(type, vendor, library, name, version);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseVLNVAttributes()
//-----------------------------------------------------------------------------
VLNV CommonItemsReader::parseVLNVAttributes(QDomNode const& vlnvNode, VLNV::IPXactType vlnvType)
{
    QDomNamedNodeMap attributes = vlnvNode.attributes();

    VLNV attributedVLNV;
    attributedVLNV.setType(vlnvType);
    attributedVLNV.setVendor(attributes.namedItem(QStringLiteral("vendor")).nodeValue());
    attributedVLNV.setLibrary(attributes.namedItem(QStringLiteral("library")).nodeValue());
    attributedVLNV.setName(attributes.namedItem(QStringLiteral("name")).nodeValue());
    attributedVLNV.setVersion(attributes.namedItem(QStringLiteral("version")).nodeValue());

    return attributedVLNV;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseAndCreateParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > CommonItemsReader::parseAndCreateParameters(
    QDomNode const& itemNode, Document::Revision revision)
{
    QDomNodeList parameterNodeList = itemNode.firstChildElement(QStringLiteral("ipxact:parameters")).childNodes();
    
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters (new QList<QSharedPointer<Parameter> > ());
    if (!parameterNodeList.isEmpty())
    {
        ParameterReader parameterReader;

        int parameterCount = parameterNodeList.count();
        for (int parameterIndex = 0;parameterIndex < parameterCount; parameterIndex++)
        {
            newParameters->append(parameterReader.createParameterFrom(parameterNodeList.at(parameterIndex), revision));
        }
    }
    return newParameters;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void CommonItemsReader::parseVendorExtensions(QDomNode const& itemNode, QSharedPointer<Extendable> element)
{
    QDomNodeList extensionNodes = itemNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions")).childNodes();

    int extensionCount = extensionNodes.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QDomNode extensionNode = extensionNodes.at(i);

        if (!extensionNode.nodeName().startsWith(QStringLiteral("kactus2:")))
        {
            QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNode));
            element->getVendorExtensions()->append(extension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseConfigurableVLNVReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> CommonItemsReader::parseConfigurableVLNVReference(
    QDomNode const& configurableVLNVNode, VLNV::IPXactType type)
{
    QDomNamedNodeMap attributeMap = configurableVLNVNode.attributes();

    QString vendor = attributeMap.namedItem(QStringLiteral("vendor")).nodeValue();
    QString library = attributeMap.namedItem(QStringLiteral("library")).nodeValue();
    QString name = attributeMap.namedItem(QStringLiteral("name")).nodeValue();
    QString version = attributeMap.namedItem(QStringLiteral("version")).nodeValue();

    QSharedPointer<ConfigurableVLNVReference> vlnvReference(
        new ConfigurableVLNVReference(type, vendor, library, name, version));

    QDomNode configurableElementsNode = configurableVLNVNode.firstChildElement(QStringLiteral("ipxact:configurableElementValues"));

    QDomNodeList configurableElementNodeList = configurableElementsNode.childNodes();
    for (int i = 0; i < configurableElementNodeList.size(); ++i)
    {
        QSharedPointer<ConfigurableElementValue> newConfigurableElementValue =
            parseConfigurableElementValue(configurableElementNodeList.at(i));

        vlnvReference->getConfigurableElementValues()->append(newConfigurableElementValue);
    }

    return vlnvReference;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseConfigurableElementValue()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableElementValue> CommonItemsReader::parseConfigurableElementValue(
    QDomNode const& configurableElementNode)
{
    QSharedPointer<ConfigurableElementValue> newConfigurableElementValue (new ConfigurableElementValue());

    newConfigurableElementValue->setConfigurableValue(configurableElementNode.firstChild().nodeValue());

    QDomNamedNodeMap attributeMap = configurableElementNode.attributes();

    for (int i = 0; i < attributeMap.size(); ++i)
    {
        QDomNode attributeItem = attributeMap.item(i);
        newConfigurableElementValue->insertAttribute(
            attributeItem.nodeName(), attributeItem.firstChild().nodeValue());
    }

    return newConfigurableElementValue;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> CommonItemsReader::parseAttributes(QDomNode const& attributedNode)
{
    QMap<QString, QString> attributes;

    QDomNamedNodeMap attributeMap = attributedNode.attributes();
    for (int j = 0; j < attributeMap.size(); ++j)
    {
        QString name = attributeMap.item(j).nodeName();
        QString value = attributeMap.item(j).nodeValue();
        attributes.insert(name, value);
    }

    return attributes;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseIsPresent()
//-----------------------------------------------------------------------------
QString CommonItemsReader::parseIsPresent(QDomElement const& isPresentElement)
{
    if (!isPresentElement.isNull())
    {
        return isPresentElement.firstChild().nodeValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parsePoint()
//-----------------------------------------------------------------------------
QPointF CommonItemsReader::parsePoint(QDomNode const& node)
{
    int x = node.attributes().namedItem(QStringLiteral("x")).nodeValue().toInt();
    int y = node.attributes().namedItem(QStringLiteral("y")).nodeValue().toInt();

    return QPointF(x, y);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseChoices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Choice> > > CommonItemsReader::parseChoices(QDomNode const& itemNode)
{
    QDomElement choicesElement = itemNode.firstChildElement(QStringLiteral("ipxact:choices"));
    
    QSharedPointer<QList<QSharedPointer<Choice> > > parsedChoices(new QList<QSharedPointer<Choice> >);

    if (!choicesElement.isNull())
    {
        ChoiceReader choiceReader;

        QDomNodeList choiceNodeList = choicesElement.elementsByTagName(QStringLiteral("ipxact:choice"));
        for (int choiceIndex = 0; choiceIndex < choiceNodeList.count(); ++choiceIndex)
        {
            QDomNode choiceNode = choiceNodeList.at(choiceIndex);
            QSharedPointer<Choice> newChoice = choiceReader.createChoiceFrom(choiceNode);

            parsedChoices->append(newChoice);
        }
    }

    return parsedChoices;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseQualifier()
//-----------------------------------------------------------------------------
void CommonItemsReader::parseQualifier(QDomNode const& qualifierNode, QSharedPointer<Qualifier> qualifier,
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
        qualifier->setType(Qualifier::Address);
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isData")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Data);

    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isClock")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Clock);

    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isReset"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Reset);
        
        if (isStd22)
        {
            qualifier->setAttribute(Qualifier::Attribute::ResetLevel, node.attributes().namedItem(QStringLiteral("level")).nodeValue());
        }
    }

    if (!isStd22)
    {
        return;
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isValid")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Valid);
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isInterrupt")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Interrupt);
    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isClockEn"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::ClockEnable);

        qualifier->setAttribute(Qualifier::Attribute::ClockEnableLevel, node.attributes().namedItem(QStringLiteral("level")).nodeValue());
    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isPowerEn"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::PowerEnable);

        qualifier->setAttribute(Qualifier::Attribute::PowerEnableLevel, node.attributes().namedItem(QStringLiteral("level")).nodeValue());
        qualifier->setAttribute(Qualifier::Attribute::PowerDomainReference, node.attributes().namedItem(QStringLiteral("powerDomainRef")).nodeValue());
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isOpcode")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Opcode);

    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isProtection")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Protection);

    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isFlowControl"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::FlowControl);

        auto attributes = node.attributes();

        auto flowType = attributes.namedItem(QStringLiteral("flowType")).nodeValue();
        qualifier->setAttribute(Qualifier::Attribute::FlowType, flowType);

        if (flowType == QStringLiteral("user"))
        {
            qualifier->setAttribute(Qualifier::Attribute::UserFlowType, attributes.namedItem(QStringLiteral("user")).nodeValue());
        }
    }

    if (auto const& node = qualifierNode.firstChildElement(QStringLiteral("ipxact:isUser"));
        node.firstChild().nodeValue() == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::User);

        qualifier->setAttribute(Qualifier::Attribute::UserDefined, node.attributes().namedItem(QStringLiteral("user")).nodeValue());
    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isRequest")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Request);

    }

    if (qualifierNode.firstChildElement(QStringLiteral("ipxact:isResponse")).firstChild().nodeValue()
        == QStringLiteral("true"))
    {
        qualifier->setType(Qualifier::Response);

    }
}
