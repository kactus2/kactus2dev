//-----------------------------------------------------------------------------
// File: InterfacePortMapColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.05.2016
//
// Description:
// Common declarations for interface port map.
//-----------------------------------------------------------------------------

#ifndef INTERFACEPORTMAPCOLUMNS_H
#define INTERFACEPORTMAPCOLUMNS_H

namespace InterfacePortMapColumns
{
    //! Defines the columns for the interface port map table.
    enum Columns
    {
        INTERFACE_LOGICAL_NAME = 0,     //!< Column for the logical port name.
        INTERFACE_PHYSICAL_NAME,        //!< Column for the physical port name.
        INTERFACE_EXCLUDE,              //!< Column for toggling port exclusion.
        INTERFACE_COLUMN_COUNT
    };
}

#endif //INTERFACEPORTMAPCOLUMNS_H