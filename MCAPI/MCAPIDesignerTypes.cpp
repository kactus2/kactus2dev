//-----------------------------------------------------------------------------
// File: MCAPIDesignerTypes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.9.2011
//
// Description:
// Designer-specific MCAPI types and utility functions.
//-----------------------------------------------------------------------------

#include "MCAPIDesignerTypes.h"

//-----------------------------------------------------------------------------
// Function: stringToValue()
//-----------------------------------------------------------------------------
bool stringToValue(QString const& str, MCAPIEndpointDirection& dir)
{
    if (str == "in")
    {
        dir = MCAPI_ENDPOINT_IN;
        return true;
    }
    else if (str == "out")
    {
        dir = MCAPI_ENDPOINT_OUT;
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: stringToValue()
//-----------------------------------------------------------------------------
bool stringToValue(QString const& str, MCAPIDataType& type)
{
    if (str == "message")
    {
        type = MCAPI_TYPE_MESSAGE;
        return true;
    }
    else if (str == "packet")
    {
        type = MCAPI_TYPE_PACKET;
        return true;
    }
    else if (str == "scalar")
    {
        type = MCAPI_TYPE_SCALAR;
        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: valueToString()
//-----------------------------------------------------------------------------
QString valueToString(MCAPIEndpointDirection dir)
{
    switch (dir)
    {
    case MCAPI_ENDPOINT_IN:
        {
            return "in";
        }

    case MCAPI_ENDPOINT_OUT:
        {
            return "out";
        }

    default:
        {
            Q_ASSERT(false);
            return "";
        }
    }
}

//-----------------------------------------------------------------------------
// Function: valueToString()
//-----------------------------------------------------------------------------
QString valueToString(MCAPIDataType type)
{
    switch (type)
    {
    case MCAPI_TYPE_MESSAGE:
        {
            return "message";
        }

    case MCAPI_TYPE_PACKET:
        {
            return "packet";
        }

    case MCAPI_TYPE_SCALAR:
        {
            return "scalar";
        }

    default:
        {
            Q_ASSERT(false);
            return "";
        }
    }
}


