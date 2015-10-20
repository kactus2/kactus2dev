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

namespace FileDependencyColumns
{
    //! The editable columns in the file dependency editor.
    enum columns
    {
        FILE_DEPENDENCY_COLUMN_TREE = 0,
        FILE_DEPENDENCY_COLUMN_STATUS,
        FILE_DEPENDENCY_COLUMN_PATH,
        FILE_DEPENDENCY_COLUMN_FILESETS,
        FILE_DEPENDENCY_COLUMN_CREATE,
        FILE_DEPENDENCY_COLUMN_DEPENDENCIES,
        FILE_DEPENDENCY_COLUMN_COUNT
    };
}
