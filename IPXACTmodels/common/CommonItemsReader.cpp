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

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::CommonItemsReader()
//-----------------------------------------------------------------------------
CommonItemsReader::CommonItemsReader(QObject* parent /* = 0 */) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::~CommonItemsReader()
//-----------------------------------------------------------------------------
CommonItemsReader::~CommonItemsReader()
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::createVLNVFrom()
//-----------------------------------------------------------------------------
VLNV CommonItemsReader::createVLNVFrom(QDomNode const& vlnvNode, VLNV::IPXactType type) const
{
    QString vendor = vlnvNode.firstChildElement(QStringLiteral("ipxact:vendor")).firstChild().nodeValue();
    QString library = vlnvNode.firstChildElement(QStringLiteral("ipxact:library")).firstChild().nodeValue();
    QString name = vlnvNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();
    QString version = vlnvNode.firstChildElement(QStringLiteral("ipxact:version")).firstChild().nodeValue();

    VLNV itemVLNV;
    itemVLNV.setType(type);
    itemVLNV.setVendor(vendor);
    itemVLNV.setLibrary(library);
    itemVLNV.setName(name);
    itemVLNV.setVersion(version);

    return itemVLNV;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseVLNVAttributes()
//-----------------------------------------------------------------------------
VLNV CommonItemsReader::parseVLNVAttributes(QDomNode const& vlnvNode, VLNV::IPXactType vlnvType) const
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
    QDomNode const& itemNode) const
{
    QDomNodeList parameterNodeList = itemNode.firstChildElement(QStringLiteral("ipxact:parameters")).childNodes();
    
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters (new QList<QSharedPointer<Parameter> > ());
    if (!parameterNodeList.isEmpty())
    {
        ParameterReader parameterReader;

        int parameterCount = parameterNodeList.count();
        for (int parameterIndex = 0;parameterIndex < parameterCount; parameterIndex++)
        {
            newParameters->append(parameterReader.createParameterFrom(parameterNodeList.at(parameterIndex)));
        }
    }
    return newParameters;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void CommonItemsReader::parseVendorExtensions(QDomNode const& itemNode, QSharedPointer<Extendable> element)
    const
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
    QDomNode const& configurableVLNVNode, VLNV::IPXactType type) const
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
    QDomNode const& configurableElementNode) const
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
// Function: CommonItemsReader::parseIsPresent()
//-----------------------------------------------------------------------------
QString CommonItemsReader::parseIsPresent(QDomElement const& isPresentElement) const
{
    if (!isPresentElement.isNull())
    {
        return isPresentElement.firstChild().nodeValue();
    }
    else
    {
        return QString();
    }
}