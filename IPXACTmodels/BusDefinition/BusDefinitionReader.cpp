//-----------------------------------------------------------------------------
// File: BusDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2015
//
// Description:
// XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#include "BusDefinitionReader.h"

#include "BusDefinition.h"

#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::BusDefinitionReader()
//-----------------------------------------------------------------------------
BusDefinitionReader::BusDefinitionReader(QObject *parent): QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::~BusDefinitionReader()
//-----------------------------------------------------------------------------
BusDefinitionReader::~BusDefinitionReader()
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::createBusDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<BusDefinition> BusDefinitionReader::createBusDefinitionFrom(QDomNode const& document) const
{
    QSharedPointer<BusDefinition> busDefinition(new BusDefinition());

    parseTopComments(document, busDefinition);

    QDomNode busNode = document.firstChildElement();
    parseVLNV(busNode, busDefinition);

    parseDirectConnection(busNode, busDefinition);

    parseBroadcast(busNode, busDefinition);

    parseIsAddressable(busNode, busDefinition);

    parseExtends(busNode, busDefinition);

    parseMaximumMasters(busNode, busDefinition);
    
    parseMaximumSlaves(busNode, busDefinition);

    parseSystemGroupNames(busNode, busDefinition);

    parseDescription(busNode, busDefinition);

    parseParameters(busNode, busDefinition);

    parseAssertions(busNode, busDefinition);

    parseVendorExtension(busNode, busDefinition);

    return busDefinition;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseTopComments()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseTopComments(QDomNode const& document, 
    QSharedPointer<BusDefinition> busDefinition) const
{
    QStringList comments;

    QDomNodeList nodeList = document.childNodes();
    QDomNode busNode = document.firstChildElement();

    for (int i = 0; i < nodeList.size() && nodeList.at(i) != busNode; i++)
    {
        if (nodeList.at(i).isComment())
        {
            comments.append(nodeList.at(i).nodeValue());            
        }
    }

    busDefinition->setTopComments(comments);
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseVLNV()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseVLNV(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const
{
    QString vendor = busNode.firstChildElement("ipxact:vendor").firstChild().nodeValue();
    QString library = busNode.firstChildElement("ipxact:library").firstChild().nodeValue();
    QString name = busNode.firstChildElement("ipxact:name").firstChild().nodeValue();
    QString version = busNode.firstChildElement("ipxact:version").firstChild().nodeValue();

    VLNV busVLNV(VLNV::BUSDEFINITION, vendor, library, name, version);
    busDefinition->setVlnv(busVLNV);
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseDirectConnection()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseDirectConnection(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QString directConnection = busNode.firstChildElement("ipxact:directConnection").firstChild().nodeValue();
    busDefinition->setDirectConnection(directConnection == "true");
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseBroadcast()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseBroadcast(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode broadcastNode = busNode.firstChildElement("ipxact:broadcast");
    if (!broadcastNode.isNull())
    {
        busDefinition->setBroadcast(broadcastNode.firstChild().nodeValue() == "true");
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseIsAddressable()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseIsAddressable(QDomNode const& busNode, 
    QSharedPointer<BusDefinition> busDefinition) const
{
    QString addressable = busNode.firstChildElement("ipxact:isAddressable").firstChild().nodeValue();
    busDefinition->setIsAddressable(addressable == "true");
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseExtends()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseExtends(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode extendsNode = busNode.firstChildElement("ipxact:extends");
    if (!extendsNode.isNull())
    {
        QDomNamedNodeMap attributes = extendsNode.attributes();
        VLNV extendedVLNV;
        extendedVLNV.setType(VLNV::BUSDEFINITION);
        extendedVLNV.setVendor(attributes.namedItem("vendor").nodeValue());
        extendedVLNV.setLibrary(attributes.namedItem("library").nodeValue());
        extendedVLNV.setName(attributes.namedItem("name").nodeValue());
        extendedVLNV.setVersion(attributes.namedItem("version").nodeValue());

        busDefinition->setExtends(extendedVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseMaximumMasters()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseMaximumMasters(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode maximumMastersNode = busNode.firstChildElement("ipxact:maxMasters");
    if (!maximumMastersNode.isNull())
    {
        busDefinition->setMaxMasters(maximumMastersNode.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseMaximumSlaves()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseMaximumSlaves(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode maximumSlavesNode = busNode.firstChildElement("ipxact:maxSlaves");
    if (!maximumSlavesNode.isNull())
    {
        busDefinition->setMaxSlaves(maximumSlavesNode.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseSystemGroupNames()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseSystemGroupNames(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNodeList systemNodes = busNode.firstChildElement("ipxact:systemGroupNames").childNodes();
    
    QStringList systemGroupNames;
    int systemNameCount = systemNodes.count();
    for (int i = 0; i < systemNameCount; i++)
    {
        systemGroupNames.append(systemNodes.at(i).firstChild().nodeValue());
    }

    busDefinition->setSystemGroupNames(systemGroupNames);
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseDescription()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseDescription(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    busDefinition->setDescription(busNode.firstChildElement("ipxact:description").firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseParameters()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseParameters(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNodeList parameterNodes = busNode.firstChildElement("ipxact:parameters").childNodes();
    ParameterReader parameterReader;

    int parameterCount = parameterNodes.count();
    for (int i = 0; i < parameterCount; i++)
    {
        busDefinition->getParameters()->append(parameterReader.createParameterFrom(parameterNodes.at(i)));
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseAssertions()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseAssertions(QDomNode const& busNode, 
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNodeList assertionNodes = busNode.firstChildElement("ipxact:assertions").childNodes();

    int assertionCount = assertionNodes.count();
    for (int i = 0; i < assertionCount; i++)
    {
        QDomNode assertionNode = assertionNodes.at(i);

        QSharedPointer<Assertion> assertion(new Assertion());
        assertion->setName(assertionNode.firstChildElement("ipxact:name").firstChild().nodeValue());
        assertion->setDisplayName(assertionNode.firstChildElement("ipxact:displayName").firstChild().nodeValue());
        assertion->setDescription(assertionNode.firstChildElement("ipxact:description").firstChild().nodeValue());

        assertion->setAssert(assertionNode.firstChildElement("ipxact:assert").firstChild().nodeValue());

        busDefinition->getAssertions()->append(assertion);
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseVendorExtension()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseVendorExtension(QDomNode const& busNode, 
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNodeList extensionNodes = busNode.firstChildElement("ipxact:vendorExtensions").childNodes();

    int extensionCount = extensionNodes.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNodes.at(i)));
        busDefinition->getVendorExtensions()->append(extension);
    }
}
