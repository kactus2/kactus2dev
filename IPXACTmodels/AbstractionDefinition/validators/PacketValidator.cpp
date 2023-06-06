//-----------------------------------------------------------------------------
// File: PacketValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 06.06.2023
//
// Description:
// Validator for ipxact:packet.
//-----------------------------------------------------------------------------

#include "PacketValidator.h"

#include <IPXACTmodels/common/validators/QualifierValidator.h>

//-----------------------------------------------------------------------------
// Function: PacketValidator::validate()
//-----------------------------------------------------------------------------
bool PacketValidator::validate(QSharedPointer<Packet> packet)
{
    if (packet->name().isEmpty())
    {
        return false;
    }

    if (packet->getPacketFields()->isEmpty())
    {
        return false;
    }

    for (auto const& packetField : *packet->getPacketFields())
    {
        if (!Details::isValidPacketField(packetField))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PacketValidator::findErrorsIn(QStringList& errors, QSharedPointer<Packet> packet, QString const& context)
{
    if (packet->name().isEmpty())
    {
        errors.append(QObject::tr("Unnamed packet in %1").arg(context));
    }

    if (packet->getPacketFields()->isEmpty())
    {
        errors.append(QObject::tr("No packet field defined for packet %1 in %2").arg(packet->name(), context));
    }
    
    for (auto const& packetField : *packet->getPacketFields())
    {
        Details::findErrorsInPacketField(errors, context, packetField);
    }
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::Details::isValidPacketField()
//-----------------------------------------------------------------------------
bool PacketValidator::Details::isValidPacketField(QSharedPointer<PacketField> packetField)
{
    if (packetField->name().isEmpty())
    {
        return false;
    }

    if (packetField->getWidth().isEmpty())
    {
        return false;
    }


    // TODO: validate width element and value (SCR 6.49, 6.47)

    return true;
}

//-----------------------------------------------------------------------------
// Function: PacketValidator::Details::findErrorsInPacketField()
//-----------------------------------------------------------------------------
void PacketValidator::Details::findErrorsInPacketField(QStringList& errors, QString const& context, 
    QSharedPointer<PacketField> packetField)
{
    if (packetField->name().isEmpty())
    {
        errors.append(QObject::tr("No name defined for packet field in %1").arg(context));
    }

    if (packetField->getWidth().isEmpty())
    {
        errors.append(QObject::tr("No width defined for packet field %1 in %2").arg(packetField->name(), context));
    }

    // TODO: find errors in width and value
}
