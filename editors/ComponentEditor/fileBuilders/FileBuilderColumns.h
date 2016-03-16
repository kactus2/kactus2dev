//-----------------------------------------------------------------------------
// File: FileBuilderColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.10.2015
//
// Description:
// Common declarations for editing file builders.
//-----------------------------------------------------------------------------

namespace FileBuilderColumns
{
    //! The editable columns in the file builder editor.
    enum columns
    {
        FILETYPE_COLUMN = 0,
        COMMAND_COLUMN,
        FLAGS_COLUMN,
        REPLACE_DEFAULT_COLUMN,
        COLUMN_COUNT
    };
}
