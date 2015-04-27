//-----------------------------------------------------------------------------
// File: BinaryValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.03.2015
//
// Description:
// Validator class for binary type value.
// This validator can be used to validate user inputs where the input type is specified to be bit-type.
//-----------------------------------------------------------------------------

#include "BinaryValidator.h"

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: BinaryValidator::BinaryValidator()
//-----------------------------------------------------------------------------
BinaryValidator::BinaryValidator(QString sizeRestraint, QObject* parent):
QRegularExpressionValidator(parent)
{
    setNewExpressionvalidator(sizeRestraint);
}

//-----------------------------------------------------------------------------
// Function: BinaryValidator::~BinaryValidator()
//-----------------------------------------------------------------------------
BinaryValidator::~BinaryValidator()
{

}

//-----------------------------------------------------------------------------
// Function: BinaryValidator::setNewExpressionvalidator()
//-----------------------------------------------------------------------------
void BinaryValidator::setNewExpressionvalidator(QString const& sizeRestraint)
{
    QRegularExpression expression ("^([1-9]?[0-9]*'[bB][01_]{" + sizeRestraint + ",})?$");

    setRegularExpression(expression);
}