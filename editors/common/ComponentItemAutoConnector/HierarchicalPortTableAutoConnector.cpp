//-----------------------------------------------------------------------------
// File: HierarchicalPortTableAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically connects ports of a component item and a top level component.
//-----------------------------------------------------------------------------

#include "HierarchicalPortTableAutoConnector.h"

#include <editors/common/PortUtilities.h>

//-----------------------------------------------------------------------------
// Function: HierarchicalPortTableAutoConnector::getConnectableDirections()
//-----------------------------------------------------------------------------
QVector<DirectionTypes::Direction> HierarchicalPortTableAutoConnector::getConnectableDirections(
    DirectionTypes::Direction portDirection) const
{
    return PortUtilities::getConnectableHierarchicalDirections(portDirection);
}
