//-----------------------------------------------------------------------------
// File: WirePort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Implementation for ipxact:wirePort element within abstraction definition.
//-----------------------------------------------------------------------------

#include "WirePort.h"

#include <IPXACTmodels/common/TimingConstraint.h>

#include <IPXACTmodels/common/CellSpecification.h>

//-----------------------------------------------------------------------------
// Function: WirePort::WirePort()
//-----------------------------------------------------------------------------
WirePort::WirePort(): 
systemGroupName_(),
    presence_(PresenceTypes::UNKNOWN), 
    width_(),
    direction_(DirectionTypes::DIRECTION_INVALID),
    timingConstraint_(),
    driveConstraint_(),
    loadConstraint_(),
    mirroredTimingConstraint_(),
    mirroredDriveConstraint_(),
    mirroredLoadConstraint_()
{

}

//-----------------------------------------------------------------------------
// Function: WirePort::WirePort()
//-----------------------------------------------------------------------------
WirePort::WirePort(WirePort const& other): 
systemGroupName_(other.systemGroupName_),
    presence_(other.presence_),
    width_(other.width_),
    direction_(other.direction_),
    timingConstraint_(),
    driveConstraint_(),
    loadConstraint_(),
    mirroredTimingConstraint_(),
    mirroredDriveConstraint_(),
    mirroredLoadConstraint_()
{
    if (!other.timingConstraint_.isNull())
    {
        timingConstraint_ = QSharedPointer<TimingConstraint>(new TimingConstraint(*other.timingConstraint_));
    }

    if (!other.driveConstraint_.isNull())
    {
        driveConstraint_ = QSharedPointer<CellSpecification>(new CellSpecification(*other.driveConstraint_));
    }

    if (!other.driveConstraint_.isNull())
    {
        loadConstraint_ = QSharedPointer<CellSpecification>(new CellSpecification(*other.loadConstraint_));
    }

    if (!other.mirroredTimingConstraint_.isNull())
    {
        mirroredTimingConstraint_ = 
            QSharedPointer<TimingConstraint>(new TimingConstraint(*other.mirroredTimingConstraint_));
    }

    if (!other.mirroredDriveConstraint_.isNull())
    {
        mirroredDriveConstraint_ = 
            QSharedPointer<CellSpecification>(new CellSpecification(*other.mirroredDriveConstraint_));
    }

    if (!other.mirroredDriveConstraint_.isNull())
    {
        loadConstraint_ = QSharedPointer<CellSpecification>(new CellSpecification(*other.mirroredDriveConstraint_));
    }
}

//-----------------------------------------------------------------------------
// Function: WirePort::~WirePort()
//-----------------------------------------------------------------------------
WirePort::~WirePort()
{

}

//-----------------------------------------------------------------------------
// Function: WirePort::setSystemGroup()
//-----------------------------------------------------------------------------
void WirePort::setSystemGroup(QString const& groupName)
{
    systemGroupName_ = groupName;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getSystemGroup()
//-----------------------------------------------------------------------------
QString WirePort::getSystemGroup() const
{
    return systemGroupName_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setPresence()
//-----------------------------------------------------------------------------
void WirePort::setPresence(PresenceTypes::Presence presence)
{
    presence_ = presence;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getPresence()
//-----------------------------------------------------------------------------
PresenceTypes::Presence WirePort::getPresence() const
{
    return presence_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setWidth()
//-----------------------------------------------------------------------------
void WirePort::setWidth(QString const& widthExpression)
{
    width_ = widthExpression;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getWidth()
//-----------------------------------------------------------------------------
QString WirePort::getWidth() const
{
    return width_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setDirection()
//-----------------------------------------------------------------------------
void WirePort::setDirection(DirectionTypes::Direction direction)
{
    direction_ = direction;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction WirePort::getDirection() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setTimingConstraint()
//-----------------------------------------------------------------------------
void WirePort::setTimingConstraint(QSharedPointer<TimingConstraint> timeConstraint)
{
    timingConstraint_ = timeConstraint;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getTimingConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<TimingConstraint> WirePort::getTimingConstraint() const
{
    return timingConstraint_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setLoadConstraint()
//-----------------------------------------------------------------------------
void WirePort::setLoadConstraint(QSharedPointer<CellSpecification> loadConstraint)
{
    loadConstraint_ = loadConstraint;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getLoadConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<CellSpecification> WirePort::getLoadConstraint() const
{
    return loadConstraint_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setDriveConstraint()
//-----------------------------------------------------------------------------
void WirePort::setDriveConstraint(QSharedPointer<CellSpecification> driveConstraint)
{
    driveConstraint_ = driveConstraint;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getDriveConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<CellSpecification> WirePort::getDriveConstraint() const
{
    return driveConstraint_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setMirroredTimingConstraint()
//-----------------------------------------------------------------------------
void WirePort::setMirroredTimingConstraint(QSharedPointer<TimingConstraint> timeConstraint)
{
    mirroredTimingConstraint_ = timeConstraint;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getMirroredTimingConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<TimingConstraint> WirePort::getMirroredTimingConstraint() const
{
    return mirroredTimingConstraint_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setMirroredLoadConstraint()
//-----------------------------------------------------------------------------
void WirePort::setMirroredLoadConstraint(QSharedPointer<CellSpecification> loadConstraint)
{
    mirroredLoadConstraint_ = loadConstraint;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getMirroredLoadConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<CellSpecification> WirePort::getMirroredLoadConstraint() const
{
    return mirroredLoadConstraint_;
}

//-----------------------------------------------------------------------------
// Function: WirePort::setMirroredDriveConstraint()
//-----------------------------------------------------------------------------
void WirePort::setMirroredDriveConstraint(QSharedPointer<CellSpecification> driveConstraint)
{
    mirroredDriveConstraint_ = driveConstraint;
}

//-----------------------------------------------------------------------------
// Function: WirePort::getMirroredDriveConstraint()
//-----------------------------------------------------------------------------
QSharedPointer<CellSpecification> WirePort::getMirroredDriveConstraint() const
{
    return mirroredDriveConstraint_;
}
