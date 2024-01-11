//-----------------------------------------------------------------------------
// File: PortSummaryColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 27.11.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef PORT_SUMMARY_COLUMNS_H
#define PORT_SUMMARY_COLUMNS_H

namespace PortSummaryColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum Columns
    {
        ROW_NUMBER = 0,         //!< Column for the row number.
        NAME = 1,               //!< Column for the port name.
        TYPE = 2,               //!< Column for the port type: wire/transactional/structured.
        TAG_GROUP = 3,          //!< Column for tagging a port.
        ADHOC_VISIBILITY = 4,   //!< Column for toggling ad-hoc visibility on/off.
        DESCRIPTION = 5,        //!< Column for adding a description for the port.
        ARRAY_LEFT = -1,        //!< Column for setting the left side of the array. Disabled.
        ARRAY_RIGHT = -1,       //!< Column for setting the right side of the array. Disabled.
        TYPE_NAME = -1,         //!< Column for the port typename. Disabled.
    };

    constexpr static auto COLUMN_COUNT = 6;
}

#endif // PORT_SUMMARY_COLUMNS_H