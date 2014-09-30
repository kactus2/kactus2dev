//-----------------------------------------------------------------------------
// File: HDLmath.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 4.7.2013
//
// Description:
// This file contains common elements of math in different HDLs.
//-----------------------------------------------------------------------------

#ifndef HDLMATH_H
#define HDLMATH_H

#include <QRegExp>

namespace HDLmath
{
    //! Power operator **.
    const QRegExp POWER = QRegExp("[/*]{2}");           

    //! Multiply * and divide /.
    const QRegExp MULTIPLYDIVIDE = QRegExp("[/*/]");

    //! Supported mathematical operators in math expressions.
    const QString OPERATOR = "[+-]|" + POWER.pattern() + "|" + MULTIPLYDIVIDE.pattern();

    //! Mathematical terms e.g. x or x + y.
    const QString TERM = "(?:\\w+)(?:\\s*(?:" + OPERATOR + ")\\s*(?:\\w+))*";

    //! Mathematical expressions for capturing.
    const QRegExp OPERATION_EXP = QRegExp(QString(TERM).remove("?:"));
}

#endif // HDLMATH_H
