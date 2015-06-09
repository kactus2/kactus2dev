//-----------------------------------------------------------------------------
// File: MemoryMapColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.02.2015
//
// Description:
// Common declarations for editing memory map table columns.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAP_COLUMNS_H
#define MEMORYMAP_COLUMNS_H

namespace MemoryMapColumns
{
    //! Defines the columns for the memory map table.
    enum Column
    {
        NAME_COLUMN = 0,
        BASE_COLUMN,
        RANGE_COLUMN,
        WIDTH_COLUMN,
        USAGE_COLUMN,
        ACCESS_COLUMN,
        VOLATILE_COLUMN,
        DESCRIPTION_COLUMN,
        COLUMN_COUNT
    };
}

#endif //MEMORYMAP_COLUMNS_H