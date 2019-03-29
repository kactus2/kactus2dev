////-----------------------------------------------------------------------------
// File: BusDefinitionUtils.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.03.2019
//
// Description:
// Utilities for Bus Definitions and abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef BUSDEFINITIONUTILS_H
#define BUSDEFINITIONUTILS_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>

class LibraryInterface;
class BusDefinition;

//-----------------------------------------------------------------------------
//! Utilities for Bus Definitions and abstraction definitions.
//-----------------------------------------------------------------------------
namespace BusDefinitionUtils
{
    /*!
     *  Get a list of the system groups from the selected bus definition.
     *
     *      @param [in] bus             The selected bus definition.
     *      @param [in] libraryAccess   Interface to the library.
     *
     *      @return List of bus definition system groups.
     */
    IPXACTMODELS_EXPORT QStringList getSystemGroups(QSharedPointer<const BusDefinition> bus,
        LibraryInterface* libraryAccess);
};

//-----------------------------------------------------------------------------

#endif // BUSDEFINITIONUTILS_H
