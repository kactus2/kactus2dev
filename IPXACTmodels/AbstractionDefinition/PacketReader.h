//-----------------------------------------------------------------------------
// File: PacketReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 30.05.2023
//
// Description:
// Reader for ipxact:packet within a port of an abstraction definition.
//-----------------------------------------------------------------------------

#ifndef PACKETREADER_H
#define PACKETREADER_H

#include "PacketField.h"
#include "Packet.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader for ipxact:packet within ports of an abstraction definition.
//-----------------------------------------------------------------------------
namespace PacketReader
{
    /*!
     *  Creates a packet from XML description.
     *
     *      @param [in] packetNode      The XML document node to create the packet from.
     *
     *      @return The created packet.
     */
    QSharedPointer<Packet> createPacketFrom(QDomNode const& packetNode);

    namespace Details
    {
        /*!
         *  Parse the packet name group.
         *
         *      @param [in] packetNode      The XML document node of the packet.
         *      @param [in/out] packet      The packet definition to write to.
         */
        void parseNameGroup(QDomNode const& packetNode, QSharedPointer<Packet> packet);

        /*!
         *  Parse the packet fields of the packet.
         *
         *      @param [in] packetNode      The XML document node of the packet.
         *      @param [in/out] packet      The packet definition to inserts packet fields into.
         */
        void parsePacketFields(QDomNode const& packetNode, QSharedPointer<Packet> packet);

        /*!
         *	Parse a single packet field.
         *  
        *      @param [in] fieldNode	        The XML description of the packet field.
         *      @param [in/out] packetFields	The list of packet field definitions.
         */
        void parseSinglePacketField(QDomNode const& fieldNode,
            QSharedPointer<QList<QSharedPointer<PacketField> > > packetFields);
    }
}

#endif // PACKETREADER_H

