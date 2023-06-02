//-----------------------------------------------------------------------------
// File: PacketReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 30.05.2023
//
// Description:
// Reader for ipxact:packet within a port of an abstraction definition.
//-----------------------------------------------------------------------------

#include "PacketReader.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
// Function: PacketReader::createPacketFrom()
//-----------------------------------------------------------------------------
QSharedPointer<Packet> PacketReader::createPacketFrom(QDomNode const& packetNode)
{
    QSharedPointer<Packet> packet(new Packet);

    Details::parseNameGroup(packetNode, packet);
    
    packet->setEndianness(packetNode.firstChildElement(QStringLiteral("ipxact:endianness")).firstChild().nodeValue());

    Details::parsePacketFields(packetNode, packet);

    CommonItemsReader::parseVendorExtensions(packetNode, packet);

    return packet;
}

//-----------------------------------------------------------------------------
// Function: PacketReader::Details::parseNameGroup()
//-----------------------------------------------------------------------------
void PacketReader::Details::parseNameGroup(QDomNode const& packetNode, QSharedPointer<Packet> packet)
{
    packet->setName(packetNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue());
    packet->setDisplayName(packetNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
    packet->setShortDescription(packetNode.firstChildElement(QStringLiteral("ipxact:shortDescription")).firstChild().nodeValue());
    packet->setDescription(packetNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: PacketReader::Details::parsePacketFields()
//-----------------------------------------------------------------------------
void PacketReader::Details::parsePacketFields(QDomNode const& packetNode, QSharedPointer<Packet> packet)
{
    auto packetFields = packetNode.firstChildElement(QStringLiteral("ipxact:packetFields"))
        .elementsByTagName(QStringLiteral("ipxact:packetField"));

    QSharedPointer<QList<QSharedPointer<PacketField> > > packetPacketFields(
        new QList<QSharedPointer<PacketField> >());

    for (int i = 0; i < packetFields.count(); ++i)
    {
        auto fieldNode = packetFields.at(i);
        
        parseSinglePacketField(fieldNode, packetPacketFields);
    }

    packet->setPacketFields(packetPacketFields);
}

//-----------------------------------------------------------------------------
// Function: PacketReader::Details::parseSinglePacketField()
//-----------------------------------------------------------------------------
void PacketReader::Details::parseSinglePacketField(QDomNode const& fieldNode, QSharedPointer<QList<QSharedPointer<PacketField> > > packetFields)
{
    QSharedPointer<PacketField> parsedPacketField(new PacketField());

    parsedPacketField->setName(fieldNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue());
    parsedPacketField->setDisplayName(
        fieldNode.firstChildElement(QStringLiteral("ipxact:displayName")).firstChild().nodeValue());
    parsedPacketField->setShortDescription(
        fieldNode.firstChildElement(QStringLiteral("ipxact:shortDescription")).firstChild().nodeValue());
    parsedPacketField->setDescription(
        fieldNode.firstChildElement(QStringLiteral("ipxact:description")).firstChild().nodeValue());
    parsedPacketField->setWidth(
        fieldNode.firstChildElement(QStringLiteral("ipxact:width")).firstChild().nodeValue());
    parsedPacketField->setValue(
        fieldNode.firstChildElement(QStringLiteral("ipxact:value")).firstChild().nodeValue());
    parsedPacketField->setEndianness(
        fieldNode.firstChildElement(QStringLiteral("ipxact:endianness")).firstChild().nodeValue());

    QSharedPointer<Qualifier> fieldQualifier(new Qualifier());
    CommonItemsReader::parseQualifier(
        fieldNode.firstChildElement(QStringLiteral("ipxact:qualifier")), fieldQualifier, Document::Revision::Std22);

    parsedPacketField->setQualifier(fieldQualifier);

    CommonItemsReader::parseVendorExtensions(fieldNode, parsedPacketField);

    packetFields->append(parsedPacketField);
}
