//-----------------------------------------------------------------------------
// File: ApiInterfaceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.10.2015
//
// Description:
// Common declarations for editing API interfaces.
//-----------------------------------------------------------------------------

#ifndef API_INTERFACE_COLUMNS_H
#define API_INTERFACE_COLUMNS_H

namespace ApiInterfaceColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum Columns
    {
        NAME = 0,
        API_DEFINITION,
        DEPENDENCY,
        DISPLAY_NAME,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif // API_INTERFACE_COLUMNS_H