//-----------------------------------------------------------------------------
// File: RemapConditionColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.04.2015
//
// Description:
// Common declarations for editing remap conditions
//-----------------------------------------------------------------------------

namespace RemapConditionColumns
{
    //! The editable columns in the remap condition editor.
    enum columns
    {
        NAME_COLUMN = 0,    //!< Column for the name of the remap condition.
        LEFT_COLUMN,        //!< Column for the left bound of the remap port.
        RIGHT_COLUMN,       //!< Column for the right bound of the remap port.
        VALUE_COLUMN,       //!< Column for the value of the remap port.
        COLUMN_COUNT
    };
}
