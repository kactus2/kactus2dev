//-----------------------------------------------------------------------------
// File: HierarchicalBusInterfaceItemMatcher.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically defines possible matches between an active and a hierarchical bus interface.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICALBUSINTERFACEITEMMATCHER_H
#define HIERARCHICALBUSINTERFACEITEMMATCHER_H

#include <IPXACTmodels/generaldeclarations.h>

#include <editors/common/ComponentItemAutoConnector/BusInterfaceItemMatcher.h>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Automatically defines possible matches between an active and a hierarchical bus interface.
//-----------------------------------------------------------------------------
class HierarchicalBusInterfaceItemMatcher : public BusInterfaceItemMatcher
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     Access to the document library.
     */
    HierarchicalBusInterfaceItemMatcher(LibraryInterface* library);

    /*!
     *  Destructor.
     */
    virtual ~HierarchicalBusInterfaceItemMatcher() = default;

    // No copying. No assignments.
    HierarchicalBusInterfaceItemMatcher(HierarchicalBusInterfaceItemMatcher const& rhs) = delete;
    HierarchicalBusInterfaceItemMatcher& operator=(HierarchicalBusInterfaceItemMatcher const& rhs) = delete;

private:

    /*!
     *  Get connectible bus interface modes for the selected interface mode.
     *
     *      @param [in] busMode     The selected bus interface mode.
     *
     *      @return List of connectible interface modes.
     */
    virtual QVector<General::InterfaceMode> getCompatibleModes(General::InterfaceMode busMode) const;
};

//-----------------------------------------------------------------------------

#endif // HIERARCHICALBUSINTERFACEITEMMATCHER_H
