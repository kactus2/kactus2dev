//-----------------------------------------------------------------------------
// File: LogicalPortColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.08.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

namespace LogicalPortColumns
{
    //! Enumeration for columns in abstraction definition logical port table.
    enum Columns
    {
        NAME = 0,
        QUALIFIER,
        WIDTH,
        DIRECTION,
        MODE,
        GROUP,
        PRESENCE,
        DEFAULT_VALUE,
        DRIVER,
        DESCRIPTION,
        COLUMN_COUNT
    };
}