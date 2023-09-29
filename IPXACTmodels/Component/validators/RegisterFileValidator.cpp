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
#include <IPXACTmodels/Component/MemoryArray.h>
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
bool RegisterFileValidator::validate(QSharedPointer<RegisterFile> selectedRegisterFile, QString const& addressUnitBits, QString const& addressBlockWidth) const
{
    if (docRevision_ == Document::Revision::Std14)
    {
        return RegisterBaseValidator::validate(selectedRegisterFile) &&
            hasValidRange(selectedRegisterFile) &&
            hasValidRegisterData(selectedRegisterFile, addressUnitBits, addressBlockWidth);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        return RegisterBaseValidator::validate(selectedRegisterFile) &&
            hasValidRange(selectedRegisterFile) &&
            hasValidRegisterData(selectedRegisterFile, addressUnitBits, addressBlockWidth) &&
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
bool RegisterFileValidator::hasValidRegisterData(QSharedPointer<RegisterFile> selectedRegisterFile, QString const& addressUnitBits, QString const& addressBlockWidth) const
{
    if (selectedRegisterFile->getRegisterData()->isEmpty())
    {
        return true;
    }

    QSharedPointer<EnumeratedValueValidator> enumValidator(new EnumeratedValueValidator(expressionParser_));
    QSharedPointer<FieldValidator> fieldValidator(new FieldValidator(expressionParser_, enumValidator, parameterValidator_));

    RegisterValidator newRegisterValidator(expressionParser_, fieldValidator, parameterValidator_, docRevision_);

    MemoryReserve reservedArea;

    QStringList registerNames;
    QStringList registerFileNames;

    for (auto const& registerData : *selectedRegisterFile->getRegisterData())
    {
        if (auto asRegister = registerData.dynamicCast<Register>())
        {
            if (!childRegisterIsValid(asRegister, addressUnitBits, addressBlockWidth, 
                selectedRegisterFile->getRange(), reservedArea, registerNames))
            {
                return false;
            }
        }
        else if (auto asRegisterFile = registerData.dynamicCast<RegisterFile>())
        {
            if (!childRegisterFileIsValid(asRegisterFile, addressUnitBits, addressBlockWidth, 
                selectedRegisterFile->getRange(), reservedArea, registerFileNames))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return !reservedArea.hasOverlap();
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
    QString const& context, QString const& addressUnitBits, QString const& addressBlockWidth) const
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
    findErrorsInRegisterData(errors, selectedRegisterFile, completeContext, addressUnitBits, addressBlockWidth);
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
    QSharedPointer<RegisterFile> selectedRegisterFile, QString const& context, QString const& addressUnitBits,
    QString const& addressBlockWidth) const
{
    if (selectedRegisterFile->getRegisterData()->isEmpty())
    {
        return;
    }

    MemoryReserve reservedArea;

    QStringList registerNames;
    QStringList registerFileNames;
    QStringList duplicateRegisterNames;
    QStringList duplicateRegisterFileNames;

    for (auto const& registerData : *selectedRegisterFile->getRegisterData())
    {
        if (auto asRegister = registerData.dynamicCast<Register>())
        {
            findErrorsInChildRegister(errors, asRegister, context, addressUnitBits, 
                selectedRegisterFile->getRange(), addressBlockWidth, reservedArea, registerNames, duplicateRegisterNames);
        }
        else if (auto asRegisterFile = registerData.dynamicCast<RegisterFile>())
        {
            findErrorsInChildRegisterFile(errors, asRegisterFile, context, addressUnitBits, 
                selectedRegisterFile->getRange(), addressBlockWidth, reservedArea, registerFileNames, duplicateRegisterFileNames);
        }
    }

    reservedArea.findErrorsInOverlap(errors, QStringLiteral("Register data"), context);
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

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::getRegisterSizeInLAU()
//-----------------------------------------------------------------------------
qint64 RegisterFileValidator::getRegisterSizeInLAU(QSharedPointer<Register> targetRegister, qint64 addressUnitBits) const
{
    qint64 size = expressionParser_->parseExpression(targetRegister->getSize()).toLongLong();
    qint64 dimensionsProduct = 1;

    if (auto memArray = targetRegister->getMemoryArray())
    {
        for (auto const& dimension : *memArray->getDimensions())
        {
            dimensionsProduct *= expressionParser_->parseExpression(dimension->value_).toLongLong();
        }
    }

    qint64 topPart = size + addressUnitBits - 1;
    qint64 dimensionlessSize = topPart / addressUnitBits;

    qint64 trueSize = dimensionlessSize * dimensionsProduct;

    return trueSize;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::childRegisterIsValid()
//-----------------------------------------------------------------------------
bool RegisterFileValidator::childRegisterIsValid(QSharedPointer<Register> childRegister, 
    QString const& addressUnitBits, QString const& addressBlockWidth, QString const& registerFileRange, 
    MemoryReserve& reservedArea, QStringList& registerNames) const
{
    if (registerNames.contains(childRegister->name()) ||
        !registerValidator_->validate(childRegister))
    {
        return false;
    }
    else
    {
        registerNames.append(childRegister->name());
    }

    bool aubChangeOk = true;
    qint64 aubInt = expressionParser_->parseExpression(addressUnitBits).toLongLong(&aubChangeOk);
    qint64 registerFileRangeInt = expressionParser_->parseExpression(registerFileRange).toLongLong();

    if (aubChangeOk && aubInt != 0)
    {
        int registerSize = expressionParser_->parseExpression(childRegister->getSize()).toInt();
        int blockWidth = expressionParser_->parseExpression(addressBlockWidth).toInt();
        
        if (registerSize > blockWidth)
        {
            return false;
        }

        qint64 realRegisterSize = getRegisterSizeInLAU(childRegister, aubInt);

        qint64 registerBegin = expressionParser_->parseExpression(
            childRegister->getAddressOffset()).toLongLong();

        qint64 registerEnd = registerBegin + realRegisterSize - 1;

        if (registerBegin < 0 || registerBegin + realRegisterSize > registerFileRangeInt)
        {
            return false;
        }

        if (childRegister->getIsPresent().isEmpty() ||
            expressionParser_->parseExpression(childRegister->getIsPresent()).toInt())
        {
            reservedArea.addArea(childRegister->name(), registerBegin, registerEnd);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::childRegisterFileIsValid()
//-----------------------------------------------------------------------------
bool RegisterFileValidator::childRegisterFileIsValid(QSharedPointer<RegisterFile> childRegisterFile, 
    QString const& addressUnitBits, QString const& addressBlockWidth, QString const& registerFileRange, 
    MemoryReserve& reservedArea, QStringList& registerFileNames) const
{
    if (registerFileNames.contains(childRegisterFile->name()) ||
        !validate(childRegisterFile, addressUnitBits, addressBlockWidth))
    {
        return false;
    }
    else
    {
        registerFileNames.append(childRegisterFile->name());
    }

    qint64 registerFileBegin = expressionParser_->parseExpression(
        childRegisterFile->getAddressOffset()).toLongLong();

    qint64 registerFileRangeInt = expressionParser_->parseExpression(childRegisterFile->getRange()).toLongLong();

    qint64 parentRegisterFileRangeInt = expressionParser_->parseExpression(registerFileRange).toLongLong();

    qint64 registerFileEnd = registerFileBegin + registerFileRangeInt - 1;

    if (registerFileBegin < 0 || registerFileBegin + registerFileEnd > parentRegisterFileRangeInt)
    {
        return false;
    }

    if (childRegisterFile->getIsPresent().isEmpty() ||
        expressionParser_->parseExpression(childRegisterFile->getIsPresent()).toInt())
    {
        reservedArea.addArea(childRegisterFile->name(), registerFileBegin, registerFileEnd);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::findErrorsInChildRegister()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsInChildRegister(QStringList& errors, QSharedPointer<Register> childRegister,
    QString const& context, QString const& addressUnitBits, QString const& parentRegisterFileRange, 
    QString const& addressBlockWidth, MemoryReserve& reservedArea, QStringList& registerNames, 
    QStringList& duplicateRegisterNames) const
{
    QString const& childRegName = childRegister->name();

    if (registerNames.contains(childRegName) && !duplicateRegisterNames.contains(childRegName))
    {
        errors.append(QObject::tr("Name %1 of registers in %2 is not unique.").arg(childRegName).arg(context));
        duplicateRegisterNames.append(childRegName);
    }
    else
    {
        registerNames.append(childRegName);
    }

    registerValidator_->findErrorsIn(errors, childRegister, context);

    bool aubChangeOk = true;
    qint64 aubInt = expressionParser_->parseExpression(addressUnitBits).toLongLong(&aubChangeOk);
    qint64 registerFileRangeInt = expressionParser_->parseExpression(parentRegisterFileRange).toLongLong();

    if (aubChangeOk && aubInt != 0)
    {
        qint64 registerSize = expressionParser_->parseExpression(childRegister->getSize()).toInt();
        qint64 blockWidth = expressionParser_->parseExpression(addressBlockWidth).toInt();

        if (registerSize > blockWidth)
        {
            errors.append(QObject::tr("Register %1 size must not be greater than the containing "
                "addressBlock width.").arg(childRegName));
        }

        qint64 realRegisterSize = getRegisterSizeInLAU(childRegister, aubInt);

        qint64 registerBegin = expressionParser_->parseExpression(
            childRegister->getAddressOffset()).toLongLong();

        qint64 registerEnd = registerBegin + realRegisterSize - 1;

        if (registerBegin < 0 || registerBegin + realRegisterSize > registerFileRangeInt)
        {
            errors.append(QObject::tr("Register %1 is not contained within parent register file.")
                .arg(childRegName));
        }

        if (childRegister->getIsPresent().isEmpty() ||
            expressionParser_->parseExpression(childRegister->getIsPresent()).toInt())
        {
            reservedArea.addArea(childRegName, registerBegin, registerEnd);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::findErrorsInChildRegisterFile()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsInChildRegisterFile(QStringList& errors, 
    QSharedPointer<RegisterFile> childRegisterFile, QString const& context, QString const& addressUnitBits, 
    QString const& parentRegisterFileRange, QString const& addressBlockWidth, MemoryReserve& reservedArea, 
    QStringList& registerFileNames, QStringList& duplicateRegisterFileNames) const
{
    QString const& childRegFileName = childRegisterFile->name();

    if (registerFileNames.contains(childRegFileName) && !duplicateRegisterFileNames.contains(childRegFileName))
    {
        errors.append(QObject::tr("Name %1 of register files in %2 is not unique.").arg(childRegFileName).arg(context));
        duplicateRegisterFileNames.append(childRegFileName);
    }
    else
    {
        registerFileNames.append(childRegFileName);
    }

    findErrorsIn(errors, childRegisterFile, context, addressUnitBits, addressBlockWidth);

    qint64 registerFileBegin = expressionParser_->parseExpression(
        childRegisterFile->getAddressOffset()).toLongLong();

    qint64 registerFileRangeInt = expressionParser_->parseExpression(childRegisterFile->getRange()).toLongLong();
    qint64 parentRegisterFileRangeInt = expressionParser_->parseExpression(parentRegisterFileRange).toLongLong();
    qint64 registerFileEnd = registerFileBegin + registerFileRangeInt - 1;

    if (registerFileBegin < 0 || registerFileBegin + registerFileEnd > parentRegisterFileRangeInt)
    {
        errors.append(QObject::tr("Register file %1 not contained within %2.").arg(childRegisterFile->name()).arg(context));
    }

    if (childRegisterFile->getIsPresent().isEmpty() ||
        expressionParser_->parseExpression(childRegisterFile->getIsPresent()).toInt())
    {
        reservedArea.addArea(childRegisterFile->name(), registerFileBegin, registerFileEnd);
    }
}
