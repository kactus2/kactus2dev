//-----------------------------------------------------------------------------
// File: EnvIdentifiersColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.10.2015
//
// Description:
// Common declarations for editing environment identifiers columns.
//-----------------------------------------------------------------------------

#ifndef ENVIDENTIFIERSCOLUMNS_H
#define ENVIDENTIFIERSCOLUMNS_H

namespace EnvIdentifiersColumns
{
    //! Defines the columns for the environment identifiers table.
    enum Column
    {
        LANGUAGE = 0,
        TOOL,
        VENDOR_SPECIFIC,
        COLUMN_COUNT
    };
}

#endif //ENVIDENTIFIERSCOLUMNS_H