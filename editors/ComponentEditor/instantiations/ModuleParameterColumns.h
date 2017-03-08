//-----------------------------------------------------------------------------
// File: ModuleParameterColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 06.11.2014
//
// Description:
// Common declarations for editing module parameters.
//-----------------------------------------------------------------------------

namespace ModuleParameterColumns
{
    //! The editable columns in the module parameter editor.
    enum columns
    {
        NAME = 0,
        DISPLAY_NAME,
        DESCRIPTION,
        ID,
        DATA_TYPE,
        TYPE,
        VALUE,
        CHOICE,
        MINIMUM,
        MAXIMUM,
        USAGE_TYPE,
        RESOLVE,
        BITWIDTH_LEFT,
        BITWIDTH_RIGHT,
        ARRAY_LEFT,
        ARRAY_RIGHT,
        USAGE_COUNT,
        COLUMN_COUNT,
    };
}
