//-----------------------------------------------------------------------------
// File: PortColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.10.2015
//
// Description:
// Common declarations for editing COM interfaces.
//-----------------------------------------------------------------------------

namespace ComInterfaceColumns
{
    //! The user roles that can be used.
    enum UserRole
    {
        TRANSFER_TYPE_OPTIONS = Qt::UserRole //<! Returns QStringList that contains the possible transfer types.
    };

    //! The column numbers for columns.
    enum Column
    {
        NAME = 0,
        COM_DEFINITION,
        TRANSFER_TYPE,
        DIRECTION,
        DESCRIPTION,
        COLUMN_COUNT
    };
}
