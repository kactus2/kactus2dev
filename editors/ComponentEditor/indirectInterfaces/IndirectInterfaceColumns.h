//-----------------------------------------------------------------------------
// File: IndirectInterfaceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Common declarations for editing indirect interfaces table columns.
//-----------------------------------------------------------------------------

#ifndef INDIRECT_INTERFACE_COLUMNS_H
#define INDIRECT_INTERFACE_COLUMNS_H

namespace IndirectInterfaceColumns
{
    //! Defines the columns for the indirect interfaces table.
    enum Column {
        NAME = 0,
        DISPLAY_NAME,
        INDIRECT_ADDRESS_REF,
        INDIRECT_DATA_REF,
        MEMORY_MAP_REF,
        TRANSPARENT_BRIDGES,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif //INDIRECT_INTERFACE_COLUMNS_H