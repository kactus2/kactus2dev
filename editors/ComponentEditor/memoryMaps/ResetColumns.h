//-----------------------------------------------------------------------------
// File: ResetColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.01.2019
//
// Description:
// Common declarations for editing resets of register fields.
//-----------------------------------------------------------------------------

#ifndef RESETCOLUMNS_H
#define RESETCOLUMNS_H

namespace ResetColumns
{
    //! Defines the columns for the field resets.
    enum columns
    {
        RESETTYPEREFERENCE_COLUMN = 0,
        RESETVALUE_COLUMN,
        RESETMASK_COLUMN,
        COLUMN_COUNT
    };
}

#endif // RESETCOLUMNS_H