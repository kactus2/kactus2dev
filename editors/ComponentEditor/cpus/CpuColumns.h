//-----------------------------------------------------------------------------
// File: CpuColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
        NAME = 0,
        DISPLAY_NAME,
        SHORT_DESCRIPTION,
        ADDRSPACE,
        MEMORY_MAP,
        RANGE,
        WIDTH,
        AUB,
        DESCRIPTION,
        COLUMN_COUNT
    };

    //! The roles that can operate on QStringList on address space column.
    enum Role
    {
        USER_DISPLAY_ROLE = Qt::UserRole,
        USER_EDIT_ROLE
    };
}
