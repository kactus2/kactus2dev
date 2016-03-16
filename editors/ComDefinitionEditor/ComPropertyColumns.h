//-----------------------------------------------------------------------------
// File: ComPropertyColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.10.2015
//
// Description:
// Common declarations for editing Com Definitions.
//-----------------------------------------------------------------------------

namespace ComPropertyColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum PropertyColumn
    {
        NAME = 0,           //!< Column for the property name.
        REQUIRED,           //!< Column for the required/optional check box.
        TYPE,               //!< Column for setting the value type for the property.
        DEFAULT_VALUE,      //!< Column for setting the default value for the property.
        DESCRIPTION,        //!< Column for adding a description for the property.
        COLUMN_COUNT
    };
}
