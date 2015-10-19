//-----------------------------------------------------------------------------
// File: CpuColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.10.2015
//
// Description:
// Common declarations for editing cpus.
//-----------------------------------------------------------------------------

namespace CpuColumns
{
    //! Contains the columns for cpu properties.
    enum Column
    {
        NAME_COLUMN = 0,
        DISPLAY_NAME_COLUMN,
        ADDRSPACE_COLUMN,
        DESCRIPTION_COLUMN,
        COLUMN_COUNT
    };

    //! The roles that can operate on QStringList on address space column.
    enum Role
    {
        USER_DISPLAY_ROLE = Qt::UserRole,
        USER_EDIT_ROLE
    };
}
