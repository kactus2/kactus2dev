//-----------------------------------------------------------------------------
// File: FileTypeColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 13.03.2017
//
// Description:
// Common declarations for editing file type table columns.
//-----------------------------------------------------------------------------

#ifndef FILE_TYPE_COLUMNS_H
#define FILE_TYPE_COLUMNS_H

namespace FileTypeColumns
{
    //! Defines the columns for the file types table.
    enum Columns
    {
        NAME = 0,        
        EXTENSIONS,
        EXECUTABLE,
        EDITABLE_IN_KACTUS,
        COLUMN_COUNT
    };
}

#endif //FILE_TYPE_COLUMNS_H