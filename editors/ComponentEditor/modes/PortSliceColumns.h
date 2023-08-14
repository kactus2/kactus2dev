//-----------------------------------------------------------------------------
// File: PortSliceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 09.08.2023
//
// Description:
// Common declarations for editing PortSlices in mode.
//-----------------------------------------------------------------------------

namespace PortSliceColumns
{
    //! Contains the columns for PortSlice properties.
    enum Column
    {
        NAME = 0,
        PORT_REF,
        SUB_PORT,
        LEFT_BOUND,
        RIGHT_BOUND,
        DESCRIPTION,
        COLUMN_COUNT
    };

}
