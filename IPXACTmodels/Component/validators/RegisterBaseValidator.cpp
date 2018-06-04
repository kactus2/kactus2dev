//-----------------------------------------------------------------------------
// File: RegisterBaseValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.11.2015
//
// Description:
// Validator for ipxact:Register.
//-----------------------------------------------------------------------------

#include "RegisterBaseValidator.h"
#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/common/Parameter.h>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::RegisterBaseValidator()
//-----------------------------------------------------------------------------
RegisterBaseValidator::RegisterBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
                                     QSharedPointer<ParameterValidator> parameterValidator):
expressionParser_(expressionParser),
parameterValidator_(parameterValidator)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::~RegisterBaseValidator()
//-----------------------------------------------------------------------------
RegisterBaseValidator::~RegisterBaseValidator()
{

}


//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::validate()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::validate(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    return hasValidName(selectedRegisterBase) &&
           hasValidIsPresent(selectedRegisterBase) &&
           hasValidDimension(selectedRegisterBase) &&
           hasValidAddressOffset(selectedRegisterBase) &&
           hasValidParameters(selectedRegisterBase);
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::hasValidName()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::hasValidName(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(selectedRegisterBase->name());

    if (selectedRegisterBase->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::hasValidIsPresent(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    if (!selectedRegisterBase->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(selectedRegisterBase->getIsPresent());

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);

        if (!toIntOk || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::hasValidDimension()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::hasValidDimension(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    if (!selectedRegisterBase->getDimension().isEmpty())
    {
        bool dimensionValid = false;
        QString solvedValue = expressionParser_->parseExpression(selectedRegisterBase->getDimension(), &dimensionValid);

        bool toIntOk = true;
        solvedValue.toULongLong(&toIntOk);

        return dimensionValid && toIntOk;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::hasValidAddressOffset()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::hasValidAddressOffset(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    bool changeOk = true;
    bool expressionValid = false;

    QString solvedValue =
        expressionParser_->parseExpression(selectedRegisterBase->getAddressOffset(), &expressionValid);
    quint64 offsetInt = solvedValue.toULongLong(&changeOk);

    return changeOk && expressionValid;
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::hasValidParameters(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    if (!selectedRegisterBase->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *selectedRegisterBase->getParameters())
        {
            if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
            {
                return false;
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsIn(QVector<QString>& errors,
                                         QSharedPointer<RegisterBase> selectedRegisterBase,
                                         QString const& context) const
{
    QString registerContext = QStringLiteral("register ") + selectedRegisterBase->name();

    findErrorsInName(errors, selectedRegisterBase, context);
    findErrorsInIsPresent(errors, selectedRegisterBase, context);
    findErrorsInDimension(errors, selectedRegisterBase, context);
    findErrorsInAddressOffset(errors, selectedRegisterBase, context);
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsInName(QVector<QString>& errors,
                                             QSharedPointer<RegisterBase> selectedRegisterBase,
                                             QString const& context) const
{
    if (!hasValidName(selectedRegisterBase))
    {
        errors.append(
            QObject::tr("Invalid name specified for register '%1' within %2")
            .arg(selectedRegisterBase->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsInIsPresent(QVector<QString>& errors,
                                                  QSharedPointer<RegisterBase> selectedRegisterBase,
                                                  QString const& context) const
{
    if (!hasValidIsPresent(selectedRegisterBase))
    {
        errors.append(QObject::tr("Invalid isPresent set for %1 within %2").arg(selectedRegisterBase->name()).
            arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsInDimension()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsInDimension(QVector<QString>& errors,
                                                  QSharedPointer<RegisterBase> selectedRegisterBase,
                                                  QString const& context) const
{
    if (!hasValidDimension(selectedRegisterBase))
    {
        errors.append(QObject::tr("Invalid dimension set for register %1 within %2").arg(selectedRegisterBase->name()).
            arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsInAddressOffset()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsInAddressOffset(QVector<QString>& errors,
                                                      QSharedPointer<RegisterBase> selectedRegisterBase,
                                                      QString const& context) const
{
    if (!hasValidAddressOffset(selectedRegisterBase))
    {
        errors.append(QObject::tr("Invalid address offset set for register %1 within %2")
            .arg(selectedRegisterBase->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsInParameters(QVector<QString>&errors,
                                                   QSharedPointer<RegisterBase> selectedRegisterBase,
                                                   QString const& context) const
{
    if (!selectedRegisterBase->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *selectedRegisterBase->getParameters())
        {
            if (parameterNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Name %1 of parameters in %2 is not unique.").arg(parameter->name(),
                    context));
            }
            else
            {
                parameterNames.append(parameter->name());
            }

            parameterValidator_->findErrorsIn(errors, parameter, context);
        }
    }
}
