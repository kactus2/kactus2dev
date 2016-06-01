//-----------------------------------------------------------------------------
// File: PortMappingColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// Common declarations for physical ports in port maps editor.
//-----------------------------------------------------------------------------

namespace PortMappingColumns
{
    //! The editable columns in the physical ports of the port maps editor.
    enum columns
    {
        NAME = 0,
        DIRECTION,
        LEFT_BOUND,
        RIGHT_BOUND,
        SIZE,
        COLUMN_COUNT
    };
}
