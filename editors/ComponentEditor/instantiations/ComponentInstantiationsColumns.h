//-----------------------------------------------------------------------------
// File: ComponentInstantiationsColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Common declarations for columns editing component instantiations.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATIONSCOLUMNS_H
#define COMPONENTINSTANTIATIONSCOLUMNS_H

namespace ComponentInstantiationsColumns
{
    //! Defines the columns for the component instantiations table.
    enum Column
    {
        NAME = 0,
        DISPLAY_NAME,
        LANGUAGE,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif // COMPONENTINSTANTIATIONSCOLUMNS_H