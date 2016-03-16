//-----------------------------------------------------------------------------
// File: ClockUnit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 13.10.2015
//
// Description:
// Container for a clock unit.
//-----------------------------------------------------------------------------

#include "ClockUnit.h"

//-----------------------------------------------------------------------------
// Function: ClockUnit::ClockUnit()
//-----------------------------------------------------------------------------
ClockUnit::ClockUnit(QString const& value) :
value_(value),
timeUnit_(TIMEUNIT_UNSPECIFIED)
{

}

//-----------------------------------------------------------------------------
// Function: ClockUnit::ClockUnit()
//-----------------------------------------------------------------------------
ClockUnit::ClockUnit(const ClockUnit& other) :
value_(other.value_),
timeUnit_(other.timeUnit_)
{

}

//-----------------------------------------------------------------------------
// Function: ClockUnit::operator=()
//-----------------------------------------------------------------------------
ClockUnit& ClockUnit::operator=(const ClockUnit& other)
{
    if (this != &other)
    {
        value_ = other.value_;
        timeUnit_ = other.timeUnit_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ClockUnit::~ClockUnit()
//-----------------------------------------------------------------------------
ClockUnit::~ClockUnit()
{

}

//-----------------------------------------------------------------------------
// Function: ClockUnit::getValue()
//-----------------------------------------------------------------------------
QString ClockUnit::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: ClockUnit::setValue()
//-----------------------------------------------------------------------------
void ClockUnit::setValue(QString const& newValue)
{
    value_ = newValue;
}

//-----------------------------------------------------------------------------
// Function: ClockUnit::getTimeUnit()
//-----------------------------------------------------------------------------
ClockUnit::TimeUnit ClockUnit::getTimeUnit() const
{
    return timeUnit_;
}

//-----------------------------------------------------------------------------
// Function: ClockUnit::setTimeUnit()
//-----------------------------------------------------------------------------
void ClockUnit::setTimeUnit(TimeUnit newTimeUnit)
{
    timeUnit_ = newTimeUnit;
}

//-----------------------------------------------------------------------------
// Function: ClockUnit::setTimeUnit()
//-----------------------------------------------------------------------------
void ClockUnit::setTimeUnit(QString const& newTimeUnit)
{
    if (newTimeUnit=="ns")
    {
        setTimeUnit(NS);
    }
    else if (newTimeUnit=="ps")
    {
        setTimeUnit(PS);
    }
    else
    {
        setTimeUnit(TIMEUNIT_UNSPECIFIED);
    }
}

//-----------------------------------------------------------------------------
// Function: ClockUnit::timeUnitToString()
//-----------------------------------------------------------------------------
QString ClockUnit::timeUnitToString() const
{
    if (timeUnit_ == NS)
    {
        return QString("ns");
    }
    else if (timeUnit_ == PS)
    {
        return QString("ps");
    }
    else
    {
        return QString();
    }
}