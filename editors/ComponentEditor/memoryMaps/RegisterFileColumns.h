//-----------------------------------------------------------------------------
// File: RegisterFileColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 12.10.2018
//
// Description:
// Common declarations for editing register files table columns.
//-----------------------------------------------------------------------------

#ifndef REGISTER_FILE_COLUMNS_H
#define REGISTER_FILE_COLUMNS_H

namespace RegisterFileColumns
{
    //! Defines the columns for the register files table.
    enum Column
    {
        NAME = 0,
        ADDRESS_OFFSET,
        RANGE,
        DIMENSION,
        IS_PRESENT,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif //REGISTER_FILE_COLUMNS_H
