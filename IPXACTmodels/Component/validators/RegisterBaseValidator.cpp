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
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/AccessPolicy.h>
#include <IPXACTmodels/common/Parameter.h>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: RegisterBaseValidator::RegisterBaseValidator()
//-----------------------------------------------------------------------------
RegisterBaseValidator::RegisterBaseValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterValidator> parameterValidator,
    Document::Revision docRevision) :
expressionParser_(expressionParser),
parameterValidator_(parameterValidator),
docRevision_(docRevision)
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
               hasValidDimension(selectedRegisterBase) &&
               hasValidAddressOffset(selectedRegisterBase) &&
               hasValidParameters(selectedRegisterBase);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        return hasValidName(selectedRegisterBase) &&
            hasValidAddressOffset(selectedRegisterBase) &&
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
    bool hasAccessPolicyWithoutModeRef = false;

    QStringList checkedModeReferences;
    QStringList checkedModePriorities;

    for (auto const& accessPolicy : *registerBase->getAccessPolicies())
    {
        if (accessPolicy->getModeReferences()->isEmpty())
        {
            hasAccessPolicyWithoutModeRef = true;
        }

        // Check if the mode references of the access policy are valid. Also check for duplicate mode refs between
        // all register access policies.
        if (!CommonItemsValidator::hasValidModeRefs(accessPolicy->getModeReferences(),
            checkedModeReferences, checkedModePriorities))
        {
            return false;
        }
    }

    // Number of access policies cannot be greater than one if an access policy has no mode references.
    if (hasAccessPolicyWithoutModeRef && registerBase->getAccessPolicies()->size() > 1)
    {
        return false;
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

    if (docRevision_ == Document::Revision::Std14)
    {
        findErrorsInIsPresent(errors, selectedRegisterBase, context);
        findErrorsInDimension(errors, selectedRegisterBase, context);
    }

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
    if (!hasValidDimension(selectedRegisterBase))
    {
        errors.append(QObject::tr("Invalid dimension set for %1").arg(context));
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
    bool hasAccessPolicyWithoutModeRef = false;

    QString accessPolicyContext = QStringLiteral("access policies of ") + context;

    bool duplicateModeRefErrorIssued = false;
    bool duplicateModePriorityErrorIssued = false;

    QStringList checkedModeReferences;
    QStringList checkedModePriorities;

    for (auto const& accessPolicy : *registerBase->getAccessPolicies())
    {
        if (accessPolicy->getModeReferences()->isEmpty())
        {
            hasAccessPolicyWithoutModeRef = true;
        }

        // Check mode references in current access policy, and look for duplicate references.
        CommonItemsValidator::findErrorsInModeRefs(errors, accessPolicy->getModeReferences(), accessPolicyContext, 
            checkedModeReferences, checkedModePriorities, &duplicateModeRefErrorIssued, &duplicateModePriorityErrorIssued);
    }

    // Number of access policies cannot be greater than one if a field access policy has no mode references.
    if (hasAccessPolicyWithoutModeRef && registerBase->getAccessPolicies()->size() > 1)
    {
        errors.append(QObject::tr("In %1, multiple access policies are not allowed if one "
            "of them lacks a mode reference.").arg(registerBase->name()).arg(context));
    }
}
