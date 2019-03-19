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
        NAMESPACE,          //! Column for the name space of the vendor extension.
        NAME,               //! Column for the name of the vendor extension.
        TYPE,               //! Column for the type attribute of the vendor extension.
        VALUE,              //! Column for the configurable value.
        DESCRIPTION,        //! Column for description.
        COLUMN_COUNT
    };
}
