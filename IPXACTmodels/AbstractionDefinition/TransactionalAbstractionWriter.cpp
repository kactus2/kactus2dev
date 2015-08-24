//-----------------------------------------------------------------------------
// File: TransactionalAbstractionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "TransactionalAbstractionWriter.h"

#include "TransactionalAbstraction.h"
#include "TransactionalPort.h"

#include <IPXACTmodels/common/Protocol.h>

#include <IPXACTmodels/VendorExtension.h>

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
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    writer.writeStartElement("ipxact:transactional");

    writeQualifier(writer, transactional);

    writeSystem(writer, transactional);

    writeMaster(writer, transactional);

    writeSlave(writer, transactional);

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeQualifier()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeQualifier(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    Qualifier qualifier = transactional->getQualifier();
    if (qualifier.isData() || qualifier.isAddress())
    {
        writer.writeStartElement("ipxact:qualifier");
        if (qualifier.isData())
        {
            writer.writeTextElement("ipxact:isData", "true");
        }
        if (qualifier.isAddress())
        {
            writer.writeTextElement("ipxact:isAddress", "true");
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
        writer.writeStartElement("ipxact:onSystem");
        writer.writeTextElement("ipxact:group", systemPort->getSystemGroup());
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

    writeProtocol(writer, port);
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writePresence()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writePresence(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalPort> port) const
{
    if (port->getPresence() == General::REQUIRED)
    {
        writer.writeTextElement("ipxact:presence", "required");
    }
    else if (port->getPresence() == General::OPTIONAL)
    {
        writer.writeTextElement("ipxact:presence", "optional");
    }
    else if (port->getPresence() == General::ILLEGAL)
    {
        writer.writeTextElement("ipxact:presence", "illegal");
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
        writer.writeTextElement("ipxact:initiative", port->getInitiative());
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
        standardKinds << "tlm_port" << "tlm_socket" << "simple_socket" << "multi_socket";

        writer.writeStartElement("ipxact:kind");
        if (standardKinds.contains(port->getKind()))
        {
            writer.writeCharacters(port->getKind());
        }
        else
        {
            writer.writeAttribute("custom", port->getKind());
            writer.writeCharacters("custom");
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
        writer.writeTextElement("ipxact:busWidth", port->getBusWidth());
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeProtocol()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeProtocol(QXmlStreamWriter& writer,
    QSharedPointer<TransactionalPort> port) const
{
    if (port->hasProtocol())
    {
        QSharedPointer<Protocol> portProtocol = port->getProtocol();

        writer.writeStartElement("ipxact:protocol");

        writeProtocolType(writer, portProtocol);
        writePayload(writer, portProtocol);

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeProtocolType()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeProtocolType(QXmlStreamWriter& writer,
    QSharedPointer<Protocol> portProtocol) const
{
    if (portProtocol->getProtocolType() == "tlm")
    {
        writer.writeTextElement("ipxact:protocolType", "tlm");
    }
    else
    {
        writer.writeStartElement("ipxact:protocolType");
        writer.writeAttribute("custom", portProtocol->getProtocolType());
        writer.writeCharacters("custom");
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writePayload()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writePayload(QXmlStreamWriter& writer, 
    QSharedPointer<Protocol> portProtocol) const
{
    if (!portProtocol->getPayloadType().isEmpty())
    {
        writer.writeStartElement("ipxact:payload");

        if (!portProtocol->getPayloadName().isEmpty())
        {
            writer.writeTextElement("ipxact:name", portProtocol->getPayloadName());
        }

        writer.writeTextElement("ipxact:type", portProtocol->getPayloadType());

        if (!portProtocol->getPayloadExtension().isEmpty())
        {
            writer.writeStartElement("ipxact:extension");
         
            if (portProtocol->hasMandatoryPayloadExtension())
            {
                writer.writeAttribute("mandatory", "true");
            }
            writer.writeCharacters(portProtocol->getPayloadExtension());

            writer.writeEndElement();
        }

        writeVendorExtensions(writer, portProtocol);

        writer.writeEndElement();
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
        writer.writeStartElement("ipxact:vendorExtensions");
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
    QSharedPointer<TransactionalAbstraction> transactional) const
{
   if (transactional->hasMasterPort())
   {
       writer.writeStartElement("ipxact:onMaster");
       writeTransactionalPort(writer, transactional->getMasterPort());
       writer.writeEndElement();
   }
}

//-----------------------------------------------------------------------------
// Function: TransactionalAbstractionWriter::writeSlave()
//-----------------------------------------------------------------------------
void TransactionalAbstractionWriter::writeSlave(QXmlStreamWriter& writer, 
    QSharedPointer<TransactionalAbstraction> transactional) const
{
    if (transactional->hasSlavePort())
    {
        writer.writeStartElement("ipxact:onSlave");
        writeTransactionalPort(writer, transactional->getSlavePort());
        writer.writeEndElement();
    }
}
