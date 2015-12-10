//-----------------------------------------------------------------------------
// PresenceTypes: PresenceTypes.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 08.12.2015
//
// Description:
// Contains definitions and functions for presence types.
//-----------------------------------------------------------------------------

#ifndef PRESENCETYPES_H
#define PRESENCETYPES_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

namespace PresenceTypes
{
    /*!
    * Provides the capability to require or forbid a port to appear in a
    * bus interface.
    */
	enum Presence
	{
		ILLEGAL = 0,
		OPTIONAL,
		REQUIRED,
		UNKNOWN
    };

    /*! Convert the value of the QString into enum Presence
    *
    * if The str does not match any of the enum values the default value
    * specifies as parameter will be returned
    *
    *      @param [in] str QString containing the string to be converted
    *      @param [in] defaultValue The value that will be returned if no match is found
    *
    *      @return General::Presence that matches the str or default value.
    */
    IPXACTMODELS_EXPORT Presence str2Presence(QString str, Presence defaultValue);

    /*! Convert the enum presence to QString.
    *
    *      @param [in] presence The enum value to be converted
    *
    *      @return QString matching the enum value.
    */
    IPXACTMODELS_EXPORT QString presence2Str(Presence presence);
}

#endif