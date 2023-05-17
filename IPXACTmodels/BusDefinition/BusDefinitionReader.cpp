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

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::BusDefinitionReader()
//-----------------------------------------------------------------------------
BusDefinitionReader::BusDefinitionReader(): DocumentReader()
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
    QDomNode busNode = document.firstChildElement();
    Document::Revision docRevision = getXMLDocumentRevision(busNode);
    
    VLNV vlnv = createVLNVFrom(busNode, VLNV::BUSDEFINITION);

    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, docRevision));

    parseTopComments(document, busDefinition);

    parseXMLProcessingInstructions(document, busDefinition);

    parseNamespaceDeclarations(busNode, busDefinition);

    parseDocumentNameGroup(busNode, busDefinition);

    parseDirectConnection(busNode, busDefinition);

    parseBroadcast(busNode, busDefinition);

    parseIsAddressable(busNode, busDefinition);

    parseExtends(busNode, busDefinition);

    parseMaximumInitiators(busNode, busDefinition);
    
    parseMaximumTargets(busNode, busDefinition);

    parseSystemGroupNames(busNode, busDefinition);

    parseChoices(busNode, busDefinition);

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
// Function: BusDefinitionReader::parseMaximumInitiators()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseMaximumInitiators(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode maximumInitiatorsNode = busNode.firstChildElement(QStringLiteral("ipxact:maxMasters"));
    if (!maximumInitiatorsNode.isNull())
    {
        busDefinition->setMaxMasters(maximumInitiatorsNode.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseMaximumTargets()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseMaximumTargets(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition) const
{
    QDomNode maximumTargetsNode = busNode.firstChildElement(QStringLiteral("ipxact:maxSlaves"));
    if (!maximumTargetsNode.isNull())
    {
        busDefinition->setMaxSlaves(maximumTargetsNode.firstChild().nodeValue());
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

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseChoices()
//-----------------------------------------------------------------------------
void BusDefinitionReader::parseChoices(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition) const
{
    if (busDefinition->getRevision() == Document::Revision::Std14 ||
        busDefinition->getRevision() == Document::Revision::Unknown)
    {
        return;
    }

    auto parsedChoices = CommonItemsReader::parseChoices(busNode);
    busDefinition->setChoices(parsedChoices);
}
