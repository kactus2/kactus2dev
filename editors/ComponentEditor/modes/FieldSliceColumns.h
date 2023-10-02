//-----------------------------------------------------------------------------
// File: FieldSliceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 18.08.2023
//
// Description:
// Common declarations for editing FieldSlices in mode.
//-----------------------------------------------------------------------------

namespace FieldSliceColumns
{
    //! Contains the columns for FieldSlice properties.
    enum Column
    {
        NAME = 0,
        FIELD_REF,
        RANGE_LEFT,
        RANGE_RIGHT,
        DESCRIPTION,
        COLUMN_COUNT
    };

}
