//-----------------------------------------------------------------------------
// Protocol: ProtocolValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 07.12.2015
//
// Description:
// Validator for ipxact:Protocol.
//-----------------------------------------------------------------------------

#include "ProtocolValidator.h"
#include "CommonItemsValidator.h"

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ProtocolValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool ProtocolValidator::validate(QSharedPointer<Protocol> protocol) const
{
	if (protocol->getProtocolType() != QLatin1String("tlm") && 
        protocol->getProtocolType() != QLatin1String("custom"))
	{
		return false;
	}

    if (protocol->hasPayload())
    {
        if (!protocol->getPayloadName().isEmpty() && !hasValidName(protocol->getPayloadName()))
        {
            return false;
        }

        if (protocol->getPayloadType() != QLatin1String("generic") && 
            protocol->getPayloadType() != QLatin1String("specific"))
        {
            return false;
        }
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: ProtocolValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ProtocolValidator::findErrorsIn(QVector<QString>& errors, 
    QSharedPointer<Protocol> protocol, QString const&) const
{
	if (protocol->getProtocolType() != QLatin1String("tlm") &&
        protocol->getProtocolType() != QLatin1String("custom"))
	{
		errors.append(QObject::tr("The transactional protocol type '%1' is invalid.").arg(
            protocol->getProtocolType()));
    }

    if (protocol->hasPayload())
    {
        if (!protocol->getPayloadName().isEmpty() && !hasValidName(protocol->getPayloadName()))
        {
            errors.append(QObject::tr("The payload name '%1' is invalid.").arg(protocol->getPayloadName()));
        }

        if (protocol->getPayloadType() != QLatin1String("generic") && 
            protocol->getPayloadType() != QLatin1String("specific"))
        {
            errors.append(QObject::tr("The payload type '%1' is invalid.").arg(protocol->getPayloadType()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ProtocolValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ProtocolValidator::hasValidName(QString const& name) const
{
    return CommonItemsValidator::hasValidName(name);
}
