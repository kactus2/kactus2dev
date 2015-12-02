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
        ADHOC_BOUNDS_COL_NAME = 0,  //!< Column for the port name.
        ADHOC_BOUNDS_COL_LEFT,      //!< Column for the port's left bound.
        ADHOC_BOUNDS_COL_RIGHT,     //!< Column for the port's right bound.
        ADHOC_BOUNDS_COL_COUNT
    };

    //! Custom roles for managing port bound limits.
    enum Roles
    {
        UpperPortBoundRole = Qt::UserRole + 1,
        LowerPortBoundRole = Qt::UserRole + 2
    };
}
