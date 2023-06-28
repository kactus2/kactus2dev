//-----------------------------------------------------------------------------
// File: TransactionalAbstractionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.08.2015
//
// Description:
// Reader for ipxact:transactional within abstraction definition.
//-----------------------------------------------------------------------------

#include "TransactionalAbstractionReader.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/common/Protocol.h>
#include <IPXACTmodels/common/ProtocolReader.h>

#include <IPXACTmodels/common/PresenceTypes.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include "TransactionalAbstraction.h"
#include "TransactionalPort.h"

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::createTransactionalAbstractionFrom()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalAbstraction> TransactionalAbstractionReader::createTransactionalAbstractionFrom(
    QDomNode const& transactionalNode, Document::Revision revision)
{
    QSharedPointer<TransactionalAbstraction> transactional(new TransactionalAbstraction());

    Details::parseQualifier(transactionalNode, transactional, revision);
 
    Details::parseSystems(transactionalNode, transactional);
 
    Details::parseInitiator(transactionalNode, transactional, revision);

    Details::parseTarget(transactionalNode, transactional, revision);

    return transactional;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::Details::parseQualifier()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::Details::parseQualifier(QDomNode const& transactionalNode, 
    QSharedPointer<TransactionalAbstraction> transactional, Document::Revision revision)
{
    QDomNode qualifierNode = transactionalNode.firstChildElement(QStringLiteral("ipxact:qualifier"));

    auto qualifier = transactional->getQualifier();

    CommonItemsReader::parseQualifier(qualifierNode, qualifier, revision);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::Details::parseSystems()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::Details::parseSystems(QDomNode const& transactionalNode, 
    QSharedPointer<TransactionalAbstraction> transactional)
{
    QDomNodeList systemNodes = transactionalNode.toElement().elementsByTagName(QStringLiteral("ipxact:onSystem"));

    for (int i = 0; i < systemNodes.count(); i++)
    {
        QDomNode systemNode = systemNodes.at(i);

        QSharedPointer<TransactionalPort> system = parseTransactionalPort(systemNode);
        system->setSystemGroup(systemNode.firstChildElement(QStringLiteral("ipxact:group")).firstChild().nodeValue());
        transactional->addSystemPort(system);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::Details::parseTransactionalPort()
//-----------------------------------------------------------------------------
QSharedPointer<TransactionalPort> TransactionalAbstractionReader::Details::parseTransactionalPort(QDomNode const& portNode)
{
    QSharedPointer<TransactionalPort> transactionalPort(new TransactionalPort());

    QDomNode presenceNode = portNode.firstChildElement(QStringLiteral("ipxact:presence"));
    transactionalPort->setPresence(PresenceTypes::str2Presence(presenceNode.firstChild().nodeValue(),
        PresenceTypes::UNKNOWN));

    transactionalPort->setInitiative(portNode.firstChildElement(QStringLiteral("ipxact:initiative")).firstChild().nodeValue());

    transactionalPort->setKind(portNode.firstChildElement(QStringLiteral("ipxact:kind")).firstChild().nodeValue());

    transactionalPort->setBusWidth(portNode.firstChildElement(QStringLiteral("ipxact:busWidth")).firstChild().nodeValue());

    parseProtocol(portNode, transactionalPort);

    return transactionalPort;
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::Details::parseProtocol()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::Details::parseProtocol(QDomNode const& portNode,
    QSharedPointer<TransactionalPort> transactionalPort)
{
    QDomNode protocolNode = portNode.firstChildElement(QStringLiteral("ipxact:protocol"));

    if (!protocolNode.isNull())
    {
        ProtocolReader protocolReader;
        transactionalPort->setProtocol(protocolReader.createProtocolFrom(protocolNode));
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::Details::parseInitiator()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::Details::parseInitiator(QDomNode const& transactionalNode, 
    QSharedPointer<TransactionalAbstraction> transactional, Document::Revision revision)
{
    QString elementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onInitiator")
        : QStringLiteral("ipxact:onMaster");

    QDomNode initiatorNode = transactionalNode.firstChildElement(elementName);

    if (!initiatorNode.isNull())
    {
        QSharedPointer<TransactionalPort> initiator = parseTransactionalPort(initiatorNode);
        transactional->setInitiatorPort(initiator);
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionReader::Details::parseTarget()
//-----------------------------------------------------------------------------
void TransactionalAbstractionReader::Details::parseTarget(QDomNode const& transactionalNode, 
    QSharedPointer<TransactionalAbstraction> transactional, Document::Revision revision)
{
    QString elementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onTarget")
        : QStringLiteral("ipxact:onSlave");

    QDomNode targetNode = transactionalNode.firstChildElement(elementName);

    if (!targetNode.isNull())
    {
        QSharedPointer<TransactionalPort> target = parseTransactionalPort(targetNode);
        transactional->setTargetPort(target);
    }
}
