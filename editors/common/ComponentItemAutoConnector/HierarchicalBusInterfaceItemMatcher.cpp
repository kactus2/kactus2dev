//-----------------------------------------------------------------------------
// File: HierarchicalBusInterfaceItemMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically defines possible matches between an active and a hierarchical bus interface.
//-----------------------------------------------------------------------------

#include "HierarchicalBusInterfaceItemMatcher.h"

#include <KactusAPI/include/BusInterfaceUtilities.h>

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItemMatcher::HierarchicalBusInterfaceItemMatcher()
//-----------------------------------------------------------------------------
HierarchicalBusInterfaceItemMatcher::HierarchicalBusInterfaceItemMatcher(LibraryInterface* library):
BusInterfaceItemMatcher(library)
{

}

//-----------------------------------------------------------------------------
// Function: HierarchicalBusInterfaceItemMatcher::getCompatibleModes()
//-----------------------------------------------------------------------------
QVector<General::InterfaceMode> HierarchicalBusInterfaceItemMatcher::getCompatibleModes(
    General::InterfaceMode busMode) const
{
    return General::getCompatibleInterfaceModeForHierarchicalInterface(busMode);
}
