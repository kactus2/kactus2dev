//-----------------------------------------------------------------------------
// File: RegisterFileValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Dan Chianucci
// Date: 04.06.2018
//
// Description:
// Validator for ipxact:RegisterFile.
//-----------------------------------------------------------------------------

#include "RegisterFileValidator.h"
#include <KactusAPI/include/ExpressionParser.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/MemoryArrayValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Register.h>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::RegisterFileValidator()
//-----------------------------------------------------------------------------
RegisterFileValidator::RegisterFileValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterValidator> registerValidator,
    QSharedPointer<ParameterValidator> parameterValidator,
    Document::Revision docRevision) :
    RegisterBaseValidator(expressionParser, parameterValidator, docRevision),
    registerValidator_(registerValidator)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::getRegisterValidator()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterValidator> RegisterFileValidator::getRegisterValidator() const
{
    return registerValidator_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::componentChange()
//-----------------------------------------------------------------------------
void RegisterFileValidator::componentChange(Document::Revision newDocRevision)
{
    docRevision_ = newDocRevision;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::validate()
//-----------------------------------------------------------------------------
bool RegisterFileValidator::validate(QSharedPointer<RegisterFile> selectedRegisterFile) const
{
    if (docRevision_ == Document::Revision::Std14)
    {
        return RegisterBaseValidator::validate(selectedRegisterFile) &&
            hasValidRange(selectedRegisterFile) &&
            hasValidRegisterData(selectedRegisterFile);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        return RegisterBaseValidator::validate(selectedRegisterFile) &&
            hasValidRange(selectedRegisterFile) &&
            hasValidRegisterData(selectedRegisterFile) &&
            hasValidAccessPolicies(selectedRegisterFile) &&
            hasValidStructure(selectedRegisterFile);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::hasValidSize()
//-----------------------------------------------------------------------------
bool RegisterFileValidator::hasValidRange(QSharedPointer<RegisterFile> selectedRegisterFile) const
{
    bool toIntOk = true;
    quint64 range = expressionParser_->parseExpression(selectedRegisterFile->getRange()).toULongLong(&toIntOk);

    return toIntOk && range > 0;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::hasValidRegisterData()
//-----------------------------------------------------------------------------
bool RegisterFileValidator::hasValidRegisterData(QSharedPointer<RegisterFile> selectedRegisterFile) const
{
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(expressionParser_));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(expressionParser_, enumValidator, parameterValidator_));

    RegisterValidator newRegisterValidator(expressionParser_, fieldValidator, parameterValidator_, docRevision_);

    for (auto const& registerData : *selectedRegisterFile->getRegisterData())
    {
        if (auto asRegister = registerData.dynamicCast<Register>())
        {
            if (!newRegisterValidator.validate(asRegister))
            {
                return false;
            }
        }
        else if (auto asRegisterFile = registerData.dynamicCast<RegisterFile>())
        {
            if (!validate(asRegisterFile))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::hasValidStructure()
//-----------------------------------------------------------------------------
bool RegisterFileValidator::hasValidStructure(QSharedPointer<RegisterFile> selectedRegisterFile) const
{
    if (!selectedRegisterFile->getRegisterFileDefinitionReference().isEmpty() &&
        (!selectedRegisterFile->getTypeIdentifier().isEmpty() ||
            !selectedRegisterFile->getRange().isEmpty() ||
            !selectedRegisterFile->getAccessPolicies()->isEmpty() ||
            !selectedRegisterFile->getRegisterData()->isEmpty()))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<RegisterFile> selectedRegisterFile,
    QString const& context) const
{
    QString registerContext = QStringLiteral("register file ") + selectedRegisterFile->name();

    QString completeContext = QObject::tr("register file '%1' within %2").arg(selectedRegisterFile->name()).arg(context);

    findErrorsInName(errors, selectedRegisterFile, completeContext);

    if (docRevision_ == Document::Revision::Std14)
    {
        findErrorsInIsPresent(errors, selectedRegisterFile, completeContext);
        findErrorsInDimension(errors, selectedRegisterFile, completeContext);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        findErrorsInMemoryArray(errors, selectedRegisterFile, completeContext);
        findErrorsInAccessPolicies(errors, selectedRegisterFile, context);

        if (!hasValidStructure(selectedRegisterFile))
        {
            errors.append(QObject::tr("Register file '%1' in %2 contains both a register file definition reference and register file definition values.").arg(selectedRegisterFile->name()).arg(context));
        }
    }

    findErrorsInAddressOffset(errors, selectedRegisterFile, completeContext);
    findErrorsInParameters(errors, selectedRegisterFile, registerContext);
    findErrorsInRange(errors, selectedRegisterFile, context);
    findErrorsInRegisterData(errors, selectedRegisterFile, completeContext);
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::findErrorsInSize()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsInRange(QVector<QString>& errors,
    QSharedPointer<RegisterFile> selectedRegisterFile,
    QString const& context) const
{
    if (!hasValidRange(selectedRegisterFile))
    {
        errors.append(QObject::tr("Invalid range specified for register file %1 within %2").
            arg(selectedRegisterFile->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::findErrorsInRegisterData()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsInRegisterData(QVector<QString>& errors,
    QSharedPointer<RegisterFile> selectedRegisterFile,
    QString const& context) const
{
    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(expressionParser_));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(expressionParser_, enumValidator, parameterValidator_));

    RegisterValidator newRegisterValidator(expressionParser_, fieldValidator, parameterValidator_, docRevision_);

    for (auto const& registerData : *selectedRegisterFile->getRegisterData())
    {
        if (auto asRegister = registerData.dynamicCast<Register>())
        {
            newRegisterValidator.findErrorsIn(errors, asRegister, context);
        }
        else if (auto asRegisterFile = registerData.dynamicCast<RegisterFile>())
        {
            findErrorsIn(errors, asRegisterFile, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::findErrorsInAccessPolicies()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsInAccessPolicies(QStringList& errors, 
    QSharedPointer<RegisterFile> selectedRegisterFile, QString const& context) const
{
    QString registerFileContext = QStringLiteral("register ") + selectedRegisterFile->name() + QStringLiteral(" within ") + context;

    RegisterBaseValidator::findErrorsInAccessPolicies(errors, selectedRegisterFile, registerFileContext);
}
