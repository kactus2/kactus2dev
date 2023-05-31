//-----------------------------------------------------------------------------
// File: AbstractionDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2015
//
// Description:
// Reader class for ipxact:abstractionDefinition.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionReader.h"

#include "AbstractionDefinition.h"

#include "PortAbstraction.h"
#include "WireAbstraction.h"
#include "WireAbstractionReader.h"
#include "TransactionalAbstractionReader.h"
#include "PacketReader.h"

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::AbstractionDefinitionReader()
//-----------------------------------------------------------------------------
AbstractionDefinitionReader::AbstractionDefinitionReader() : DocumentReader()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::~AbstractionDefinitionReader()
//-----------------------------------------------------------------------------
AbstractionDefinitionReader::~AbstractionDefinitionReader()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::createAbstractionDefinitionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionDefinition> AbstractionDefinitionReader::createAbstractionDefinitionFrom(
    QDomNode const& document) const
{
    QDomNode definitionNode = document.firstChildElement(QStringLiteral("ipxact:abstractionDefinition"));
    Document::Revision docRevision = DocumentReader::getXMLDocumentRevision(definitionNode);

    VLNV vlnv = CommonItemsReader::createVLNVFrom(definitionNode, VLNV::ABSTRACTIONDEFINITION);

    QSharedPointer<AbstractionDefinition> abstractionDefinion(new AbstractionDefinition(vlnv, docRevision));

    parseTopComments(document, abstractionDefinion);

    parseXMLProcessingInstructions(document, abstractionDefinion);

    parseNamespaceDeclarations(definitionNode, abstractionDefinion);

    parseDocumentNameGroup(definitionNode, abstractionDefinion);

    parseBusType(definitionNode, abstractionDefinion);

    parseExtends(definitionNode, abstractionDefinion);

    parsePorts(definitionNode, abstractionDefinion, docRevision);
    
    parseChoices(definitionNode, abstractionDefinion);

    parseParameters(definitionNode, abstractionDefinion);

    parseAssertions(definitionNode, abstractionDefinion);

    parseKactusAndVendorExtensions(definitionNode, abstractionDefinion);

    return abstractionDefinion;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parseBusType()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parseBusType(QDomNode const& definitionNode, 
    QSharedPointer<AbstractionDefinition> abstractionDefinion) const
{
    QDomNode extendsNode = definitionNode.firstChildElement(QStringLiteral("ipxact:busType"));
    abstractionDefinion->setBusType(parseVLNVAttributes(extendsNode, VLNV::BUSDEFINITION));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parseExtends()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parseExtends(QDomNode const& definitionNode, 
    QSharedPointer<AbstractionDefinition> abstractionDefinion) const
{
    QDomNode extendsNode = definitionNode.firstChildElement(QStringLiteral("ipxact:extends"));
    if (!extendsNode.isNull())
    {
        abstractionDefinion->setExtends(parseVLNVAttributes(extendsNode, VLNV::ABSTRACTIONDEFINITION));
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parsePorts()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parsePorts(QDomNode definitionNode, 
    QSharedPointer<AbstractionDefinition> abstractionDefinion, Document::Revision revision) const
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
// Function: AbstractionDefinitionReader::parsePort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionDefinitionReader::parsePort(QDomNode const& portNode,
    Document::Revision revision) const
{
    QSharedPointer<PortAbstraction> port(new PortAbstraction());

    port->setIsPresent(portNode.firstChildElement(QStringLiteral("ipxact:isPresent")).firstChild().nodeValue());
    port->setLogicalName(portNode.firstChildElement(QStringLiteral("ipxact:logicalName")).firstChild().nodeValue());
    port->setDisplayName(portNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
    port->setDescription(portNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());
    port->setShortDescription(portNode.firstChildElement(QStringLiteral("ipxact:shortDescription")).firstChild().nodeValue());
    port->setMatch(portNode.firstChildElement(QStringLiteral("ipxact:match")).firstChild().nodeValue() == QStringLiteral("true"));

    parseWire(portNode, port, revision);

    parseTransactional(portNode, port, revision);

    parsePackets(portNode, port);

    parseVendorExtensions(portNode, port);

    return port;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parsePackets()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parsePackets(QDomNode const& portNode, QSharedPointer<PortAbstraction> port)
    const
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
// Function: AbstractionDefinitionReader::parseWire()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parseWire(QDomNode const& portNode, QSharedPointer<PortAbstraction> port,
    Document::Revision revision) const
{
    QDomNode wireNode = portNode.firstChildElement(QStringLiteral("ipxact:wire"));

    if (!wireNode.isNull())
    {
        QSharedPointer<WireAbstraction> wire = WireAbstractionReader::createWireAbstractionFrom(wireNode, revision);

        port->setWire(wire);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parseTransactional()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parseTransactional(QDomNode const& portNode, 
    QSharedPointer<PortAbstraction> port, Document::Revision revision) const
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
// Function: AbstractionDefinitionReader::parseChoices()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parseChoices(QDomNode const& definitionNode,
    QSharedPointer<AbstractionDefinition> definition) const
{
    auto parsedChoices = CommonItemsReader::parseChoices(definitionNode);

    definition->setChoices(parsedChoices);
}
