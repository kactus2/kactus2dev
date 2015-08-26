//-----------------------------------------------------------------------------
// File: TransactionalAbstractionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.08.2015
//
// Description:
// Reader class for ipxact:transactional within abstraction definition.
//-----------------------------------------------------------------------------

#include "TransactionalAbstractionReader.h"

#include <IPXACTmodels/GenericVendorExtension.h>

#include <IPXACTmodels/common/Protocol.h>

#include <IPXACTmodels/generaldeclarations.h>

#include "TransactionalAbstraction.h"
#include "TransactionalPort.h"

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::TransactionalAbstractionReader()
//-----------------------------------------------------------------------------
TransactionalAbstractionReader::TransactionalAbstractionReader(QObject* parent) : QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::~TransactionalAbstractionReader()
//-----------------------------------------------------------------------------
TransactionalAbstractionReader::~TransactionalAbstractionReader()
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::createTransactionalAbstractionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalAbstraction> TransactionalAbstractionReader::createTransactionalAbstractionFrom(
    QDomNode const& transactionalNode) const
{
    QSharedPointer<TransactionalAbstraction> transactional(new TransactionalAbstraction());

    parseQualifier(transactionalNode, transactional);
 
    parseSystems(transactionalNode, transactional);
 
    parseMaster(transactionalNode, transactional);

    parseSlave(transactionalNode, transactional);

    return transactional;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::parseQualifier()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::parseQualifier(QDomNode const& transactionalNode, 
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    QDomNode qualifierNode = transactionalNode.firstChildElement("ipxact:qualifier");

    if (!qualifierNode.isNull())
    {
        Qualifier readQualifier;
        bool isData = qualifierNode.firstChildElement("ipxact:isData").firstChild().nodeValue() == "true";
        bool isAddress = qualifierNode.firstChildElement("ipxact:isAddress").firstChild().nodeValue() == "true";
        
        if (isData && isAddress)
        {
             transactional->setQualifier(Qualifier::Data_Address);
        }
        else if (isData)
        {
            transactional->setQualifier(Qualifier::Data);
        }
        else if (isAddress)
        {
            transactional->setQualifier(Qualifier::Address);
        }        
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::parseSystems()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::parseSystems(QDomNode const& transactionalNode, 
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    QDomNodeList systemNodes = transactionalNode.toElement().elementsByTagName("ipxact:onSystem");

    for (int i = 0; i < systemNodes.count(); i++)
    {
        QDomNode systemNode = systemNodes.at(i);

        QSharedPointer<TransactionalPort> system = parseTransactionalPort(systemNode);
        system->setSystemGroup(systemNode.firstChildElement("ipxact:group").firstChild().nodeValue());
        transactional->addSystemPort(system);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::parseTransactionalPort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstractionReader::parseTransactionalPort(QDomNode const& portNode) const
{
    QSharedPointer<TransactionalPort> transactionalPort(new TransactionalPort());

    QDomNode presenceNode = portNode.firstChildElement("ipxact:presence");
    transactionalPort->setPresence(General::str2Presence(presenceNode.firstChild().nodeValue(),
        General::PRESENCE_UNKNOWN));

    transactionalPort->setInitiative(portNode.firstChildElement("ipxact:initiative").firstChild().nodeValue());

    transactionalPort->setKind(portNode.firstChildElement("ipxact:kind").firstChild().nodeValue());

    transactionalPort->setBusWidth(portNode.firstChildElement("ipxact:busWidth").firstChild().nodeValue());

    parseProtocol(portNode, transactionalPort);

    return transactionalPort;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::parseProtocol()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::parseProtocol(QDomNode const& portNode,
    QSharedPointer<TransactionalPort> transactionalPort) const
{
    QDomNode protocolNode = portNode.firstChildElement("ipxact:protocol");

    if (!protocolNode.isNull())
    {
        QSharedPointer<Protocol> protocol(new Protocol());

        protocol->setProtocolType(protocolNode.firstChildElement("ipxact:protocolType").firstChild().nodeValue());

        QDomNode payloadNode = protocolNode.firstChildElement("ipxact:payload");

        protocol->setPayloadName(payloadNode.firstChildElement("ipxact:name").firstChild().nodeValue());
        protocol->setPayloadType(payloadNode.firstChildElement("ipxact:type").firstChild().nodeValue());

        bool mandatoryExtension = payloadNode.firstChildElement("ipxact:extension").attribute("mandatory") == "true";
        protocol->setPayloadExtension(payloadNode.firstChildElement("ipxact:extension").firstChild().nodeValue(),
            mandatoryExtension);

        parseVendorExtensions(payloadNode, protocol);

        transactionalPort->setProtocol(protocol);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::parseVendorExtensions(QDomNode const& payloadNode, 
    QSharedPointer<Protocol> protocol) const
{
    QDomNodeList extensionNodes = payloadNode.firstChildElement("ipxact:vendorExtensions").childNodes();

    int extensionCount = extensionNodes.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNodes.at(i)));
        protocol->getVendorExtensions()->append(extension);
    }
}


//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::parseMaster()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::parseMaster(QDomNode const& transactionalNode, 
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    QDomNode masterNode = transactionalNode.firstChildElement("ipxact:onMaster");

    if (!masterNode.isNull())
    {
        QSharedPointer<TransactionalPort> master = parseTransactionalPort(masterNode);
        transactional->setMasterPort(master);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::parseSlave()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::parseSlave(QDomNode const& transactionalNode, 
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    QDomNode slaveNode = transactionalNode.firstChildElement("ipxact:onSlave");

    if (!slaveNode.isNull())
    {
        QSharedPointer<TransactionalPort> slave = parseTransactionalPort(slaveNode);
        transactional->setSlavePort(slave);
    }
}
