//-----------------------------------------------------------------------------
// File: FileDependencyColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.10.2015
//
// Description:
// Common declarations for editing file dependencies.
//-----------------------------------------------------------------------------

#ifndef FILE_DEPENDENCY_COLUMNS_H
#define FILE_DEPENDENCY_COLUMNS_H

namespace FileDependencyColumns
{
    //! The editable columns in the file dependency editor.
    enum columns
    {
        TREE_EXPAND = 0,
        STATUS,
        PATH,
        FILESETS,
        CREATE_DEPENDENCY,
        DEPENDENCIES,
        COLUMN_COUNT
    };
}

#endif // FILE_DEPENDENCY_COLUMNS_H