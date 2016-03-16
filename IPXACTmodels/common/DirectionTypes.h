//-----------------------------------------------------------------------------
// File: DirectionTypes.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.09.2015
//
// Description:
// Namespace DirectionTypes is used to store functions related to port directions.
//-----------------------------------------------------------------------------

#ifndef DIRECTIONTYPES_H
#define DIRECTIONTYPES_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Namespace DirectionTypes is used to store functions related to port directions.
//-----------------------------------------------------------------------------
namespace DirectionTypes
{
    //! Specifies the direction of the port
    enum Direction {
        IN,
        OUT,
        INOUT,
        DIRECTION_PHANTOM,
        DIRECTION_INVALID
    };

    /*!
     *  Convert the given direction to mirrored.
     *
     *      @param [in] originalDirection   The direction to convert.
     *
     *      @return The mirrored direction.
     */
    IPXACTMODELS_EXPORT DirectionTypes::Direction convert2Mirrored(
        const DirectionTypes::Direction originalDirection);

    /*!
     *  Convert a given qstring to direction.
     *
     *      @param [in] str             The string to be converted.
     *      @param [in] defaultValue    The value that will be returned if no match is found.
     *
     *      @return Direction that matches the string or default value.
     */
    IPXACTMODELS_EXPORT DirectionTypes::Direction str2Direction(
        QString str, DirectionTypes::Direction defaultValue);

    /*!
     *  Convert a given direction to a qstring.
     *
     *      @param [in] direction   Direction value to be converted to QString.
     *
     *      @return QString matching the given direction.
     */
    IPXACTMODELS_EXPORT QString direction2Str(const DirectionTypes::Direction direction);

}

#endif // DIRECTIONTYPES_H
