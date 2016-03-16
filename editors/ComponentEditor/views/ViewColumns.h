//-----------------------------------------------------------------------------
// File: ViewColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.10.2015
//
// Description:
// Common declarations for columns editing views.
//-----------------------------------------------------------------------------

#ifndef VIEWCOLUMNS_H
#define VIEWCOLUMNS_H

namespace ViewColumns
{
    //! Defines the columns for the views table.
    enum Column
    {
        NAME_COLUMN = 0,
        TYPE_COLUMN,
        DESCRIPTION_COLUMN,
        COLUMN_COUNT
    };
}

#endif //VIEWCOLUMNS_H