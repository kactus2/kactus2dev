//-----------------------------------------------------------------------------
// File: EnumeratedValueValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.11.2015
//
// Description:
// Validator for ipxact:EnumeratedValue.
//-----------------------------------------------------------------------------

#include "EnumeratedValueValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/EnumeratedValue.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::EnumeratedValueValidator()
//-----------------------------------------------------------------------------
EnumeratedValueValidator::EnumeratedValueValidator(QSharedPointer<ExpressionParser> expressionParser):
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::~EnumeratedValueValidator()
//-----------------------------------------------------------------------------
EnumeratedValueValidator::~EnumeratedValueValidator()
{

}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::validate()
//-----------------------------------------------------------------------------
bool EnumeratedValueValidator::validate(QSharedPointer<EnumeratedValue> enumeratedValue) const
{
    return hasValidName(enumeratedValue) && hasValidValue(enumeratedValue);
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool EnumeratedValueValidator::hasValidName(QSharedPointer<EnumeratedValue> enumeratedValue) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^ *$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(enumeratedValue->name());

    if (enumeratedValue->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidValue()
//-----------------------------------------------------------------------------
bool EnumeratedValueValidator::hasValidValue(QSharedPointer<EnumeratedValue> enumeratedValue) const
{
    return !enumeratedValue->getValue().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void EnumeratedValueValidator::findErrorsIn(QVector<QString>& errors,
    QSharedPointer<EnumeratedValue> enumeratedValue, QString const& context) const
{
    findErrorsInName(errors, enumeratedValue, context);
    findErrorsInValue(errors, enumeratedValue, context);
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void EnumeratedValueValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<EnumeratedValue> enumeratedValue, QString const& context) const
{
    if (!hasValidName(enumeratedValue))
    {
        errors.append(QObject::tr("No name specified for %1 within %2").arg(enumeratedValue->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::findErrorsInValue()
//-----------------------------------------------------------------------------
void EnumeratedValueValidator::findErrorsInValue(QVector<QString>& errors,
    QSharedPointer<EnumeratedValue> enumeratedValue, QString const& context) const
{
    if (!hasValidValue(enumeratedValue))
    {
        errors.append(QObject::tr("Invalid value %1 set for enumerated value %2 within %3").
            arg(enumeratedValue->getValue()).arg(enumeratedValue->name()).arg(context));
    }
}