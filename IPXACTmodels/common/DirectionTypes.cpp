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
	if (str == QLatin1String("in"))
    {
		return DirectionTypes::IN;
	}
	else if (str == QLatin1String("out"))
    {
		return DirectionTypes::OUT;
	}
	else if (str == QLatin1String("inout"))
    {
		return DirectionTypes::INOUT;
	}
	else if (str == QLatin1String("phantom"))
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
        return QStringLiteral("in");
    }
    else if (direction == DirectionTypes::OUT)
    {
        return QStringLiteral("out");
    }
    else if (direction == DirectionTypes::INOUT)
    {
        return QStringLiteral("inout");
    }
    else if (direction == DirectionTypes::DIRECTION_PHANTOM)
    {
        return QStringLiteral("phantom");
    }
    else
    {
        return QString();
    }
}
