//-----------------------------------------------------------------------------
// File: FileSetColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.10.2015
//
// Description:
// Common declarations for editing file sets.
//-----------------------------------------------------------------------------

#ifndef FILESETCOLUMNS_H
#define FILESETCOLUMNS_H

namespace FileSetColumns
{
    //! The editable columns in the file sets editor.
    enum columns
    {
        NAME_COLUMN = 0,
        GROUP_COLUMN,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif // FILESETCOLUMNS_H