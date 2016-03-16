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
        writer.writeStartElement("ipxact:protocol");

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
    if (portProtocol->getProtocolType() == "tlm")
    {
        writer.writeTextElement("ipxact:protocolType", "tlm");
    }
    else
    {
        writer.writeStartElement("ipxact:protocolType");
        writer.writeAttribute("custom", portProtocol->getCustomProtocolType());
        writer.writeCharacters("custom");
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
// Function: ProtocolWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void ProtocolWriter::writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const
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
