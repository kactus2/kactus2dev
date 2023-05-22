//-----------------------------------------------------------------------------
// File: BusDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2015
//
// Description:
// XML reader for IP-XACT Bus definition element.
//-----------------------------------------------------------------------------

#include "BusDefinitionReader.h"

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::createBusDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<BusDefinition> BusDefinitionReader::createBusDefinitionFrom(QDomNode const& document)
{
    QDomNode busNode = document.firstChildElement();
    Document::Revision docRevision = DocumentReader::getXMLDocumentRevision(busNode);
    
    VLNV vlnv = CommonItemsReader::createVLNVFrom(busNode, VLNV::BUSDEFINITION);

    QSharedPointer<BusDefinition> busDefinition(new BusDefinition(vlnv, docRevision));

    DocumentReader::parseTopComments(document, busDefinition);

    DocumentReader::parseXMLProcessingInstructions(document, busDefinition);

    DocumentReader::parseNamespaceDeclarations(busNode, busDefinition);

    DocumentReader::parseDocumentNameGroup(busNode, busDefinition);

    Details::parseDirectConnection(busNode, busDefinition);
    
    Details::parseBroadcast(busNode, busDefinition);
    
    Details::parseIsAddressable(busNode, busDefinition);
    
    Details::parseExtends(busNode, busDefinition);
    
    Details::parseMaximumInitiators(busNode, busDefinition);
    
    Details::parseMaximumTargets(busNode, busDefinition);
    
    Details::parseSystemGroupNames(busNode, busDefinition);
    
    Details::parseChoices(busNode, busDefinition);
    
    DocumentReader::parseParameters(busNode, busDefinition);

    DocumentReader::parseAssertions(busNode, busDefinition);

    DocumentReader::parseKactusAndVendorExtensions(busNode, busDefinition);

    return busDefinition;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseDirectConnection()
//-----------------------------------------------------------------------------
void BusDefinitionReader::Details::parseDirectConnection(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition)
{
    QString directConnection = busNode.firstChildElement(QStringLiteral("ipxact:directConnection")).firstChild().nodeValue();
    busDefinition->setDirectConnection(directConnection == QLatin1String("true"));
}

//-----------------------------------------------------------------------------
// Function: parseBroadcast()
//-----------------------------------------------------------------------------
void BusDefinitionReader::Details::parseBroadcast(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition)
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
void BusDefinitionReader::Details::parseIsAddressable(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition)
{
    QString addressable = busNode.firstChildElement(QStringLiteral("ipxact:isAddressable")).firstChild().nodeValue();
    busDefinition->setIsAddressable(addressable == QLatin1String("true"));
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseExtends()
//-----------------------------------------------------------------------------
void BusDefinitionReader::Details::parseExtends(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition)
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
void BusDefinitionReader::Details::parseMaximumInitiators(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition)
{
    QDomNode maximumInitiatorsNode = busNode.firstChildElement(QStringLiteral("ipxact:maxMasters"));
    if (!maximumInitiatorsNode.isNull())
    {
        busDefinition->setMaxMasters(maximumInitiatorsNode.firstChild().nodeValue().toStdString());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseMaximumTargets()
//-----------------------------------------------------------------------------
void BusDefinitionReader::Details::parseMaximumTargets(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition)
{
    QDomNode maximumTargetsNode = busNode.firstChildElement(QStringLiteral("ipxact:maxSlaves"));
    if (!maximumTargetsNode.isNull())
    {
        busDefinition->setMaxSlaves(maximumTargetsNode.firstChild().nodeValue().toStdString());
    }
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionReader::parseSystemGroupNames()
//-----------------------------------------------------------------------------
void BusDefinitionReader::Details::parseSystemGroupNames(QDomNode const& busNode,
    QSharedPointer<BusDefinition> busDefinition)
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
void BusDefinitionReader::Details::parseChoices(QDomNode const& busNode, QSharedPointer<BusDefinition> busDefinition)
{
    if (busDefinition->getRevision() == Document::Revision::Std14 ||
        busDefinition->getRevision() == Document::Revision::Unknown)
    {
        return;
    }

    auto parsedChoices = CommonItemsReader::parseChoices(busNode);
    busDefinition->setChoices(parsedChoices);
}
