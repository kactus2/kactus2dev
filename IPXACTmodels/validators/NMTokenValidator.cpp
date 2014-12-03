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
    QRegExp exp;

    exp.setPattern("[a-zA-Z0-9:_.-]*$");
    
    exp.setPatternSyntax(QRegExp::W3CXmlSchema11);

    setRegExp(exp);
}

//-----------------------------------------------------------------------------
// Function: NMTokenValidator::~NMTokenValidator()
//-----------------------------------------------------------------------------
NMTokenValidator::~NMTokenValidator()
{

}