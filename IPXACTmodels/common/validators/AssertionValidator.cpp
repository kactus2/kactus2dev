//-----------------------------------------------------------------------------
// File: AssertionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.01.2016
//
// Description:
// Validator for the ipxact:assertion.
//-----------------------------------------------------------------------------

#include "AssertionValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/common/Assertion.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: AssertionValidator::AssertionValidator()
//-----------------------------------------------------------------------------
AssertionValidator::AssertionValidator(QSharedPointer<ExpressionParser> expressionParser):
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: AssertionValidator::~AssertionValidator()
//-----------------------------------------------------------------------------
AssertionValidator::~AssertionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: AssertionValidator::validate()
//-----------------------------------------------------------------------------
bool AssertionValidator::validate(QSharedPointer<Assertion> assertion) const
{
    return hasValidName(assertion) && hasValidAssert(assertion);
}

//-----------------------------------------------------------------------------
// Function: AssertionValidator::hasValidName()
//-----------------------------------------------------------------------------
bool AssertionValidator::hasValidName(QSharedPointer<Assertion> assertion) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(assertion->name());

    if (assertion->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AssertionValidator::hasValidAssert()
//-----------------------------------------------------------------------------
bool AssertionValidator::hasValidAssert(QSharedPointer<Assertion> assertion) const
{
    if (!assertion->getAssert().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(assertion->getAssert());

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);

        return toIntOk && intValue == 1;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AssertionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AssertionValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Assertion> assertion,
    QString const& context) const
{
    findErrorsInName(errors, assertion, context);
    findErrorsInAssert(errors, assertion, context);
}

//-----------------------------------------------------------------------------
// Function: AssertionValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void AssertionValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<Assertion> assertion,
    QString const& context) const
{
    if (!hasValidName(assertion))
    {
        errors.append(QObject::tr("Invalid name set for assertion %1 within %2").
            arg(assertion->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AssertionValidator::findErrorsInAssert()
//-----------------------------------------------------------------------------
void AssertionValidator::findErrorsInAssert(QVector<QString>& errors, QSharedPointer<Assertion> assertion,
    QString const& context) const
{
    if (!hasValidAssert(assertion))
    {
        errors.append(QObject::tr("Invalid assert set for assertion %1 within %2").
            arg(assertion->name()).arg(context));
    }
}