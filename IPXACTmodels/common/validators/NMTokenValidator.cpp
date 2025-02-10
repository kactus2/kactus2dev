//-----------------------------------------------------------------------------
// File: NMTokenValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.12.2014
//
// Description:
// Validator for IP-Xact's NMtoken-type
//-----------------------------------------------------------------------------

#include "NMTokenValidator.h"

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: NMTokenValidator::NMTokenValidator()
//-----------------------------------------------------------------------------
NMTokenValidator::NMTokenValidator(QObject* parent):
QRegularExpressionValidator(parent)
{
    QRegularExpression exp(QStringLiteral("([a-zA-Z0-9:_.-]*$)|(^$)"));
    setRegularExpression(exp);
}
