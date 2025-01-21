//-----------------------------------------------------------------------------
// File: BusInterfaceUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.02.2019
//
// Description:
// Utilities for bus interface mode compatibilities.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACECOMPATIBILITIES_H
#define BUSINTERFACECOMPATIBILITIES_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <QSharedPointer>

#include <IPXACTmodels/generaldeclarations.h>


class BusDefinition;
class VLNV;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Utilities for bus interface mode compatibilities.
//-----------------------------------------------------------------------------
namespace BusInterfaceUtilities
{
    /*!
     *  Checks if the selected bus definitions match.
     *
     *    @param [in] firstDefinition         The first bus definition.
     *    @param [in] secondDefinitionVLNV    VLNV of the second bus definition.
     *
     *    @return True, if the bus definitions match, false otherwise.
     */
    KACTUS2_API bool hasMatchingBusDefinitions(QSharedPointer<const BusDefinition> firstDefinition,
        VLNV const& secondDefinitionVLNV, LibraryInterface* library);

    /*!
     *  Check if the selected bus definition VLNVs can be connected.
     *
     *    @param [in] firstDefinitionVLNV     The first VLNV.
     *    @param [in] secondDefinitionVLNV    The second VLNV.
     *    @param [in] library                 Access to the library.
     *
     *    @return True, if the selected bus definition VLNVs can be connected, false otherwise.
     */
    KACTUS2_API bool busDefinitionVLNVsMatch(VLNV const& firstDefinitionVLNV, VLNV const& secondDefinitionVLNV,
        LibraryInterface* library);

    /*!
     *  Get the path for the icon of the bus interface mode.
     *
     *    @param [in] busMode     The selected bus interface mode.
     *
     *    @return Path for the bus interface mode icon.
     */
    KACTUS2_API QString getIconPath(General::InterfaceMode busMode);
}

//-----------------------------------------------------------------------------

#endif // BUSINTERFACECOMPATIBILITIES_H
