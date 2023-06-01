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

#include <IPXACTmodels/common/VendorExtension.h>

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::TransactionalAbstractionWriter()
//-----------------------------------------------------------------------------
TransactionalAbstractionWriter::TransactionalAbstractionWriter(QObject *parent): QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::~TransactionalAbstractionWriter()
//-----------------------------------------------------------------------------
TransactionalAbstractionWriter::~TransactionalAbstractionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeTransactional()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeTransactional(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalAbstraction> transactional, Document::Revision revision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:transactional"));

    writeQualifier(writer, transactional);

    writeSystem(writer, transactional);

    QString initiatorElementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onInitiator")
        : QStringLiteral("ipxact:onMaster");

    QString targetElementName = revision == Document::Revision::Std22
        ? QStringLiteral("ipxact:onTarget")
        : QStringLiteral("ipxact:onSlave");

    writeMaster(writer, transactional, initiatorElementName);

    writeSlave(writer, transactional, targetElementName);

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeQualifier()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeQualifier(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    auto qualifier = transactional->getQualifier();
    if (qualifier->hasType(Qualifier::Data) || qualifier->hasType(Qualifier::Address))
    {
        writer.writeStartElement(QStringLiteral("ipxact:qualifier"));
        if (qualifier->hasType(Qualifier::Data))
        {
            writer.writeTextElement(QStringLiteral("ipxact:isData"), QStringLiteral("true"));
        }
        if (qualifier->hasType(Qualifier::Address))
        {
            writer.writeTextElement(QStringLiteral("ipxact:isAddress"), QStringLiteral("true"));
        }
     
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeSystem()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeSystem(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    foreach (QSharedPointer<TransactionalPort> systemPort, *transactional->getSystemPorts())
    {
        writer.writeStartElement(QStringLiteral("ipxact:onSystem"));
        writer.writeTextElement(QStringLiteral("ipxact:group"), systemPort->getSystemGroup());
        writeTransactionalPort(writer, systemPort);
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeTransactionalPort()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeTransactionalPort(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalPort> port) const
{
    writePresence(writer, port);

    writeInitiative(writer, port);

    writeKind(writer, port);

    writeBusWidth(writer, port);

    ProtocolWriter protocolWriter;
    protocolWriter.writeProtocol(writer, port->getProtocol());
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writePresence()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writePresence(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalPort> port) const
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
// Function: TransactionalAbstractionWriter::writeInitiative()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeInitiative(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalPort> port) const
{
    if (!port->getInitiative().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:initiative"), port->getInitiative());
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeKind()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeKind(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalPort> port) const
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
// Function: TransactionalAbstractionWriter::writeBusWidth()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeBusWidth(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalPort> port) const
{
    if (!port->getBusWidth().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:busWidth"), port->getBusWidth());
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeVendorExtensions(QXmlStreamWriter& writer, 
    QSharedPointer<Protocol> portProtocol) const
{
    if (!portProtocol->getVendorExtensions()->isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:vendorExtensions"));
        foreach (QSharedPointer<VendorExtension> extension, *portProtocol->getVendorExtensions())
        {
            extension->write(writer);
        }
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeMaster()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeMaster(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalAbstraction> transactional, QString const& elementName) const
{
   if (transactional->hasMasterPort())
   {
       writer.writeStartElement(elementName);
       writeTransactionalPort(writer, transactional->getMasterPort());
       writer.writeEndElement();
   }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeSlave()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeSlave(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalAbstraction> transactional, QString const& elementName) const
{
    if (transactional->hasSlavePort())
    {
        writer.writeStartElement(elementName);
        writeTransactionalPort(writer, transactional->getSlavePort());
        writer.writeEndElement();
    }
}
