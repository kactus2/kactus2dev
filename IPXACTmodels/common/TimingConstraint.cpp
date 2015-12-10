//-----------------------------------------------------------------------------
// File: TimingConstraint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2015
//
// Description:
// Implementation for ipxact:timingConstraint.
//-----------------------------------------------------------------------------

#include "TimingConstraint.h"

//-----------------------------------------------------------------------------
// Function: TimingConstraint::TimingConstraint()
//-----------------------------------------------------------------------------
TimingConstraint::TimingConstraint(): value_(), clockEdge_(UNKNOWN), delayType_(BOTH), clockPortName_()
{

}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::TimingConstraint()
//-----------------------------------------------------------------------------
TimingConstraint::TimingConstraint(TimingConstraint const& other) :
value_(other.value_),
    clockEdge_(other.clockEdge_),
    delayType_(other.delayType_),
    clockPortName_(other.clockPortName_)
{

}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::~TimingConstraint()
//-----------------------------------------------------------------------------
TimingConstraint::~TimingConstraint()
{

}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::setValue()
//-----------------------------------------------------------------------------
void TimingConstraint::setValue(double delayPercentage)
{
    value_ = delayPercentage;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::getValue()
//-----------------------------------------------------------------------------
double TimingConstraint::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::setClockEdge()
//-----------------------------------------------------------------------------
void TimingConstraint::setClockEdge(ClockEdgeType edge)
{
    clockEdge_ = edge;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::getClockEdge()
//-----------------------------------------------------------------------------
TimingConstraint::ClockEdgeType TimingConstraint::getClockEdge() const
{
    return clockEdge_;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::setDelayType()
//-----------------------------------------------------------------------------
void TimingConstraint::setDelayType(TimingConstraint::DelayType delayType)
{
    delayType_ = delayType;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::getDelayType()
//-----------------------------------------------------------------------------
TimingConstraint::DelayType TimingConstraint::getDelayType() const
{
    return delayType_;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::setClockName()
//-----------------------------------------------------------------------------
void TimingConstraint::setClockName(QString const& clockPort)
{
    clockPortName_ = clockPort;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraint::getClockName()
//-----------------------------------------------------------------------------
QString TimingConstraint::getClockName() const
{
    return clockPortName_;
}
