//-----------------------------------------------------------------------------
// File: HierarchicalSaveColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.05.2015
//
// Description:
// Common definitions for hierarchical save model.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICAL_SAVE_COLUMNS_H
#define HIERARCHICAL_SAVE_COLUMNS_H

namespace HierarchicalSaveColumns
{
    //! Defines the columns for the hierarchical save tree.
    enum Column
    {
        
        VLNV = 0,
        SAVE_AS_VLNV,
        SAVE,
        COLUMN_COUNT
    };
}

#endif //HIERARCHICAL_SAVE_COLUMNS_H