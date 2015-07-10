//-----------------------------------------------------------------------------
// File: EnumeratedValueColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.07.2015
//
// Description:
// Common declarations for editing enumeration values of register fields.
//-----------------------------------------------------------------------------

#ifndef ENUMERATEDVALUECOLUMNS_H
#define ENUMERATEDVALUECOLUMNS_H

namespace EnumeratedValueColumns
{
    enum columns
    {
        NAME_COLUMN = 0,
        DISPLAY_NAME,
        VALUE_COLUMN,
        USAGE_COLUMN,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif // ENUMERATEDVALUECOLUMNS_H