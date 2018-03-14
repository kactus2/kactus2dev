/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 *         filename: utils.cpp
 *         
 *         Description: This file contains definitions for general purpose 
 *         functions that can be used in the whole software.
 */

#include "utils.h"

#include <qmath.h>
#include <QRegExp>

quint64 Utils::str2Uint( const QString& str )
{
    if (str.isEmpty())
    {
        return 0;
    }

    // used to detect if the conversion was successful
    bool success = true;
    quint64 number = 0;

    // if starts with "0x" then it is hexadecimal digit
    if (str.startsWith(QStringLiteral("0x"), Qt::CaseInsensitive))
    {
        number = str.toULongLong(&success, 16);
        if (success)
        {
            return number;
        }
    }

    // needed because the last letter is chopped if one is found
    QString strNumber = str;

    // the multiple is the last letter if one exists
    const QChar multiple = strNumber.at(strNumber.size()-1);
    quint64 multiplier = 1;

    // get the correct multiplier and remove the letter from the string
    if (multiple == QLatin1Char('k') || multiple == QLatin1Char('K'))
    {
        multiplier = qPow(2, 10);
        strNumber.chop(1);
    }
    else if (multiple == QLatin1Char('M'))
    {
        multiplier = qPow(2, 20);
        strNumber.chop(1);
    }
    else if (multiple == QLatin1Char('G'))
    {
        multiplier = qPow(2, 30);
        strNumber.chop(1);
    }
    else if (multiple == QLatin1Char('T'))
    {
        multiplier = qPow(2, 40);
        strNumber.chop(1);
    }
    else if (multiple == QLatin1Char('P'))
    {
        multiplier = qPow(2, 50);
        strNumber.chop(1);
    }

    // try to convert the number 
    number = strNumber.toULongLong(&success);
    
    if (success)
    {
        return number * multiplier;    
    }
        
    return 0;
}

qint64 Utils::str2Int( const QString& str )
{
    if (str.isEmpty())
    {
        return 0;
    }

    bool negative = false;
    QString absValueStr = str;

    // if the number is negative
    if (str.startsWith(QLatin1Char('-')))
    {
        negative = true;
        // remove the negative sign from the start
        absValueStr = str.mid(1);
    }

    // get the absolute value of the number
    quint64 absValue = Utils::str2Uint(absValueStr);

    if (negative)
    {
        return -static_cast<qint64>(absValue);
    }
    else
    {
        return absValue;
    }
}

Utils::ImplementationOptions::ImplementationOptions():
hw_(true),
sw_(true),
system_(true) {
}

Utils::HierarchyOptions::HierarchyOptions():
flat_(true),
product_(true),
board_(true),
chip_(true),
soc_(true),
ip_(true) {
}

Utils::FirmnessOptions::FirmnessOptions():
templates_(true),
mutable_(true),
fixed_(true)
{
}

Utils::TypeOptions::TypeOptions():
components_(true),
buses_(true),
catalogs_(true),
apis_(false),
advanced_(false)
{
}

//-----------------------------------------------------------------------------
// Function: Utils::getCurrentUser()
//-----------------------------------------------------------------------------
KACTUS2_API QString Utils::getCurrentUser()
{
#ifdef Q_OS_WIN
#pragma warning (disable: 4996)
    return getenv("USERNAME");
#pragma warning (default: 4996)
#else
    return getenv("USER");
#endif
}
