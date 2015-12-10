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

PresenceTypes::Presence PresenceTypes::str2Presence(QString str, PresenceTypes::Presence defaultValue)
{
	// Identify the correct enum value.
	if (str == QString("illegal"))
	{
		return PresenceTypes::ILLEGAL;
	}
	else if (str == QString("required"))
	{
		return PresenceTypes::REQUIRED;
	}
	else if (str == QString("optional"))
	{
		return PresenceTypes::OPTIONAL;
	}
	else
	{
		return defaultValue;
	}
}

QString PresenceTypes::presence2Str(PresenceTypes::Presence presence)
{
	// Create string based on the enum value.
	switch (presence)
	{
		case PresenceTypes::ILLEGAL:
		{
			return QString("illegal");
		}
		case PresenceTypes::REQUIRED:
		{
			return QString("required");
		}
		case PresenceTypes::OPTIONAL:
		{
			return QString("optional");
		}
		default:
		{
			return QString();
		}
	}
}

