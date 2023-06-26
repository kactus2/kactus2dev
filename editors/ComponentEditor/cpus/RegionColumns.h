//-----------------------------------------------------------------------------
// File: SegmentColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.06.2023
//
// Description:
// Common declarations for regions table columns.
//-----------------------------------------------------------------------------

#ifndef REGIONCOLUMNS_H
#define REGIONCOLUMNS_H

namespace RegionColumns
{
    //! Defines the columns for the regions table.
    enum Column
    {
        NAME = 0,
        DISPLAY_NAME,
        SHORT_DESCRIPTION,
        OFFSET,
        RANGE,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif // SEGMENTCOLUMNS_H