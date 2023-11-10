//-----------------------------------------------------------------------------
// File: ChoiceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.10.2023
//
// Description:
// Common declarations for editing power domain links in component instance.
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINLINKCOLUMNS_H
#define POWERDOMAINLINKCOLUMNS_H

namespace PowerDomainLinkColumns
{
    //! The editable columns in the power domain links editor.
    enum Column
    {
        EXTERNAL_LINK = 0,
        INTERNAL_LINK,
        COLUMN_COUNT
    };

    enum Roles
    {
        USER_EDIT_ROLE = Qt::UserRole + 1,
        USER_DISPLAY_ROLE
    };
}

#endif // POWERDOMAINLINKCOLUMNS_H