//-----------------------------------------------------------------------------
// File: TransactionalAbstractionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Writer class for IP-XACT Transactional element within abstraction definition.
//-----------------------------------------------------------------------------

#include "TransactionalAbstractionWriter.h"

#include "TransactionalAbstraction.h"
#include "TransactionalPort.h"

#include <IPXACTmodels/common/Protocol.h>
#include <IPXACTmodels/common/ProtocolWriter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <IPXACTmodels/common/VendorExtension.h>

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeTransactional()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeTransactional(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalAbstraction> transactional, Document::Revision revision)
{
    writer.writeStartElement(QStringLiteral("ipxact:transactional"));

    Details::writeQualifier(writer, transactional);

    Details::writeSystem(writer, transactional);

    QString initiatorElementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onInitiator")
        : QStringLiteral("ipxact:onMaster");

    QString targetElementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onTarget")
        : QStringLiteral("ipxact:onSlave");

    Details::writeMaster(writer, transactional, initiatorElementName);

    Details::writeSlave(writer, transactional, targetElementName);

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeQualifier()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeQualifier(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalAbstraction> transactional)
{
    auto qualifier = transactional->getQualifier();
    
    CommonItemsWriter::writeQualifier(writer, qualifier);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeSystem()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeSystem(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalAbstraction> transactional)
{
    for (auto systemPort : *transactional->getSystemPorts())
    {
        writer.writeStartElement(QStringLiteral("ipxact:onSystem"));
        writer.writeTextElement(QStringLiteral("ipxact:group"), systemPort->getSystemGroup());
        writeTransactionalPort(writer, systemPort);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeTransactionalPort()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeTransactionalPort(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalPort> port)
{
    writePresence(writer, port);

    writeInitiative(writer, port);

    writeKind(writer, port);

    writeBusWidth(writer, port);

    ProtocolWriter protocolWriter;
    protocolWriter.writeProtocol(writer, port->getProtocol());
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writePresence()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writePresence(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalPort> port)
{
    if (port->getPresence() == PresenceTypes::REQUIRED)
    {
        writer.writeTextElement(QStringLiteral("ipxact:presence"), QStringLiteral("required"));
    }
    else if (port->getPresence() == PresenceTypes::OPTIONAL)
    {
        writer.writeTextElement(QStringLiteral("ipxact:presence"), QStringLiteral("optional"));
    }
    else if (port->getPresence() == PresenceTypes::ILLEGAL)
    {
        writer.writeTextElement(QStringLiteral("ipxact:presence"), QStringLiteral("illegal"));
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeInitiative()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeInitiative(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalPort> port)
{
    CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:initiative"), port->getInitiative());
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeKind()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeKind(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalPort> port)
{
    if (!port->getKind().isEmpty())
    {
        QStringList standardKinds;
        standardKinds << QStringLiteral("tlm_port") << QStringLiteral("tlm_socket") << 
            QStringLiteral("simple_socket") << QStringLiteral("multi_socket");

        writer.writeStartElement(QStringLiteral("ipxact:kind"));
        if (standardKinds.contains(port->getKind()))
        {
            writer.writeCharacters(port->getKind());
        }
        else
        {
            writer.writeAttribute(QStringLiteral("custom"), port->getKind());
            writer.writeCharacters(QStringLiteral("custom"));
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeBusWidth()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeBusWidth(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalPort> port)
{
    if (!port->getBusWidth().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:busWidth"), port->getBusWidth());
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeVendorExtensions()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeVendorExtensions(QXmlStreamWriter& writer, 
    QSharedPointer<Protocol> portProtocol)
{
    if (!portProtocol->getVendorExtensions()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:vendorExtensions"));
        for (auto extension : *portProtocol->getVendorExtensions())
        {
            extension->write(writer);
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeMaster()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeMaster(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalAbstraction> transactional, QString const& elementName)
{
   if (transactional->hasMasterPort())
   {
       writer.writeStartElement(elementName);
       writeTransactionalPort(writer, transactional->getMasterPort());
       writer.writeEndElement();
   }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::Details::writeSlave()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::Details::writeSlave(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalAbstraction> transactional, QString const& elementName)
{
    if (transactional->hasSlavePort())
    {
        writer.writeStartElement(elementName);
        writeTransactionalPort(writer, transactional->getSlavePort());
        writer.writeEndElement();
    }
}
