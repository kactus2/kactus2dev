//-----------------------------------------------------------------------------
// PresenceTypes: PresenceTypes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 08.12.2015
//
// Description:
// Contains definitions and functions for presence types.
//-----------------------------------------------------------------------------

#include "PresenceTypes.h"

//-----------------------------------------------------------------------------
// Function: PresenceTypes::str2Presence()
//-----------------------------------------------------------------------------
PresenceTypes::Presence PresenceTypes::str2Presence(QString str, PresenceTypes::Presence defaultValue)
{
	// Identify the correct enum value.
	if (str == QLatin1String("illegal"))
	{
		return PresenceTypes::ILLEGAL;
	}
	else if (str == QLatin1String("required"))
	{
		return PresenceTypes::REQUIRED;
	}
	else if (str == QLatin1String("optional"))
	{
		return PresenceTypes::OPTIONAL;
	}
	else
	{
		return defaultValue;
	}
}

//-----------------------------------------------------------------------------
// Function: PresenceTypes::presence2Str()
//-----------------------------------------------------------------------------
QString PresenceTypes::presence2Str(PresenceTypes::Presence presence)
{
    // Create string based on the enum value.
    if (presence == PresenceTypes::ILLEGAL)
    {
        return QStringLiteral("illegal");
    }
    else if (presence == PresenceTypes::REQUIRED)
    {
        return QStringLiteral("required");
    }
    else if (presence == PresenceTypes::OPTIONAL)
    {
        return QStringLiteral("optional");
    }
    else
    {
        return QString();
    }
}
