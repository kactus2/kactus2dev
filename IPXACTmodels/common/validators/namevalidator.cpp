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

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: namevalidator::NameValidator()
//-----------------------------------------------------------------------------
NameValidator::NameValidator(QObject* parent):
QRegularExpressionValidator(parent)
{
    QRegularExpression nameExpression(QStringLiteral("(^[a-zA-Z:_]+[a-zA-Z0-9:_.-]*$)|(^$)"));
    setRegularExpression(nameExpression);
}
