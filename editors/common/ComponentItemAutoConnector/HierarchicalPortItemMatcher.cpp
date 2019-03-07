//-----------------------------------------------------------------------------
// File: HierarchicalPortItemMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically defines possible matches between an active port and a hierarchical one.
//-----------------------------------------------------------------------------

#include "HierarchicalPortItemMatcher.h"

#include <editors/common/PortUtilities.h>

//-----------------------------------------------------------------------------
// Function: HierarchicalPortItemMatcher::getConnectableDirections()
//-----------------------------------------------------------------------------
QVector<DirectionTypes::Direction> HierarchicalPortItemMatcher::getConnectableDirections(
    DirectionTypes::Direction portDirection) const
{
    return PortUtilities::getConnectableHierarchicalDirections(portDirection);
}
