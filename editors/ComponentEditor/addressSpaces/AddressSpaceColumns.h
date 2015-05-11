//-----------------------------------------------------------------------------
// File: AddressSpaceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.05.2015
//
// Description:
// Common declarations for editing memory map table columns.
//-----------------------------------------------------------------------------

#ifndef ADDRESS_SPACE_COLUMNS_H
#define ADDRESS_SPACE_COLUMNS_H

namespace AddressSpaceColumns
{
    //! Defines the columns for the address space table.
    enum Column
    {
        NAME = 0,		
        AUB,	
        RANGE,
        WIDTH,
        INTERFACE_BINDING,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif //ADDRESS_SPACE_COLUMNS_H