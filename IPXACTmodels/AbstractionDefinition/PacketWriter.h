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

#ifndef PACKETWRITER_H
#define PACKETWRITER_H

#include "PacketField.h"
#include "Packet.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Writer for ipxact:packet within ports of an abstraction definition.
//-----------------------------------------------------------------------------
namespace PacketWriter
{
    IPXACTMODELS_EXPORT void writePacket(QXmlStreamWriter& writer, QSharedPointer<Packet> packet);

    namespace Details
    {
        void writePacketFields(QXmlStreamWriter& writer, QSharedPointer<Packet> packet);

        void writeSinglePacketField(QXmlStreamWriter& writer, QSharedPointer<PacketField> packetField);
    }
}

#endif // PACKETWRITER_H

