//-----------------------------------------------------------------------------
// File: ChoiceColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.10.2015
//
// Description:
// Common declarations for editing choices.
//-----------------------------------------------------------------------------

#ifndef CHOICECOLUMNS_H
#define CHOICECOLUMNS_H

namespace ChoiceColumns
{
    //! The editable columns in the choices editor.
    enum columns
    {
        CHOICE_NAME = 0,
        CHOICE_ENUMERATIONS,
        COLUMN_COUNT
    };
}

#endif // CHOICECOLUMNS_H