//-----------------------------------------------------------------------------
// File: PortWireTypeColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// Common declarations for editing port wire type definitions.
//-----------------------------------------------------------------------------

#ifndef PORTWIRETYPECOLUMNS_H
#define PORTWIRETYPECOLUMNS_H

namespace PortWireTypeColumns
{
    //-----------------------------------------------------------------------------
    // Constants defining which column represents what kind of information.
    //-----------------------------------------------------------------------------
    enum Columns
    {
        TYPE = 0,           //!< Column for the port type name and view references.
        TYPE_DEFINITIONS ,  //!< Column for the type definitions.
        COLUMN_COUNT
    };
}

#endif // PORTWIRETYPECOLUMNS_H