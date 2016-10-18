//-----------------------------------------------------------------------------
// File: AccessTypes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.12.2015
//
// Description:
// Namespace AccessTypes is used to store functions related to memory access.
//-----------------------------------------------------------------------------

#include "AccessTypes.h"

//-----------------------------------------------------------------------------
// Function: AccessTypes::str2Access()
//-----------------------------------------------------------------------------
AccessTypes::Access AccessTypes::str2Access(QString str, AccessTypes::Access defaultValue)
{
    if (str == QLatin1String("read-write"))
    {
		return AccessTypes::READ_WRITE;
	}
	else if (str == QLatin1String("read-only"))
    {
		return AccessTypes::READ_ONLY;
	}
	else if (str ==	QLatin1String("write-only"))
    {
		return AccessTypes::WRITE_ONLY;
	}
	else if (str ==	QLatin1String("read-writeOnce"))
    {
		return AccessTypes::READ_WRITEONCE;
	}
	else if (str == QLatin1String("writeOnce"))
    {
		return AccessTypes::WRITEONCE;
	}
	else
    {
		return defaultValue;
	}
}

//-----------------------------------------------------------------------------
// Function: AccessTypes::access2Str()
//-----------------------------------------------------------------------------
QString AccessTypes::access2Str(const AccessTypes::Access access)
{
    if (access == AccessTypes::READ_WRITE)
    {
        return QStringLiteral("read-write");
    }
    else if (access == AccessTypes::READ_ONLY)
    {
        return QStringLiteral("read-only");
    }
    else if (access == AccessTypes::WRITE_ONLY)
    {
        return QStringLiteral("write-only");
    }
    else if (access == AccessTypes::READ_WRITEONCE)
    {
        return QStringLiteral("read-writeOnce");
    }
    else if (access == AccessTypes::WRITEONCE)
    {
        return QStringLiteral("writeOnce");
    }
    else
    {
        return QStringLiteral("");
    }
}