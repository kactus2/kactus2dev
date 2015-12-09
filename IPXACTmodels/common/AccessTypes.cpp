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
    if (str == QString("read-write"))
    {
		return AccessTypes::READ_WRITE;
	}
	else if (str == QString("read-only"))
    {
		return AccessTypes::READ_ONLY;
	}
	else if (str ==	QString("write-only"))
    {
		return AccessTypes::WRITE_ONLY;
	}
	else if (str ==	QString("read-writeOnce"))
    {
		return AccessTypes::READ_WRITEONCE;
	}
	else if (str == QString("writeOnce"))
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
        return QString("read-write");
    }
    else if (access == AccessTypes::READ_ONLY)
    {
        return QString("read-only");
    }
    else if (access == AccessTypes::WRITE_ONLY)
    {
        return QString("write-only");
    }
    else if (access == AccessTypes::READ_WRITEONCE)
    {
        return QString("read-writeOnce");
    }
    else if (access == AccessTypes::WRITEONCE)
    {
        return QString("writeOnce");
    }
    else
    {
        return QString();
    }
}