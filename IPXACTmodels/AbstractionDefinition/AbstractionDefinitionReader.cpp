//-----------------------------------------------------------------------------
// File: AbstractionDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2015
//
// Description:
// Reader for ipxact:abstractionDefinition.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionReader.h"

#include "AbstractionDefinition.h"

#include "PortAbstraction.h"
#include "WireAbstraction.h"
#include "WireAbstractionReader.h"
#include "TransactionalAbstractionReader.h"
#include "PacketReader.h"

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::createAbstractionDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionDefinition> AbstractionDefinitionReader::createAbstractionDefinitionFrom(
    QDomNode const& document)
{
    QDomNode definitionNode = document.firstChildElement(QStringLiteral("ipxact:abstractionDefinition"));
    Document::Revision docRevision = DocumentReader::getXMLDocumentRevision(definitionNode);

    VLNV vlnv = CommonItemsReader::createVLNVFrom(definitionNode, VLNV::ABSTRACTIONDEFINITION);

    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition(vlnv, docRevision));

    DocumentReader::parseTopComments(document, abstractionDefinition);

    DocumentReader::parseXMLProcessingInstructions(document, abstractionDefinition);

    DocumentReader::parseNamespaceDeclarations(definitionNode, abstractionDefinition);

    DocumentReader::parseDocumentNameGroup(definitionNode, abstractionDefinition);

    Details::parseBusType(definitionNode, abstractionDefinition);

    Details::parseExtends(definitionNode, abstractionDefinition);

    Details::parsePorts(definitionNode, abstractionDefinition, docRevision);
    
    Details::parseChoices(definitionNode, abstractionDefinition);

    DocumentReader::parseParameters(definitionNode, abstractionDefinition);

    DocumentReader::parseAssertions(definitionNode, abstractionDefinition);

    DocumentReader::parseKactusAndVendorExtensions(definitionNode, abstractionDefinition);

    return abstractionDefinition;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::Details::parseBusType()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::Details::parseBusType(QDomNode const& definitionNode, 
    QSharedPointer<AbstractionDefinition> abstractionDefinion)
{
    QDomNode extendsNode = definitionNode.firstChildElement(QStringLiteral("ipxact:busType"));
    abstractionDefinion->setBusType(DocumentReader::parseVLNVAttributes(extendsNode, VLNV::BUSDEFINITION));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::Details::parseExtends()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::Details::parseExtends(QDomNode const& definitionNode, 
    QSharedPointer<AbstractionDefinition> abstractionDefinion)
{
    QDomNode extendsNode = definitionNode.firstChildElement(QStringLiteral("ipxact:extends"));
    if (!extendsNode.isNull())
    {
        abstractionDefinion->setExtends(DocumentReader::parseVLNVAttributes(extendsNode, VLNV::ABSTRACTIONDEFINITION));
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::Details::parsePorts()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::Details::parsePorts(QDomNode definitionNode, 
    QSharedPointer<AbstractionDefinition> abstractionDefinion, Document::Revision revision)
{
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > logicalPorts = abstractionDefinion->getLogicalPorts();

    QDomNodeList portNodes = definitionNode.firstChildElement(
        QStringLiteral("ipxact:ports")).elementsByTagName(QStringLiteral("ipxact:port"));
    int portCount = portNodes.count();
    for (int i = 0; i < portCount; i++)
    {
        QSharedPointer<PortAbstraction> port = parsePort(portNodes.at(i), revision);
        logicalPorts->append(port);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::Details::parsePort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionDefinitionReader::Details::parsePort(QDomNode const& portNode,
    Document::Revision revision)
{
    QSharedPointer<PortAbstraction> port(new PortAbstraction());

    port->setLogicalName(portNode.firstChildElement(QStringLiteral("ipxact:logicalName")).firstChild().nodeValue());
    port->setDisplayName(portNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
    port->setDescription(portNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());

    if (revision == Document::Revision::Std22)
    {
        port->setShortDescription(portNode.firstChildElement(QStringLiteral("ipxact:shortDescription")).firstChild().nodeValue());
        port->setMatch(portNode.firstChildElement(QStringLiteral("ipxact:match")).firstChild().nodeValue() == QStringLiteral("true"));
        parsePackets(portNode, port);
    }
    else
    {
        port->setIsPresent(portNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());
    }

    parseWire(portNode, port, revision);
    parseTransactional(portNode, port, revision);

    CommonItemsReader::parseVendorExtensions(portNode, port);

    return port;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::Details::parsePackets()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::Details::parsePackets(QDomNode const& portNode, QSharedPointer<PortAbstraction> port)
   
{
    auto packetsNode = portNode.firstChildElement(QStringLiteral("ipxact:packets"));

    if (packetsNode.isNull())
    {
        return;
    }

    QSharedPointer<QList<QSharedPointer<Packet> > > portPackets(new QList<QSharedPointer<Packet> >());

    auto packetNodes = packetsNode.elementsByTagName(QStringLiteral("ipxact:packet"));

    for (int i = 0; i < packetNodes.count(); ++i)
    {
        auto packetNode = packetNodes.at(i);
        auto packet = PacketReader::createPacketFrom(packetNode);

        portPackets->append(packet);
    }

    port->setPackets(portPackets);
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::Details::parseWire()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::Details::parseWire(QDomNode const& portNode, QSharedPointer<PortAbstraction> port,
    Document::Revision revision)
{
    QDomNode wireNode = portNode.firstChildElement(QStringLiteral("ipxact:wire"));

    if (!wireNode.isNull())
    {
        QSharedPointer<WireAbstraction> wire = WireAbstractionReader::createWireAbstractionFrom(wireNode, revision);

        port->setWire(wire);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::Details::parseTransactional()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::Details::parseTransactional(QDomNode const& portNode, 
    QSharedPointer<PortAbstraction> port, Document::Revision revision)
{
    QDomNode transactionalNode = portNode.firstChildElement(QStringLiteral("ipxact:transactional"));

    if (!transactionalNode.isNull())
    {
        QSharedPointer<TransactionalAbstraction> transactional =
            TransactionalAbstractionReader::createTransactionalAbstractionFrom(transactionalNode, revision);

        port->setTransactional(transactional);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::Details::parseChoices()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::Details::parseChoices(QDomNode const& definitionNode,
    QSharedPointer<AbstractionDefinition> definition)
{
    if (definition->getRevision() != Document::Revision::Std22)
    {
        return;
    }

    auto parsedChoices = CommonItemsReader::parseChoices(definitionNode);

    definition->setChoices(parsedChoices);
}
