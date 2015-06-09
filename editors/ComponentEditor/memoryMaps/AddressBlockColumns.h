//-----------------------------------------------------------------------------
// File: AddressBlockColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.01.2015
//
// Description:
// Common declarations for editing address block table columns.
//-----------------------------------------------------------------------------

#ifndef ADDRESS_BLOCK_COLUMNS_H
#define ADDRESS_BLOCK_COLUMNS_H

namespace AddressBlockColumns
{
    //! Defines the columns for the address block table.
    enum Column {
        NAME = 0,
        REGISTER_OFFSET,
        REGISTER_SIZE,
        REGISTER_DIMENSION,        
        VOLATILE,
        REGISTER_ACCESS,
        RESET_VALUE,
        RESET_MASK,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif //ADDRESS_BLOCK_COLUMNS_H