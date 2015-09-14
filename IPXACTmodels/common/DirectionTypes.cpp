//-----------------------------------------------------------------------------
// File: DirectionTypes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.09.2015
//
// Description:
// Namespace DirectionTypes is used to store functions related to port directions.
//-----------------------------------------------------------------------------

#include "DirectionTypes.h"

//-----------------------------------------------------------------------------
// Function: DirectionTypes::convert2Mirrored()
//-----------------------------------------------------------------------------
DirectionTypes::Direction DirectionTypes::convert2Mirrored(const Direction originalDirection )
{
    if (originalDirection == DirectionTypes::IN)
    {
        return DirectionTypes::OUT;
    }
    else if (originalDirection == DirectionTypes::OUT)
    {
        return DirectionTypes::IN;
    }
    else if (originalDirection == DirectionTypes::INOUT)
    {
        return DirectionTypes::INOUT;
    }
    else if (originalDirection == DirectionTypes::DIRECTION_PHANTOM)
    {
        return DirectionTypes::DIRECTION_PHANTOM;
    }
    else
    {
        return DirectionTypes::DIRECTION_INVALID;
    }
}

//-----------------------------------------------------------------------------
// Function: DirectionTypes::str2Direction()
//-----------------------------------------------------------------------------
DirectionTypes::Direction DirectionTypes::str2Direction(QString str, DirectionTypes::Direction defaultValue)
{
	if (str == QString("in"))
    {
		return DirectionTypes::IN;
	}
	else if (str == QString("out"))
    {
		return DirectionTypes::OUT;
	}
	else if (str == QString("inout"))
    {
		return DirectionTypes::INOUT;
	}
	else if (str == QString("phantom"))
    {
		return DirectionTypes::DIRECTION_PHANTOM;
	}
	else
    {
        return defaultValue;
	}
}

//-----------------------------------------------------------------------------
// Function: DirectionTypes::direction2Str()
//-----------------------------------------------------------------------------
QString DirectionTypes::direction2Str(const DirectionTypes::Direction direction)
{
    if (direction == DirectionTypes::IN)
    {
        return QString("in");
    }
    else if (direction == DirectionTypes::OUT)
    {
        return QString("out");
    }
    else if (direction == DirectionTypes::INOUT)
    {
        return QString("inout");
    }
    else if (direction == DirectionTypes::DIRECTION_PHANTOM)
    {
        return QString("phantom");
    }
    else
    {
        return QString();
    }
}
