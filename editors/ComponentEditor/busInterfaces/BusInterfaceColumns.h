//-----------------------------------------------------------------------------
// File: BusInterfaceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.07.2015
//
// Description:
// Common declarations for editing bus interfaces table columns.
//-----------------------------------------------------------------------------

#ifndef BUS_INTERFACE_COLUMNS_H
#define BUS_INTERFACE_COLUMNS_H

namespace BusInterfaceColumns
{
    //! Defines the columns for the bus interfaces table.
    enum Column {
        NAME = 0,
        BUSDEF,
        ABSDEF,
        INTERFACE_MODE,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif //BUS_INTERFACE_COLUMNS_H