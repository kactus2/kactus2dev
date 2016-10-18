//-----------------------------------------------------------------------------
// File: ProtocolWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.09.2015
//
// Description:
// Writer class for IP-XACT protocol element.
//-----------------------------------------------------------------------------

#include "ProtocolWriter.h"
#include "Protocol.h"

#include <IPXACTmodels/common/VendorExtension.h>

//-----------------------------------------------------------------------------
// Function: ProtocolWriter::ProtocolWriter()
//-----------------------------------------------------------------------------
ProtocolWriter::ProtocolWriter(QObject* parent) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: ProtocolWriter::~ProtocolWriter()
//-----------------------------------------------------------------------------
ProtocolWriter::~ProtocolWriter()
{

}

//-----------------------------------------------------------------------------
// Function: ProtocolWriter::writePort()
//-----------------------------------------------------------------------------
void ProtocolWriter::writeProtocol(QXmlStreamWriter& writer, QSharedPointer<Protocol> protocol) const
{
    if (!protocol.isNull())
    {
        writer.writeStartElement(QStringLiteral("ipxact:protocol"));

        writeProtocolType(writer, protocol);
        writePayload(writer, protocol);

        writer.writeEndElement(); // ipxact:protocol
    }
}

//-----------------------------------------------------------------------------
// Function: ProtocolWriter::writeProtocolType()
//-----------------------------------------------------------------------------
void ProtocolWriter::writeProtocolType(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const
{
    if (portProtocol->getProtocolType() == QLatin1String("tlm"))
    {
        writer.writeTextElement(QStringLiteral("ipxact:protocolType"), QStringLiteral("tlm"));
    }
    else
    {
        writer.writeStartElement(QStringLiteral("ipxact:protocolType"));
        writer.writeAttribute(QStringLiteral("custom"), portProtocol->getCustomProtocolType());
        writer.writeCharacters(QStringLiteral("custom"));
        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: ProtocolWriter::writePayload()
//-----------------------------------------------------------------------------
void ProtocolWriter::writePayload(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const
{
    if (!portProtocol->getPayloadType().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:payload"));

        if (!portProtocol->getPayloadName().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:name"), portProtocol->getPayloadName());
        }

        writer.writeTextElement(QStringLiteral("ipxact:type"), portProtocol->getPayloadType());

        if (!portProtocol->getPayloadExtension().isEmpty())
        {
            writer.writeStartElement(QStringLiteral("ipxact:extension"));
         
            if (portProtocol->hasMandatoryPayloadExtension())
            {
                writer.writeAttribute(QStringLiteral("mandatory"), QStringLiteral("true"));
            }
            writer.writeCharacters(portProtocol->getPayloadExtension());

            writer.writeEndElement();
        }

        writeVendorExtensions(writer, portProtocol);

        writer.writeEndElement();
    }
}

//-----------------------------------------------------------------------------
// Function: ProtocolWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void ProtocolWriter::writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const
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
