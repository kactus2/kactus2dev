//-----------------------------------------------------------------------------
// File: MCAPIDesignerTypes.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.1.2011
//
// Description:
// Designer-specific MCAPI types and utility functions.
//-----------------------------------------------------------------------------

#ifndef MCAPIDESIGNERTYPES_H
#define MCAPIDESIGNERTYPES_H

#include <QString>

//-----------------------------------------------------------------------------
//! MCAPIContentType enumeration.
//-----------------------------------------------------------------------------
enum MCAPIContentType
{
    MCAPI_CONTENT_FUNC = 0,
    MCAPI_CONTENT_TYPENAME,
    MCAPI_CONTENT_NODE_ID,
    MCAPI_CONTENT_PORT_ID,
    MCAPI_CONTENT_ENDPOINT,
    MCAPI_CONTENT_TYPE_COUNT
};

//-----------------------------------------------------------------------------
//! MCAPIEndpointDirection enumeration.
//-----------------------------------------------------------------------------
enum MCAPIEndpointDirection
{
    MCAPI_ENDPOINT_IN = 0,
    MCAPI_ENDPOINT_OUT
};

/*!
 *  Converts a string to an endpoint direction.
 */
bool stringToValue(QString const& str, MCAPIEndpointDirection& dir);

//-----------------------------------------------------------------------------
//! MCAPIDataType enumeration.
//-----------------------------------------------------------------------------
enum MCAPIDataType
{
    MCAPI_TYPE_MESSAGE = 0,
    MCAPI_TYPE_PACKET,
    MCAPI_TYPE_SCALAR
};

/*!
 *  Converts a string to an MCAPI data type.
 */
bool stringToValue(QString const& str, MCAPIDataType& type);

//-----------------------------------------------------------------------------

#endif // MCAPIDESIGNERTYPES_H
