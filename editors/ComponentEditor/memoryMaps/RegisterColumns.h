//-----------------------------------------------------------------------------
// File: RegisterColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.02.2015
//
// Description:
// Common declarations for editing register table columns.
//-----------------------------------------------------------------------------

#ifndef REGISTER_COLUMNS_H
#define REGISTER_COLUMNS_H

namespace RegisterColumns
{
    //! Defines the columns for the register table.
    enum Column
    {
        NAME_COLUMN = 0,
        OFFSET_COLUMN,
        WIDTH_COLUMN,
        DESC_COLUMN,
        VOLATILE_COLUMN,
        ACCESS_COLUMN,
        MOD_WRITE_COLUMN,
        READ_ACTION_COLUMN,
        TESTABLE_COLUMN,
        TEST_CONSTR_COLUMN,
        IS_PRESENT_COLUMN,
        COLUMN_COUNT
    };
}

#endif //REGISTER_COLUMNS_H