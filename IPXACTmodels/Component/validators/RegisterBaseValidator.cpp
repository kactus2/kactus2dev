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

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/AccessPolicy.h>
#include <IPXACTmodels/Component/Mode.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::RegisterBaseValidator()
//-----------------------------------------------------------------------------
RegisterBaseValidator::RegisterBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterValidator> parameterValidator,
    Document::Revision docRevision) :
expressionParser_(expressionParser),
parameterValidator_(parameterValidator),
docRevision_(docRevision),
memArrayValidator_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::validate()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::validate(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    if (docRevision_ == Document::Revision::Std14)
    {
        return hasValidName(selectedRegisterBase) &&
               hasValidIsPresent(selectedRegisterBase) &&
               hasValidDimensions(selectedRegisterBase) &&
               hasValidAddressOffset(selectedRegisterBase) &&
               hasValidParameters(selectedRegisterBase);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        return hasValidName(selectedRegisterBase) &&
            hasValidAddressOffset(selectedRegisterBase) &&
            hasValidMemoryArray(selectedRegisterBase) &&
            hasValidAccessPolicies(selectedRegisterBase) &&
            hasValidParameters(selectedRegisterBase);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::hasValidName()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::hasValidName(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    return CommonItemsValidator::hasValidName(selectedRegisterBase->name());
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
bool RegisterBaseValidator::hasValidDimensions(QSharedPointer<RegisterBase> selectedRegisterBase) const
{
    return memArrayValidator_.hasValidDimensions(selectedRegisterBase->getMemoryArray());
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
    [[maybe_unused]] quint64 offsetInt = solvedValue.toULongLong(&changeOk);

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
        for (QSharedPointer<Parameter> parameter : *selectedRegisterBase->getParameters())
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
// Function: RegisterBaseValidator::hasValidAccessPolicies()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::hasValidAccessPolicies(QSharedPointer<RegisterBase> registerBase) const
{
    return CommonItemsValidator::hasValidAccessPolicies(registerBase->getAccessPolicies(), componentModes_);
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::hasValidMemoryArray()
//-----------------------------------------------------------------------------
bool RegisterBaseValidator::hasValidMemoryArray(QSharedPointer<RegisterBase> registerBase) const
{
    if (!registerBase->getMemoryArray())
    {
        return true;
    }

    return memArrayValidator_.validate(registerBase->getMemoryArray());
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
            QObject::tr("Invalid name specified for %1").arg(context));
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
        errors.append(QObject::tr("Invalid isPresent set for %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsInDimension()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsInDimension(QVector<QString>& errors,
                                                  QSharedPointer<RegisterBase> selectedRegisterBase,
                                                  QString const& context) const
{
    if (!hasValidDimensions(selectedRegisterBase))
    {
        errors.append(QObject::tr("Invalid dimensions set for %1").arg(context));
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
        errors.append(QObject::tr("Invalid address offset set for %1").arg(context));
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
        for (QSharedPointer<Parameter> parameter : *selectedRegisterBase->getParameters())
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

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsInAccessPolicies()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsInAccessPolicies(QStringList& errors, QSharedPointer<RegisterBase> registerBase, QString const& context) const
{
    CommonItemsValidator::findErrorsInAccessPolicies(errors, registerBase->getAccessPolicies(), componentModes_, context);
}

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::findErrorsInMemoryArray()
//-----------------------------------------------------------------------------
void RegisterBaseValidator::findErrorsInMemoryArray(QStringList& errors, QSharedPointer<RegisterBase> registerBase,
    QString const& context) const
{
    if (auto const& memArray = registerBase->getMemoryArray())
    {
        memArrayValidator_.findErrorsIn(errors, memArray, context);
    }
}
