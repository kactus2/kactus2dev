//-----------------------------------------------------------------------------
// File: ValueFormatter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.12.2014
//
// Description:
// Formatter for numeric values.
//-----------------------------------------------------------------------------

#include "ValueFormatter.h"

//-----------------------------------------------------------------------------
// Function: ValueFormatter::ValueFormatter()
//-----------------------------------------------------------------------------
ValueFormatter::ValueFormatter()
{

}

//-----------------------------------------------------------------------------
// Function: ValueFormatter::~ValueFormatter()
//-----------------------------------------------------------------------------
ValueFormatter::~ValueFormatter()
{

}

//-----------------------------------------------------------------------------
// Function: ValueFormatter::format()
//-----------------------------------------------------------------------------
QString ValueFormatter::format(QString const& value, int base) const
{
    if (base == 2)
    {
        return "'o" + QString::number(value.toInt(0), 2);
    }
    else if (base == 8)
    {
        return "'o" + QString::number(value.toInt(0), 8);
    }
    else if (base == 16)
    {
        return "'h" + QString::number(value.toInt(0), 16);
    }
    else
    {
        return value;
    }
}
