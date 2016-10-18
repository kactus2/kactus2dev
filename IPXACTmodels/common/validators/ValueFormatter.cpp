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

#include <QStringBuilder>
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
    if (value.contains(QLatin1Char('{')) && value.contains(QLatin1Char('}')))
    {
        return formatArray(value, base);
    }

    else if (base == 2)
    {
        return QStringLiteral("'b") + QString::number(value.toInt(), 2);
    }
    else if (base == 8)
    {
        return QStringLiteral("'o") + QString::number(value.toInt(), 8);
    }
    else if (base == 16)
    {
        return QStringLiteral("'h") + QString::number(value.toInt(), 16);
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
    QStringList subValues = arrayValue.split(QLatin1Char(','));

    QRegularExpression arrayStart(QStringLiteral("^'?{"));

    subValues.first().remove(arrayStart);
    subValues.last().chop(1);

    QString formattedArray(QStringLiteral("{"));
    for (int arrayIndex = 0; arrayIndex < subValues.size(); ++arrayIndex)
    {
        formattedArray.append(format(subValues.at(arrayIndex), base));
        if (arrayIndex < subValues.size() - 1)
        {
            formattedArray.append(QLatin1Char(','));
        }
    }
    formattedArray.append(QLatin1Char('}'));

    return formattedArray;
}