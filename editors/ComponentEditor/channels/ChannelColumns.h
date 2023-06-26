//-----------------------------------------------------------------------------
// File: ChannelColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.10.2015
//
// Description:
// Common declarations for editing channels.
//-----------------------------------------------------------------------------

namespace ChannelColumns
{
    //! The column numbers.
    enum Column
    {
        NAME = 0,
        DISPLAY_NAME,
        INTERFACES,
        SHORT_DESCRIPTION,
        DESCRIPTION,
        COLUMN_COUNT
    };

    //! The roles to display/edit interface column with QStringList.
    enum Roles
    {
        USER_DISPLAY_ROLE = Qt::UserRole,
        USER_EDIT_ROLE
    };
}
