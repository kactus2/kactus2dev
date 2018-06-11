//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.06.2018
//
// Description:
// Common definitions for abstraction definition transactional port table.
//-----------------------------------------------------------------------------

namespace AbstractionTransactionalPortColumns
{
    //! Enumeration for columns in abstraction definition transactional port table.
    enum Columns
    {
        NAME = 0,
        MODE,
        PRESENCE,
        INITIATIVE,
        KIND,
        BUSWIDTH,
        SYSTEM_GROUP,
        QUALIFIER,
        DESCRIPTION,
        COLUMN_COUNT
    };
}