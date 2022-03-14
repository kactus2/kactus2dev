//-----------------------------------------------------------------------------
// File: SubspaceMapColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.02.2022
//
// Description:
// Common declarations for subspace map columns.
//-----------------------------------------------------------------------------

#ifndef SUBSPACEMAP_COLUMNS_H
#define SUBSPACEMAP_COLUMNS_H

namespace SubspaceMapColumns
{
    //! Defines the columns for the subspace maps table.
    enum Column
    {
        NAME = 0,
        BASE,
        MASTERREFERENCE,
        SEGMENTREFERENCE,
        IS_PRESENT,
        DESCRIPTION_COLUMN,
        COLUMN_COUNT
    };
}

#endif //SUBSPACEMAP_COLUMNS_H