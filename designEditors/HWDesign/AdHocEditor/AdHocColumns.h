//-----------------------------------------------------------------------------
// File: AdHocColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.10.2015
//
// Description:
// Common declarations for ad hoc columns.
//-----------------------------------------------------------------------------

#ifndef ADHOCCOLUMNS_H
#define ADHOCCOLUMNS_H

namespace AdHocColumns
{
    //! Defines the columns for the ad hoc table.
    enum Columns
    {
        ADHOC_COL_NAME = 0,          //!< Column for the port name.
        ADHOC_COL_DIRECTION,         //!< Column for the port direction.
        ADHOC_COL_VISIBILITY,        //!< Column for toggling ad-hoc visibility on/off.
        ADHOC_COL_COUNT
    };
}

#endif //ADHOCCOLUMNS_H