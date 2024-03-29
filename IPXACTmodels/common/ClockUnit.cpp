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
value_(value)
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
    if (newTimeUnit == QLatin1String("ns"))
    {
        setTimeUnit(NS);
    }
    else if (newTimeUnit == QLatin1String("ps"))
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
        return QStringLiteral("ns");
    }
    else if (timeUnit_ == PS)
    {
        return QStringLiteral("ps");
    }
    else
    {
        return QString();
    }
}