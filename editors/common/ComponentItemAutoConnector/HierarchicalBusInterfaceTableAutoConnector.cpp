//-----------------------------------------------------------------------------
// File: HierarchicalBusInterfaceTableAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically connects active and hierarchical bus interfaces.
//-----------------------------------------------------------------------------

#include "HierarchicalBusInterfaceTableAutoConnector.h"

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceTableAutoConnector::HierarchicalBusInterfaceTableAutoConnector()
//-----------------------------------------------------------------------------
HierarchicalBusInterfaceTableAutoConnector::HierarchicalBusInterfaceTableAutoConnector(LibraryInterface* library):
BusInterfaceTableAutoConnector(library)
{

}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceTableAutoConnector::getCompatibleInterfaceModes()
//-----------------------------------------------------------------------------
QVector<General::InterfaceMode> HierarchicalBusInterfaceTableAutoConnector::getCompatibleInterfaceModes(
    General::InterfaceMode const& busMode) const
{
    return General::getCompatibleInterfaceModeForHierarchicalInterface(busMode);
}
