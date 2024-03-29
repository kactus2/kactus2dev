//-----------------------------------------------------------------------------
// File: ChoiceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.12.2015
//
// Description:
// Validator for the ipxact:choice.
//-----------------------------------------------------------------------------

#include "ChoiceValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/common/Choice.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ChoiceValidator::ChoiceValidator()
//-----------------------------------------------------------------------------
ChoiceValidator::ChoiceValidator(QSharedPointer<ExpressionParser> expressionParser):
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: ChoiceValidator::validate()
//-----------------------------------------------------------------------------
bool ChoiceValidator::validate(QSharedPointer<Choice> choice) const
{
    return hasValidName(choice) && hasValidEnumerations(choice);
}

//-----------------------------------------------------------------------------
// Function: ChoiceValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ChoiceValidator::hasValidName(QSharedPointer<Choice> choice) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(choice->name());

    if (choice->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ChoiceValidator::hasValidEnumerations()
//-----------------------------------------------------------------------------
bool ChoiceValidator::hasValidEnumerations(QSharedPointer<Choice> choice) const
{
    if (!choice->enumerations()->isEmpty())
    {
        for (QSharedPointer<Enumeration> const& enumeration : *choice->enumerations())
        {
            bool isValidValue = false;
            expressionParser_->parseExpression(enumeration->getValue(), &isValidValue);
            if (enumeration->getValue().isEmpty() || isValidValue == false)
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ChoiceValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ChoiceValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Choice> choice, QString const& context)
    const
{
    findErrorsInName(errors, choice, context);
    findErrorsInEnumerations(errors, choice, context);
}

//-----------------------------------------------------------------------------
// Function: ChoiceValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void ChoiceValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<Choice> choice,
    QString const& context) const
{
    if (!hasValidName(choice))
    {
        errors.append(QObject::tr("Invalid name set for choice %1 within %2").arg(choice->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: ChoiceValidator::findErrorsInEnumerations()
//-----------------------------------------------------------------------------
void ChoiceValidator::findErrorsInEnumerations(QVector<QString>& errors, QSharedPointer<Choice> choice,
    QString const& context) const
{
    if (choice->enumerations()->isEmpty())
    {
        errors.append(QObject::tr("No enumerations found in choice '%1' within %2").arg(choice->name(), context));
    }
    else
    {
        for (QSharedPointer<Enumeration> const& enumeration : *choice->enumerations())
        {
            bool isValidValue = false;
            expressionParser_->parseExpression(enumeration->getValue(), &isValidValue);
            
            if (enumeration->getValue().isEmpty() || isValidValue == false)
            {
                errors.append(QObject::tr("Invalid value '%1' set for enumeration in choice '%2' within %3").arg(
                    enumeration->getValue(), choice->name(), context));
            }
        }
    }
}
