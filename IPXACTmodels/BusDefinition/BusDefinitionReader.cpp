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
#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::BusDefinitionReader()
//-----------------------------------------------------------------------------
BusDefinitionReader::BusDefinitionReader(QObject *parent):
DocumentReader(parent)
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

    parseXMLProcessingInstructions(document, busDefinition);

    QDomNode busNode = document.firstChildElement();
    parseVLNVElements(busNode, busDefinition, VLNV::BUSDEFINITION);

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

    parseKactusAndVendorExtensions(busNode, busDefinition);

    return busDefinition;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseDirectConnection()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseDirectConnection(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QString directConnection = busNode.firstChildElement(QStringLiteral("ipxact:directConnection")).firstChild().nodeValue();
    busDefinition->setDirectConnection(directConnection == QLatin1String("true"));
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseBroadcast()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseBroadcast(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode broadcastNode = busNode.firstChildElement(QStringLiteral("ipxact:broadcast"));
    if (!broadcastNode.isNull())
    {
        busDefinition->setBroadcast(broadcastNode.firstChild().nodeValue() == QLatin1String("true"));
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseIsAddressable()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseIsAddressable(QDomNode const& busNode, 
    QSharedPointer<BusDefinition> busDefinition) const
{
    QString addressable = busNode.firstChildElement(QStringLiteral("ipxact:isAddressable")).firstChild().nodeValue();
    busDefinition->setIsAddressable(addressable == QLatin1String("true"));
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseExtends()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseExtends(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode extendsNode = busNode.firstChildElement(QStringLiteral("ipxact:extends"));
    if (!extendsNode.isNull())
    {
        QDomNamedNodeMap attributes = extendsNode.attributes();
        VLNV extendedVLNV;
        extendedVLNV.setType(VLNV::BUSDEFINITION);
        extendedVLNV.setVendor(attributes.namedItem(QStringLiteral("vendor")).nodeValue());
        extendedVLNV.setLibrary(attributes.namedItem(QStringLiteral("library")).nodeValue());
        extendedVLNV.setName(attributes.namedItem(QStringLiteral("name")).nodeValue());
        extendedVLNV.setVersion(attributes.namedItem(QStringLiteral("version")).nodeValue());

        busDefinition->setExtends(extendedVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseMaximumMasters()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseMaximumMasters(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode maximumMastersNode = busNode.firstChildElement(QStringLiteral("ipxact:maxMasters"));
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
    QDomNode maximumSlavesNode = busNode.firstChildElement(QStringLiteral("ipxact:maxSlaves"));
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
    QDomNodeList systemNodes = busNode.firstChildElement(QStringLiteral("ipxact:systemGroupNames")).childNodes();
    
    QStringList systemGroupNames;
    int systemNameCount = systemNodes.count();
    for (int i = 0; i < systemNameCount; i++)
    {
        systemGroupNames.append(systemNodes.at(i).firstChild().nodeValue());
    }

    busDefinition->setSystemGroupNames(systemGroupNames);
}
