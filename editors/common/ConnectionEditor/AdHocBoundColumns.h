//-----------------------------------------------------------------------------
// File: AdHocBoundColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.10.2015
//
// Description:
// Constants for defining common elements in the AdHocBound editing.
//-----------------------------------------------------------------------------

namespace AdHocBoundColumns
{
    //! Constants defining which column represents what kind of information.
    enum Columns
    {
        NAME = 0,  //!< Column for the port name.
        LEFT_BOUND,      //!< Column for the port's left bound.
        RIGHT_BOUND,     //!< Column for the port's right bound.
        COLUMN_COUNT
    };

}
