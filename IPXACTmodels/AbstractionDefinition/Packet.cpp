//-----------------------------------------------------------------------------
// File: Packet.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 25.05.2023
//
// Description:
// Implementation for ipxact:packet element within abstraction definition.
//-----------------------------------------------------------------------------

#include "Packet.h"

//-----------------------------------------------------------------------------
// Function: Packet::Packet()
//-----------------------------------------------------------------------------
Packet::Packet(QString const& name):
NameGroup(name)
{
}

//-----------------------------------------------------------------------------
// Function: Packet::getEndianness()
//-----------------------------------------------------------------------------
QString Packet::getEndianness() const
{
    return endianness_;
}

//-----------------------------------------------------------------------------
// Function: Packet::setEndianness()
//-----------------------------------------------------------------------------
void Packet::setEndianness(QString const& endianness)
{
    endianness_ = endianness;
}

//-----------------------------------------------------------------------------
// Function: Packet::getPacketFields()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PacketField> > > Packet::getPacketFields() const
{
    return packetFields_;
}

//-----------------------------------------------------------------------------
// Function: Packet::setPacketFields()
//-----------------------------------------------------------------------------
void Packet::setPacketFields(QSharedPointer<QList<QSharedPointer<PacketField>>> packetFields)
{
    packetFields_ = packetFields;
}
