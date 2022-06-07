//-----------------------------------------------------------------------------
// File: MasterPortInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.12.2020
//
// Description:
// Master interface for editing component ports and port abstractions.
//-----------------------------------------------------------------------------

#include "MasterPortInterface.h"

#include <IPXACTmodels/common/TransactionalTypes.h>

//-----------------------------------------------------------------------------
// Function: MasterPortInterface::MasterPortInterface()
//-----------------------------------------------------------------------------
MasterPortInterface::MasterPortInterface():
NameGroupInterface()
{

}

//-----------------------------------------------------------------------------
// Function: MasterPortInterface::getIconPathForMissingPort()
//-----------------------------------------------------------------------------
std::string MasterPortInterface::getIconPathForMissingPort() const
{
    QString missingPath(QLatin1String(":icons/common/graphics/cross.png"));
    return missingPath.toStdString();
}

//-----------------------------------------------------------------------------
// Function: MasterPortInterface::getIconPathForDirection()
//-----------------------------------------------------------------------------
std::string MasterPortInterface::getIconPathForDirection(DirectionTypes::Direction direction) const
{
    std::string directionPath(":icons/common/graphics/cross.png");
    if (direction == DirectionTypes::IN)
    {
        directionPath = ":icons/common/graphics/input.png";
    }
    else if (direction == DirectionTypes::OUT)
    {
        directionPath = ":icons/common/graphics/output.png";
    }
    else if (direction == DirectionTypes::INOUT)
    {
        directionPath = ":icons/common/graphics/inout.png";
    }
    else if (direction == DirectionTypes::DIRECTION_PHANTOM)
    {
        directionPath = ":icons/common/graphics/phantom.png";
    }

    return directionPath;
}

//-----------------------------------------------------------------------------
// Function: MasterPortInterface::getIconPathForInitiative()
//-----------------------------------------------------------------------------
std::string MasterPortInterface::getIconPathForInitiative(QString const& initiative) const
{
    std::string iconPath(":icons/common/graphics/cross.png");
    if (initiative.compare(TransactionalTypes::INITIATIVE_PROVIDES, Qt::CaseInsensitive) == 0)
    {
        iconPath = ":icons/common/graphics/provides.png";
    }
    else if (initiative.compare(TransactionalTypes::INITIATIVE_REQUIRES, Qt::CaseInsensitive) == 0)
    {
        iconPath = ":icons/common/graphics/requires.png";
    }
    else if (initiative.compare(TransactionalTypes::INITIATIVE_BOTH, Qt::CaseInsensitive) == 0)
    {
        iconPath = ":icons/common/graphics/requires_provides.png";
    }
    else if (initiative.compare(TransactionalTypes::INITIATIVE_PHANTOM, Qt::CaseInsensitive) == 0)
    {
        iconPath = ":icons/common/graphics/phantom.png";
    }

    return iconPath;
}