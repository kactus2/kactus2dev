//-----------------------------------------------------------------------------
// File: PacketValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 06.06.2023
//
// Description:
// Validator for ipxact:packet.
//-----------------------------------------------------------------------------

#ifndef PACKETVALIDATOR_H
#define PACKETVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/AbstractionDefinition/Packet.h>

#include <QString>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Validator for ipxact:packet.
//-----------------------------------------------------------------------------
namespace PacketValidator
{
    /*!
     *  Validates the given Packet.
     *
     *      @param [in] packet  The packet to validate.
     *
     *      @return True, if the Packet is valid IP-XACT, otherwise false.
     */
    IPXACTMODELS_EXPORT bool validate(QSharedPointer<Packet> packet);

    /*!
     *  Finds possible errors in a Packet and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] packet      The AbstractionDefinition whose errors to find.
     *      @param [in] context     Context to help locate the error.
     */
    IPXACTMODELS_EXPORT void findErrorsIn(QStringList& errors, QSharedPointer<Packet> packet,
        QString const& context);

    namespace Details
    {
        bool isValidPacketField(QSharedPointer<PacketField> packetField);

        void findErrorsInPacketField(QStringList& errors, QString const& context, QSharedPointer<PacketField> packetField);
    }
}

#endif // PACKETVALIDATOR_H
