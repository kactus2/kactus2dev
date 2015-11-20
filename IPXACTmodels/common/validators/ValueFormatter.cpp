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

#include <QStringList>
#include <QRegularExpression>

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
    if (value.contains('{') && value.contains('}'))
    {
        return formatArray(value, base);
    }

    else if (base == 2)
    {
        return "'b" + QString::number(value.toInt(), 2);
    }
    else if (base == 8)
    {
        return "'o" + QString::number(value.toInt(), 8);
    }
    else if (base == 16)
    {
        return "'h" + QString::number(value.toInt(), 16);
    }
    else
    {
        return value;
    }
}

//-----------------------------------------------------------------------------
// Function: ValueFormatter::formatArray()
//-----------------------------------------------------------------------------
QString ValueFormatter::formatArray(QString const& arrayValue, int& base) const
{
    QStringList subValues = arrayValue.split(',');

    QRegularExpression arrayStart ("^'?{");

    subValues.first().remove(arrayStart);
    subValues.last().chop(1);

    QString formattedArray("{");
    for (int arrayIndex = 0; arrayIndex < subValues.size(); ++arrayIndex)
    {
        formattedArray.append(format(subValues.at(arrayIndex), base));
        if (arrayIndex < subValues.size() - 1)
        {
            formattedArray.append(',');
        }
    }
    formattedArray.append('}');

    return formattedArray;
}