//-----------------------------------------------------------------------------
// File: PortMapsColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
        LOGICAL_PRESENCE = 0,
        LOGICAL_LEFT,
        LOGICAL_RIGHT,
        LOGICAL_PORT,
        PHYSICAL_PORT,
        PHYSICAL_LEFT,
        PHYSICAL_RIGHT,
        INVERT,
        ISINFORMATIVE,
        TIEOFF,
        COLUMN_COUNT
    };
}
