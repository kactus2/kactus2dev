//-----------------------------------------------------------------------------
// File: MemoryMapsColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.04.2015
//
// Description:
// Common declarations for columns in memory maps editor.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPS_COLUMNS_H
#define MEMORYMAPS_COLUMNS_H

namespace MemoryMapsColumns
{
    //! Defines the columns for the memory map table.
    enum Column
    {
        NAME_COLUMN = 0,
        AUB_COLUMN,
        INTERFACE_COLUMN,
        REMAPSTATE_COLUMN,
        DESCRIPTION_COLUMN,
        COLUMN_COUNT
    };
}

#endif // MEMORYMAPS_COLUMNS_H