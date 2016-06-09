//-----------------------------------------------------------------------------
// File: PortMapsColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// Common declarations for port maps.
//-----------------------------------------------------------------------------

namespace PortMapsColumns
{
    //! The editable columns in the port maps editor.
    enum columns
    {
        LOGICAL_PORT = 0,
        LOGICAL_LEFT,
        LOGICAL_RIGHT,
        LOGICAL_PRESENCE,
        PHYSICAL_PORT,
        PHYSICAL_LEFT,
        PHYSICAL_RIGHT,
        INVERT,
        TIEOFF,
        COLUMN_COUNT
    };
}
