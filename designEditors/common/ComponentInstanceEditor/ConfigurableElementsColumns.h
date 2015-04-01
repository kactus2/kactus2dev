//-----------------------------------------------------------------------------
// File: ConfigurableElementColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.01.2015
//
// Description:
// Common declarations for editing configurable element values.
//-----------------------------------------------------------------------------

namespace ConfigurableElementsColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum Columns
    {
        NAME,               //!< Column for the name of the configurable element.
        VALUE,              //!< Column for the configurable value.
        DEFAULT_VALUE,      //!< Column for the original value of the parameter.
        CHOICE,             //!< Column for the selected choice of the parameter.
        ARRAY_LEFT,         //!< Column for the array left side of the parameter.
        TYPE,               //!< Column for the type of the parameter.
        COLUMN_COUNT
    };
}
