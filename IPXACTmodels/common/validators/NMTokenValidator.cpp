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

#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: NMTokenValidator::NMTokenValidator()
//-----------------------------------------------------------------------------
NMTokenValidator::NMTokenValidator(QObject* parent):
QRegExpValidator(parent)
{
    QRegExp exp(QStringLiteral("[a-zA-Z0-9:_.-]*$"), Qt::CaseSensitive, QRegExp::W3CXmlSchema11);
    setRegExp(exp);
}
