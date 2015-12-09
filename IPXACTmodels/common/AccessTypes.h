//-----------------------------------------------------------------------------
// File: AccessTypes.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.12.2015
//
// Description:
// Namespace AccessTypes is used to store functions related to memory access.
//-----------------------------------------------------------------------------

#ifndef ACCESSTYPES_H
#define ACCESSTYPES_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Namespace AccessTypes is used to store functions related to memory access.
//-----------------------------------------------------------------------------
namespace AccessTypes
{
    /*!
     *  Equals ipxact:access element within ipxact:memoryBlockData
     *
     * Access specifies the accessibility of the data in the address block.
     */
    enum Access {
        READ_WRITE = 0,
        READ_ONLY,
        WRITE_ONLY,
        READ_WRITEONCE,
        WRITEONCE,
        ACCESS_COUNT
    };

    /*!
     *  Convert the value of the QString into enum Access
     *
     *  If The str does not match any of the enum values the default value specified as parameter will be returned.
     *
     *      @param [in] str             QString containing the string to be converted.
     *      @param [in] defaultValue    The value that will be returned if no match is found.
     *
     *      @return Access that matches the str or default value.
     */
    IPXACTMODELS_EXPORT AccessTypes::Access str2Access(QString str, AccessTypes::Access defaultValue);

    /*!
     *  Convert the Access value to QString.
     *
     *      @param [in] access  The Access value to be converted into QString.
     *
     *      @return QString that matches the enum value.
     */
    IPXACTMODELS_EXPORT QString access2Str(const AccessTypes::Access access);
}

#endif // ACCESSTYPES_H
