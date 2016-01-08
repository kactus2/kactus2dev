//-----------------------------------------------------------------------------
// File: ComponentGeneratorValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.01.2016
//
// Description:
// Validator for the component generator.
//-----------------------------------------------------------------------------

#include "ComponentGeneratorValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/ComponentGenerator.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::ComponentGeneratorValidator()
//-----------------------------------------------------------------------------
ComponentGeneratorValidator::ComponentGeneratorValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterValidator2014> parameterValidator):
expressionParser_(expressionParser),
parameterValidator_(parameterValidator)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::~ComponentGeneratorValidator()
//-----------------------------------------------------------------------------
ComponentGeneratorValidator::~ComponentGeneratorValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::validate()
//-----------------------------------------------------------------------------
bool ComponentGeneratorValidator::validate(QSharedPointer<ComponentGenerator> generator) const
{
    return hasValidName(generator) && hasValidPhase(generator) && hasValidGeneratorExe(generator);
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::hasValidName()
//-----------------------------------------------------------------------------
bool ComponentGeneratorValidator::hasValidName(QSharedPointer<ComponentGenerator> generator) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(generator->name());

    if (generator->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::hasValidPhase()
//-----------------------------------------------------------------------------
bool ComponentGeneratorValidator::hasValidPhase(QSharedPointer<ComponentGenerator> generator) const
{
    if (!generator->getPhase().isEmpty())
    {
        bool valueToRealOk = true;
        expressionParser_->parseExpression(generator->getPhase()).toDouble(&valueToRealOk);
        
        return valueToRealOk;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::hasValidGeneratorExe()
//-----------------------------------------------------------------------------
bool ComponentGeneratorValidator::hasValidGeneratorExe(QSharedPointer<ComponentGenerator> generator) const
{
    return !generator->getGeneratorExe().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ComponentGeneratorValidator::findErrorsIn(QVector<QString>& errors,
    QSharedPointer<ComponentGenerator> generator, QString const& context) const
{
    findErrorsInName(errors, generator, context);
    findErrorsInPhase(errors, generator, context);
    findErrorsInGeneratorExe(errors, generator, context);
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void ComponentGeneratorValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<ComponentGenerator> generator, QString const& context) const
{
    if (!hasValidName(generator))
    {
        errors.append(QObject::tr("Invalid name specified for component generator %1 within %2").
            arg(generator->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::findErrorsInPhase()
//-----------------------------------------------------------------------------
void ComponentGeneratorValidator::findErrorsInPhase(QVector<QString>& errors,
    QSharedPointer<ComponentGenerator> generator, QString const& context) const
{
    if (!hasValidPhase(generator))
    {
        errors.append(QObject::tr("Invalid phase value set for component generator '%1' within %2")
            .arg(generator->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentGeneratorValidator::findErrorsInGeneratorExe()
//-----------------------------------------------------------------------------
void ComponentGeneratorValidator::findErrorsInGeneratorExe(QVector<QString>& errors,
    QSharedPointer<ComponentGenerator> generator, QString const& context) const
{
    if (!hasValidGeneratorExe(generator))
    {
        errors.append(QObject::tr("Invalid generator exe set for component generator '%1' within %2")
            .arg(generator->name()).arg(context));
    }
}