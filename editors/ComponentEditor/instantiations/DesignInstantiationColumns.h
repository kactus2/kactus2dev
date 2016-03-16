//-----------------------------------------------------------------------------
// File: DesignInstantiationsColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Common declarations for columns editing design (configurations) instantiations.
//-----------------------------------------------------------------------------

#ifndef DESIGNINSTANTIATIONSCOLUMNS_H
#define DESIGNINSTANTIATIONSCOLUMNS_H

namespace DesignInstantiationsColumns
{
    //! Defines the columns for the component design (configuration) table.
    enum Column
    {
        NAME = 0,
        DISPLAY_NAME,
        VLNV_REFERENCE,
        DESCRIPTION,
        COLUMN_COUNT
    };
}

#endif //DESIGNINSTANTIATIONSCOLUMNS_H