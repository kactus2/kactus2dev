//-----------------------------------------------------------------------------
// File: HierarchicalBusInterfaceTableAutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically connects active and hierarchical bus interfaces.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICALBUSINTERFACETABLEAUTOCONNECTOR_H
#define HIERARCHICALBUSINTERFACETABLEAUTOCONNECTOR_H

#include <editors/common/ComponentItemAutoConnector/BusInterfaceTableAutoConnector.h>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Automatically connects active and hierarchical bus interfaces.
//-----------------------------------------------------------------------------
class HierarchicalBusInterfaceTableAutoConnector : public BusInterfaceTableAutoConnector
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     Access to the document library.
     */
    HierarchicalBusInterfaceTableAutoConnector(LibraryInterface* library);

    /*!
     *  Destructor.
     */
    virtual ~HierarchicalBusInterfaceTableAutoConnector() = default;

    // No copying. No assignments.
    HierarchicalBusInterfaceTableAutoConnector(HierarchicalBusInterfaceTableAutoConnector const& rhs) = delete;
    HierarchicalBusInterfaceTableAutoConnector& operator=(HierarchicalBusInterfaceTableAutoConnector const& rhs) =
        delete;

private:

    /*!
     *  Get connectible bus interface modes for the selected interface mode.
     *
     *      @param [in] busMode     The selected bus interface mode.
     *
     *      @return List of connectible interface modes.
     */
    virtual QVector<General::InterfaceMode> getCompatibleInterfaceModes(General::InterfaceMode const& busMode)
        const;
};

//-----------------------------------------------------------------------------

#endif // HIERARCHICALBUSINTERFACETABLEAUTOCONNECTOR_H
