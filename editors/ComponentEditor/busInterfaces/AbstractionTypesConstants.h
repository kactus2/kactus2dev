//-----------------------------------------------------------------------------
// File: AbstractionTypesConstants.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.01.2018
//
// Description:
// Common declarations for editing bus interface abstraction types.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTYPESCONSTANTS_H
#define ABSTRACTIONTYPESCONSTANTS_H

namespace AbstractionTypesConstants
{
    //! Defines the columns for the abstraction types table.
    enum Column
    {
        ABSTRACTIONDEFINITION = 0,
        VIEWREFERENCES,
        COLUMN_COUNT
    };

    //! Separating string for view references.
    const QString VIEW_SEPARATOR = QLatin1String("; ");

    //! Data role for gaining the VLNV of the referenced abstraction definition.
    const int ABSTRACTIONVLNVROLE = Qt::UserRole + 1;
}

#endif //ABSTRACTIONTYPESCONSTANTS_H