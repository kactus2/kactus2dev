//-----------------------------------------------------------------------------
// File: EnumerationColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.10.2022
//
// Description:
// Common declarations for editing choice enumerations.
//-----------------------------------------------------------------------------

#ifndef ENUMERATIONCOLUMNS_H
#define ENUMERATIONCOLUMNS_H

namespace EnumerationColumns
{
    //! The editable columns in the enumerations editor.
    enum columns
    {
		ENUMERATION = 0,
		TEXT,
		HELP,
		COLUMN_COUNT
    };
}

#endif // ENUMERATIONCOLUMNS_H