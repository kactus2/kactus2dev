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

//-----------------------------------------------------------------------------
//! ColumnContentType enumeration.
//-----------------------------------------------------------------------------
enum ColumnContentType
{
    COLUMN_CONTENT_IO = 0,
    COLUMN_CONTENT_BUSES,
    COLUMN_CONTENT_COMPONENTS,
    COLUMN_CONTENT_MEMORY,
    COLUMN_CONTENT_CUSTOM
};

//-----------------------------------------------------------------------------
//! ColumnItemType enumeration.
//-----------------------------------------------------------------------------
enum ColumnItemType
{
    CIT_NONE = 0,
    CIT_INTERFACE = 0x01,
    CIT_COMPONENT = 0x02,
    CIT_CHANNEL = 0x04,
    CIT_BRIDGE = 0x08,

    CIT_COUNT = 4
};

//-----------------------------------------------------------------------------

#endif // COLUMNTYPES_H