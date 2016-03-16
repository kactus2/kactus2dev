//-----------------------------------------------------------------------------
// File: ColumnTypes.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.4.2011
//
// Description:
// Common enumerations and other types for the column layout.
//-----------------------------------------------------------------------------

#ifndef COLUMNTYPES_H
#define COLUMNTYPES_H

namespace ColumnTypes
{
    //-----------------------------------------------------------------------------
    //! ColumnContentType enumeration.
    //-----------------------------------------------------------------------------
    enum ColumnContentType
    {
        IO = 0,
        BUSES,
        COMPONENTS,
        MEMORY,
        CUSTOM
    };

    //-----------------------------------------------------------------------------
    //! ColumnItemType enumeration.
    //-----------------------------------------------------------------------------
    enum ColumnItemType
    {
        NONE = 0,
        INTERFACE = 0x01,
        COMPONENT = 0x02,
        CHANNEL = 0x04,
        BRIDGE = 0x08,

        TYPE_COUNT = 4
    };
}

#endif // COLUMNTYPES_H