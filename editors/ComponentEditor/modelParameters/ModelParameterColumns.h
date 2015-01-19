//-----------------------------------------------------------------------------
// File: ModelParameterColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 06.11.2014
//
// Description:
// Common declarations for editing model parameters.
//-----------------------------------------------------------------------------

namespace ModelParameterColumns
{
    //! The editable columns in the model parameter editor.
    enum columns
    {
        NAME = 0,
        DISPLAY_NAME,
        DESCRIPTION,
        VALUEID,
        DATA_TYPE,
        TYPE,
        VALUE,
        CHOICE,
        MINIMUM,
        MAXIMUM,
        USAGE_TYPE,
        RESOLVE,
        BITWIDTH,
        ARRAY_SIZE,
        ARRAY_OFFSET,
        USAGE_COUNT,
        COLUMN_COUNT,
    };
}
