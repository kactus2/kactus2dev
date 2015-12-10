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

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/common/Protocol.h>
#include <IPXACTmodels/common/ProtocolReader.h>

#include <IPXACTmodels/common/PresenceTypes.h>

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
    transactionalPort->setPresence(PresenceTypes::str2Presence(presenceNode.firstChild().nodeValue(),
        PresenceTypes::UNKNOWN));

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
        ProtocolReader protocolReader;
        transactionalPort->setProtocol(protocolReader.createProtocolFrom(protocolNode));
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
