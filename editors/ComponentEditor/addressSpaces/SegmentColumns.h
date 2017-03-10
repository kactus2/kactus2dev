//-----------------------------------------------------------------------------
// File: SegmentColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.10.2015
//
// Description:
// Common declarations for segment table columns.
//-----------------------------------------------------------------------------

#ifndef SEGMENTCOLUMNS_H
#define SEGMENTCOLUMNS_H

namespace SegmentColumns
{
    //! Defines the columns for the address space table.
    enum Column
    {
        NAME = 0,
        OFFSET,
        RANGE,
        IS_PRESENT,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif // SEGMENTCOLUMNS_H