//-----------------------------------------------------------------------------
// File: TransactionalPortColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Common declarations for editing transactional ports.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONAL_PORT_COLUMNS_H
#define TRANSACTIONAL_PORT_COLUMNS_H

namespace TransactionalPortColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum Columns
    {
        ROW_NUMBER = 0,     //!< Column for the row number.
        NAME ,              //!< Column for the port name.
        WIDTH,              //!< Column for the port width.
        INITIATIVE,         //!< Column for the transactional initiative.
        KIND,               //!< Column for the transactional kind.
        PROTOCOL,           //!< Column for the transactional protocol.
        TYPE_NAME,          //!< Column for the port typename.
        ARRAY_LEFT,         //!< Column for setting the left side of the array.
        ARRAY_RIGHT,        //!< Column for setting the right side of the array.
        TAG_GROUP,          //!< Column for tagging a port.
        MAX_CONNECTIONS,    //!< Column for setting the maximum number of connections.
        MIN_CONNECTIONS,    //!< Column for setting the minimum number of connections.
        ADHOC_VISIBILITY,   //!< Column for toggling ad-hoc visibility on/off.
        DESCRIPTION,        //!< Column for adding a description for the port.
        COLUMN_COUNT
    };
}

#endif // TRANSACTIONAL_PORT_COLUMNS_H