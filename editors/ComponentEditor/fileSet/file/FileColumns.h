//-----------------------------------------------------------------------------
// File: FileColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.10.2015
//
// Description:
// Common declarations for editing files.
//-----------------------------------------------------------------------------

#ifndef FILECOLUMNS_H
#define FILECOLUMNS_H

namespace FileColumns
{
    //! The editable columns in the files editor.
    enum columns
    {
        NAME_COLUMN = 0,
        PATH_COLUMN,
        TYPES_COLUMN,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif // FILECOLUMNS_H