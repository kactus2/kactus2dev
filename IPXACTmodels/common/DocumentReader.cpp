//-----------------------------------------------------------------------------
// File: DocumentReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.08.2015
//
// Description:
// Base class for XML readers.
//-----------------------------------------------------------------------------

#include "DocumentReader.h"
#include "ParameterReader.h"

#include <IPXACTmodels/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: DocumentReader::DocumentReader()
//-----------------------------------------------------------------------------
DocumentReader::DocumentReader(QObject* parent /* = 0 */):
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: DocumentReader::~DocumentReader()
//-----------------------------------------------------------------------------
DocumentReader::~DocumentReader()
{

}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseTopComments()
//-----------------------------------------------------------------------------
void DocumentReader::parseTopComments(QDomNode const& documentNode, QSharedPointer<Document> document) const
{
    QStringList comments;

    QDomNodeList nodeList = documentNode.childNodes();
    QDomNode singleDocumentNode = documentNode.firstChildElement();

    for (int i = 0; i < nodeList.size() && nodeList.at(i) != singleDocumentNode; ++i)
    {
        if (nodeList.at(i).isComment())
        {
            comments.append(nodeList.at(i).nodeValue());
        }
    }

    document->setTopComments(comments);
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseVLNV()
//-----------------------------------------------------------------------------
void DocumentReader::parseVLNVElements(QDomNode const& documentNode, QSharedPointer<Document> document,
    VLNV::IPXactType type) const
{
    QString vendor = documentNode.firstChildElement("ipxact:vendor").firstChild().nodeValue();
    QString library = documentNode.firstChildElement("ipxact:library").firstChild().nodeValue();
    QString name = documentNode.firstChildElement("ipxact:name").firstChild().nodeValue();
    QString version = documentNode.firstChildElement("ipxact:version").firstChild().nodeValue();

    VLNV documentVLNV(type, vendor, library, name, version);
    document->setVlnv(documentVLNV);
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseVLNVAttributes()
//-----------------------------------------------------------------------------
VLNV DocumentReader::parseVLNVAttributes(QDomNode const& vlnvNode, VLNV::IPXactType vlnvType) const
{
    QDomNamedNodeMap attributes = vlnvNode.attributes();

    VLNV readVLNV;
    readVLNV.setType(vlnvType);
    readVLNV.setVendor(attributes.namedItem("vendor").nodeValue());
    readVLNV.setLibrary(attributes.namedItem("library").nodeValue());
    readVLNV.setName(attributes.namedItem("name").nodeValue());
    readVLNV.setVersion(attributes.namedItem("version").nodeValue());

    return readVLNV;
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseDescription()
//-----------------------------------------------------------------------------
void DocumentReader::parseDescription(QDomNode const& documentNode, QSharedPointer<Document> document) const
{
    document->setDescription(documentNode.firstChildElement("ipxact:description").firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseParameters()
//-----------------------------------------------------------------------------
void DocumentReader::parseParameters(QDomNode const& documentNode, QSharedPointer<Document> document) const
{
    QDomNodeList parameterNodes = documentNode.firstChildElement("ipxact:parameters").childNodes();
    ParameterReader parameterReader;

    int parameterCount = parameterNodes.count();
    for (int i = 0;i < parameterCount; i++)
    {
        document->getParameters()->append(parameterReader.createParameterFrom(parameterNodes.at(i)));
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseAssertions()
//-----------------------------------------------------------------------------
void DocumentReader::parseAssertions(QDomNode const& documentNode, QSharedPointer<Document> document) const
{
    QDomNodeList assertionNodes = documentNode.firstChildElement("ipxact:assertions").childNodes();

    int assertionCount = assertionNodes.count();
    for (int i = 0; i < assertionCount; i++)
    {
        QDomNode assertionNode = assertionNodes.at(i);

        QSharedPointer<Assertion> assertion(new Assertion());
        assertion->setName(assertionNode.firstChildElement("ipxact:name").firstChild().nodeValue());
        assertion->setDisplayName(assertionNode.firstChildElement("ipxact:displayName").firstChild().nodeValue());
        assertion->setDescription(assertionNode.firstChildElement("ipxact:description").firstChild().nodeValue());
        
        assertion->setAssert(assertionNode.firstChildElement("ipxact:assert").firstChild().nodeValue());

        document->getAssertions()->append(assertion);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void DocumentReader::parseVendorExtensions(QDomNode const& documentNode, QSharedPointer<Extendable> document) const
{
    QDomNodeList extensionNodes = documentNode.firstChildElement("ipxact:vendorExtensions").childNodes();

    int extensionCount = extensionNodes.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNodes.at(i)));
        document->getVendorExtensions()->append(extension);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::ConfigurableVLNVReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> DocumentReader::parseConfigurableVLNVReference(
    QDomNode const& configurableVLNVNode, VLNV::IPXactType type) const
{
    QDomNamedNodeMap attributeMap = configurableVLNVNode.attributes();

    QString vendor = attributeMap.namedItem("vendor").nodeValue();
    QString library = attributeMap.namedItem("library").nodeValue();
    QString name = attributeMap.namedItem("name").nodeValue();
    QString version = attributeMap.namedItem("version").nodeValue();

    QSharedPointer<ConfigurableVLNVReference> newAbstractorRef (
        new ConfigurableVLNVReference(type, vendor, library, name, version));

    QDomNode configurableElementsNode = configurableVLNVNode.
        firstChildElement("ipxact:configurableElementValues");

    QDomNodeList configurableElementNodeList = configurableElementsNode.childNodes();
    for (int i = 0; i < configurableElementNodeList.size(); ++i)
    {
        QSharedPointer<ConfigurableElementValue> newConfigurableElementValue =
            parseConfigurableElementValue(configurableElementNodeList.at(i));

        newAbstractorRef->getConfigurableElementValues()->append(newConfigurableElementValue);
    }

    return newAbstractorRef;
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseConfigurableElementValue()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableElementValue> DocumentReader::parseConfigurableElementValue(
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