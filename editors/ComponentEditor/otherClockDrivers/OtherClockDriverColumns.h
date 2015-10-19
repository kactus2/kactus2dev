//-----------------------------------------------------------------------------
// File: OtherClockDriverColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.10.2015
//
// Description:
// Common declarations for editing otherClockDrivers.
//-----------------------------------------------------------------------------

namespace OtherClockDriverColumns
{
    //! The columns for otherClockDriver table.
    enum Columns
    {
        NAME = 0,
        CLOCK_SOURCE,
        CLOCK_PERIOD,
        CLOCK_PERIOD_UNIT,
        PULSE_OFFSET,
        PULSE_OFFSET_UNIT,
        PULSE_VALUE,
        PULSE_DURATION,
        PULSE_DURATION_UNIT,
        COLUMN_COUNT
    };
}
