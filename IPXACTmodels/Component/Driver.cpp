//-----------------------------------------------------------------------------
// File: Driver.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 7.11.2023
//
// Description:
// Implementation of ipxact:driver for wire ports.
//-----------------------------------------------------------------------------

#include "Driver.h"

#include <QList>
#include <QString>

//-----------------------------------------------------------------------------
// Function: Driver::getRange()
//-----------------------------------------------------------------------------
Range Driver::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: Driver::setRange()
//-----------------------------------------------------------------------------
void Driver::setRange(Range const& range)
{
    range_ = range;
}

//-----------------------------------------------------------------------------
// Function: Driver::getDefaultValue()
//-----------------------------------------------------------------------------
QString Driver::getDefaultValue() const
{
    return defaultValue_;
}

//-----------------------------------------------------------------------------
// Function: Driver::setDefaultValue()
//-----------------------------------------------------------------------------
void Driver::setDefaultValue(QString const& value)
{
    defaultValue_ = value;
}
