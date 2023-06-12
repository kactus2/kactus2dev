//-----------------------------------------------------------------------------
// File: RenodeColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.03.2023
//
// Description:
// Utility columns for Renode generator.
//-----------------------------------------------------------------------------

#ifndef RENODECOLUMNS_H
#define RENODECOLUMNS_H

#include <QString>

//-----------------------------------------------------------------------------
//! Columns for Renode peripherals.
//-----------------------------------------------------------------------------
namespace PeripheralColumns
{
    //! The editable columns in the peripheral editor.
    enum columns
    {
        NAME = 0,
        BASEADDRESS,
        SIZE,
        CLASS,
        INITABLE,
        TEMPLATE,
        FILEPATH,
        COLUMN_COUNT
    };
}

//-----------------------------------------------------------------------------
//! Columns for Renode memories.
//-----------------------------------------------------------------------------
namespace MemoryColumns
{
    //! The editable columns in the memory editor.
    enum columns
    {
        NAME = 0,
        CLASS,
        BASEADDRESS,
        SIZE,
        COLUMN_COUNT
    };

};

//-----------------------------------------------------------------------------
//! Columns for Renode peripheral templates.
//-----------------------------------------------------------------------------
namespace PeripheralTemplateColumns
{
    //! The columns in the peripheral template editor.
    enum columns
    {
        NAME = 0,
		TEMPLATE,
        PATH,
        COLUMN_COUNT
    };
};

#endif // RENODECOLUMNS_H