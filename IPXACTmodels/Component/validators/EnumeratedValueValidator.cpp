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
    whiteSpaceExpression.setPattern("^\\s*$");
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
    QString value = enumeratedValue->getValue();
    QString solvedValue = expressionParser_->parseExpression(value);

    QRegularExpression bitExpression("^([0-9]+|[1-9]+[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$");
    return !value.isEmpty() || bitExpression.match(value).hasMatch() || bitExpression.match(solvedValue).hasMatch();
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
        errors.append(QObject::tr("Invalid name specified for %1 within %2").arg(enumeratedValue->name(), context));
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