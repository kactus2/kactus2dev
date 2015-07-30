//-----------------------------------------------------------------------------
// File: PortColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.01.2015
//
// Description:
// Common declarations for editing ports.
//-----------------------------------------------------------------------------

namespace PortColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum Columns
    {
        ROW_NUMBER = 0,     //!< Column for the row number.
        NAME ,              //!< Column for the port name.
        DIRECTION,          //!< Column for the port direction.
        LEFT_BOUND,         //!< Column for specifying the left bound of the port.
        RIGHT_BOUND,        //!< Column for specifying the right bound of the port.
        WIDTH,              //!< Column for the port width.
        TYPE_NAME,          //!< Column for the port typename.
        TYPE_DEF,           //!< Column for the port type definition.
        DEFAULT_VALUE,      //!< Column for setting the default value for the port.
        ARRAY_LEFT,         //!< Column for setting the left side of the array.
        ARRAY_RIGHT,        //!< Column for setting the right side of the array.
        TAG_GROUP,          //!< Column for tagging a port.
        ADHOC_VISIBILITY,   //!< Column for toggling ad-hoc visibility on/off.
        DESCRIPTION,      //!< Column for adding a description for the port.
        COLUMN_COUNT
    };
}
