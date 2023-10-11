//-----------------------------------------------------------------------------
// File: OtherClockDriverColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 16.10.2015
//
// Description:
// Common declarations for editing otherClockDrivers.
//-----------------------------------------------------------------------------

namespace PowerDomainColumns
{
    //! The columns for otherClockDriver table.
    enum Columns
    {
        NAME = 0,
        DISPLAY_NAME,
        ALWAYS_ON,
        SUBDOMAIN,
        SHORT_DESCRIPTION,
        DESCRIPTION,
        COLUMN_COUNT
    };
}
