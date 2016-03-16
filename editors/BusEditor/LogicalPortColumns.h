//-----------------------------------------------------------------------------
// File: LogicalPortColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.08.2015
//
// Description:
// Common definitions for abstraction definition logical port table.
//-----------------------------------------------------------------------------

namespace LogicalPortColumns
{
    //! Enumeration for columns in abstraction definition logical port table.
    enum Columns
    {
        NAME = 0,
        MODE,
        PRESENCE,
        DIRECTION,
        WIDTH,
        DEFAULT_VALUE,
        DRIVER,
        QUALIFIER,
        SYSTEM_GROUP,
        DESCRIPTION,
        COLUMN_COUNT
    };
}