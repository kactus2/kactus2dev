//-----------------------------------------------------------------------------
// File: PacketWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2023
//
// Description:
// Writer for ipxact:packet within a port of an abstraction definition.
//-----------------------------------------------------------------------------

#include "PacketWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
// Function: PacketWriter::writePacket()
//-----------------------------------------------------------------------------
void PacketWriter::writePacket(QXmlStreamWriter& writer, QSharedPointer<Packet> packet)
{
    writer.writeStartElement(QStringLiteral("ipxact:packet"));

    NameGroupWriter::writeNameGroup(writer, packet);

    if (auto const& endianness = packet->getEndianness(); !endianness.isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:endianness"), endianness);
    }

    Details::writePacketFields(writer, packet);

    CommonItemsWriter::writeVendorExtensions(writer, packet);

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: PacketWriter::Details::writePacketFields()
//-----------------------------------------------------------------------------
void PacketWriter::Details::writePacketFields(QXmlStreamWriter& writer, QSharedPointer<Packet> packet)
{
    auto packetFields = packet->getPacketFields();

    if (packetFields->isEmpty())
    {
        return;
    }

    writer.writeStartElement(QStringLiteral("ipxact:packetFields"));

    for (auto const& packetField : *packetFields)
    {
        Details::writeSinglePacketField(writer, packetField);
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: PacketWriter::Details::writeSinglePacketField()
//-----------------------------------------------------------------------------
void PacketWriter::Details::writeSinglePacketField(QXmlStreamWriter& writer, QSharedPointer<PacketField> packetField)
{
    writer.writeStartElement(QStringLiteral("ipxact:packetField"));

    NameGroupWriter::writeNameGroup(writer, packetField);

    if (auto const& width = packetField->getWidth(); !width.isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:width"), width);
    }
    if (auto const& value = packetField->getValue(); !value.isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:value"), value);
    }
    if (auto const& endianness = packetField->getEndianness(); !endianness.isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:endianness"), endianness);
    }

    CommonItemsWriter::writeQualifier(writer, packetField->getQualifier());

    CommonItemsWriter::writeVendorExtensions(writer, packetField);

    writer.writeEndElement();
}
