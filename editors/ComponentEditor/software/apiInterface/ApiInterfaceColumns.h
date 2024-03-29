//-----------------------------------------------------------------------------
// File: ApiInterfaceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
        DISPLAY_NAME,
        API_DEFINITION,
        DEPENDENCY,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif // API_INTERFACE_COLUMNS_H