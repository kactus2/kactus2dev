//-----------------------------------------------------------------------------
// File: namevalidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.02.2011
//
// Description:
// Validator class for IP-Xact's Name-type.
//-----------------------------------------------------------------------------

#include "namevalidator.h"

#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: namevalidator::NameValidator()
//-----------------------------------------------------------------------------
NameValidator::NameValidator(QObject* parent):
QRegExpValidator(parent)
{
	QRegExp exp;

	exp.setPattern("^[a-zA-Z:_]+[a-zA-Z0-9:_.-]*$");

    exp.setPatternSyntax(QRegExp::W3CXmlSchema11);

	setRegExp(exp);
}

//-----------------------------------------------------------------------------
// Function: namevalidator::~NameValidator()
//-----------------------------------------------------------------------------
NameValidator::~NameValidator()
{

}
