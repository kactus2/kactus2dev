//-----------------------------------------------------------------------------
// File: AbstractionDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::AbstractionDefinitionReader()
//-----------------------------------------------------------------------------
AbstractionDefinitionReader::AbstractionDefinitionReader(QObject* parent) : DocumentReader(parent)
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
    QSharedPointer<AbstractionDefinition> abstractionDefinion(new AbstractionDefinition());

    parseTopComments(document, abstractionDefinion);

    parseXMLProcessingInstructions(document, abstractionDefinion);

    QDomNode definitionNode = document.firstChildElement("ipxact:abstractionDefinition");

    parseVLNVElements(definitionNode, abstractionDefinion, VLNV::ABSTRACTIONDEFINITION);

    parseBusType(definitionNode, abstractionDefinion);

    parseExtends(definitionNode, abstractionDefinion);

    parsePorts(definitionNode, abstractionDefinion);

    parseDescription(definitionNode, abstractionDefinion);

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
    QDomNode extendsNode = definitionNode.firstChildElement("ipxact:busType");
    abstractionDefinion->setBusType(parseVLNVAttributes(extendsNode, VLNV::BUSDEFINITION));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parseExtends()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parseExtends(QDomNode const& definitionNode, 
    QSharedPointer<AbstractionDefinition> abstractionDefinion) const
{
    QDomNode extendsNode = definitionNode.firstChildElement("ipxact:extends");
    if (!extendsNode.isNull())
    {
        abstractionDefinion->setExtends(parseVLNVAttributes(extendsNode, VLNV::ABSTRACTIONDEFINITION));
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parsePorts()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parsePorts(QDomNode definitionNode, 
    QSharedPointer<AbstractionDefinition> abstractionDefinion) const
{
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > logicalPorts = abstractionDefinion->getLogicalPorts();

    QDomNodeList portNodes = definitionNode.firstChildElement("ipxact:ports").elementsByTagName("ipxact:port");
    int portCount = portNodes.count();
    for (int i = 0; i < portCount; i++)
    {
        QSharedPointer<PortAbstraction> port = parsePort(portNodes.at(i));
        logicalPorts->append(port);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parsePort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionDefinitionReader::parsePort(QDomNode const& portNode) const
{
    QSharedPointer<PortAbstraction> port(new PortAbstraction());

    port->setIsPresent(portNode.firstChildElement("ipxact:isPresent").firstChild().nodeValue());
    port->setLogicalName(portNode.firstChildElement("ipxact:logicalName").firstChild().nodeValue());
    port->setDisplayName(portNode.firstChildElement("ipxact:displayName").firstChild().nodeValue());
    port->setDescription(portNode.firstChildElement("ipxact:description").firstChild().nodeValue());

    parseWire(portNode, port);

    parseTransactional(portNode, port);

    parseVendorExtensions(portNode, port);

    return port;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parseWire()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parseWire(QDomNode const& portNode, QSharedPointer<PortAbstraction> port) const
{
    QDomNode wireNode = portNode.firstChildElement("ipxact:wire");

    if (!wireNode.isNull())
    {
        WireAbstractionReader wireReader;
        QSharedPointer<WireAbstraction> wire = wireReader.createWireAbstractionFrom(wireNode);

        port->setWire(wire);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionReader::parseTransactional()
//-----------------------------------------------------------------------------
void AbstractionDefinitionReader::parseTransactional(QDomNode const& portNode, 
    QSharedPointer<PortAbstraction> port) const
{
    QDomNode transactionalNode = portNode.firstChildElement("ipxact:transactional");

    if (!transactionalNode.isNull())
    {
        TransactionalAbstractionReader transactionalReader;
        QSharedPointer<TransactionalAbstraction> transactional =
            transactionalReader.createTransactionalAbstractionFrom(transactionalNode);

        port->setTransactional(transactional);
    }
}
