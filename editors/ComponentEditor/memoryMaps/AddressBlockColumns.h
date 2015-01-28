//-----------------------------------------------------------------------------
// File: AddressBlockColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.01.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef ADDRESS_BLOCK_COLUMNS_H
#define ADDRESS_BLOCK_COLUMNS_H

namespace AddressBlockColumns
{
    //! \brief Defines the columns for the address block table.
    enum Column {
        NAME_COLUMN = 0,
        OFFSET_COLUMN,
        SIZE_COLUMN,
        DIM_COLUMN,
        DESC_COLUMN,
        VOLATILE_COLUMN,
        ACCESS_COLUMN,
        RESET_VALUE_COLUMN,
        RESET_MASK_COLUMN,
        COLUMN_COUNT
    };
}

#endif //ADDRESS_BLOCK_COLUMNS_H