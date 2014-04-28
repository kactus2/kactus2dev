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
    MCAPI_CONTENT_ENDPOINT_HANDLE,
    MCAPI_CONTENT_TYPE_COUNT
};

//-----------------------------------------------------------------------------

#endif // MCAPIDESIGNERTYPES_H
