//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// File: VendorExtensionColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.12.2018
//
// Description:
// Common declarations for editing vendor extensions.
//-----------------------------------------------------------------------------

namespace VendorExtensionColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum Columns
    {
        NAME,               //!< Column for the name of the vendor extension.
        TYPE,               //!< Column for the type attribute of the vendor extension.
        VALUE,              //!< Column for the configurable value.
        COLUMN_COUNT
    };
}
