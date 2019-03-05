//-----------------------------------------------------------------------------
// File: PortUtilities.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.02.2019
//
// Description:
// Utilities for port compatibilities.
//-----------------------------------------------------------------------------

#include "PortUtilities.h"

//-----------------------------------------------------------------------------
// Function: PortItemMatcher::getConnectableDirections()
//-----------------------------------------------------------------------------
QVector<DirectionTypes::Direction> PortUtilities::getConnectableDirections(
    DirectionTypes::Direction portDirection)
{
    //! In -> Out & InOut & phantom
    //! Out -> In & InOut & phantom
    //! InOut -> InOut & phantom
    //! Phantom -> In & Out & InOut & Phantom
    QVector<DirectionTypes::Direction> connectableDirections;
    if (portDirection == DirectionTypes::IN)
    {
        connectableDirections = { DirectionTypes::OUT, DirectionTypes::INOUT, DirectionTypes::DIRECTION_PHANTOM };
    }
    else if (portDirection == DirectionTypes::OUT)
    {
        connectableDirections = { DirectionTypes::IN, DirectionTypes::INOUT, DirectionTypes::DIRECTION_PHANTOM };
    }
    else if (portDirection == DirectionTypes::INOUT)
    {
        connectableDirections =
        { DirectionTypes::IN, DirectionTypes::OUT, DirectionTypes::INOUT, DirectionTypes::DIRECTION_PHANTOM };
    }
    else if (portDirection == DirectionTypes::DIRECTION_PHANTOM)
    {
        connectableDirections =
        { DirectionTypes::IN, DirectionTypes::OUT, DirectionTypes::INOUT, DirectionTypes::DIRECTION_PHANTOM };
    }

    return connectableDirections;
}

//-----------------------------------------------------------------------------
// Function: PortUtilities::getConnectableHierarchicalDirections()
//-----------------------------------------------------------------------------
QVector<DirectionTypes::Direction> PortUtilities::getConnectableHierarchicalDirections(
    DirectionTypes::Direction portDirection)
{
    QVector<DirectionTypes::Direction> connectableDirections;

    connectableDirections.append(portDirection);

    return connectableDirections;
}

//-----------------------------------------------------------------------------
// Function: PortUtilities::getDirectionIconPath()
//-----------------------------------------------------------------------------
QString PortUtilities::getDirectionIconPath(DirectionTypes::Direction portDirection)
{
    QString iconPath = ":icons/common/graphics/cross.png";
    if (portDirection == DirectionTypes::IN)
    {
        iconPath = ":icons/common/graphics/input.png";
    }
    else if (portDirection == DirectionTypes::OUT)
    {
        iconPath = ":icons/common/graphics/output.png";
    }
    else if (portDirection == DirectionTypes::INOUT)
    {
        iconPath = ":icons/common/graphics/inout.png";
    }
    else if (portDirection == DirectionTypes::DIRECTION_PHANTOM)
    {
        iconPath = ":icons/common/graphics/phantom.png";
    }

    return iconPath;
}
