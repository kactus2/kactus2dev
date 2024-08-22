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
#include <IPXACTmodels/Component/Component.h>
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
void RegisterFileValidator::componentChange(QSharedPointer<Component> newComponent)
{
    if (newComponent)
    {
        docRevision_ = newComponent->getRevision();
        componentModes_ = newComponent->getModes();
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::validate()
//-----------------------------------------------------------------------------
bool RegisterFileValidator::validate(QSharedPointer<RegisterFile> selectedRegisterFile, QString const& addressUnitBits, QString const& addressBlockWidth)
{
    if (docRevision_ == Document::Revision::Std14)
    {
        return hasValidRange(selectedRegisterFile) &&
            hasValidRegisterData(selectedRegisterFile, addressUnitBits, addressBlockWidth) &&
            RegisterBaseValidator::validate(selectedRegisterFile);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        return hasValidRange(selectedRegisterFile) &&
            hasValidRegisterData(selectedRegisterFile, addressUnitBits, addressBlockWidth) &&
            hasValidAccessPolicies(selectedRegisterFile) &&
            RegisterBaseValidator::validate(selectedRegisterFile) &&
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
bool RegisterFileValidator::hasValidRegisterData(QSharedPointer<RegisterFile> selectedRegisterFile, QString const& addressUnitBits, QString const& addressBlockWidth)
{
    if (selectedRegisterFile->getRegisterData()->isEmpty())
    {
        return true;
    }

    QMultiHash<QString, QSharedPointer<RegisterBase> > foundNames;

    bool aubChangeOk = true;
    qint64 aubInt = expressionParser_->parseExpression(addressUnitBits).toLongLong(&aubChangeOk);
    qint64 registerFileRangeInt = getTrueRegisterFileRange(selectedRegisterFile);

    auto registerDataCopy = QSharedPointer<QList<QSharedPointer<RegisterBase> > >(new QList(*selectedRegisterFile->getRegisterData()));

    bool registerDataIsValid = true;

    // Sort registers and register files by address offset
    auto sortByAddressOffset = [&](QSharedPointer<RegisterBase> registerBaseA, QSharedPointer<RegisterBase> registerBaseB)
        {
            return expressionParser_->parseExpression(registerBaseA->getAddressOffset()).toLongLong() <
                expressionParser_->parseExpression(registerBaseB->getAddressOffset()).toLongLong();
        };

    std::sort(registerDataCopy->begin(), registerDataCopy->end(), sortByAddressOffset);

    qint64 lastRangeEnd = -1;

    bool previousWasRegister = false;

    for (auto regIter = registerDataCopy->begin(); regIter != registerDataCopy->end(); ++regIter)
    {
        if (auto asRegister = regIter->dynamicCast<Register>())
        {
            if (!foundNames.contains(asRegister->name()))
            {
                registerValidator_->setChildItemValidity(asRegister, true);
            }

            foundNames.insert(asRegister->name(), asRegister);

            qint64 registerSize = getRegisterSizeInLAU(asRegister, aubInt);
            qint64 blockWidth = expressionParser_->parseExpression(addressBlockWidth).toLongLong();

            if (registerSize > blockWidth)
            {
                registerValidator_->setChildItemValidity(asRegister, false);
                registerDataIsValid = false;
            }

            if (aubChangeOk && aubInt != 0 &&
                markRegisterOverlap(regIter, lastRangeEnd, registerFileRangeInt, aubInt, true, previousWasRegister))
            {
                registerDataIsValid = false;
            }

            previousWasRegister = true;
        }
        else if (auto asRegisterFile = regIter->dynamicCast<RegisterFile>())
        {
            if (!foundNames.contains(asRegisterFile->name()))
            {
                setChildItemValidity(asRegisterFile, true);
            }

            foundNames.insert(asRegisterFile->name(), asRegisterFile);

            if (aubChangeOk && aubInt != 0 &&
                markRegisterOverlap(regIter, lastRangeEnd, registerFileRangeInt, aubInt, false, previousWasRegister))
            {
                registerDataIsValid = false;
            }

            previousWasRegister = false;
        }
    }

    // Mark registers and register files with duplicate names as invalid.
    for (auto const& name : foundNames.keys())
    {
        if (auto const& duplicateNames = foundNames.values(name);
            duplicateNames.count() > 1)
        {
            std::for_each(duplicateNames.begin(), duplicateNames.end(),
                [this](QSharedPointer<RegisterBase> registerBase)
                {
                    if (registerBase.dynamicCast<Register>())
                    {
                        registerValidator_->setChildItemValidity(registerBase, false);
                    }
                    else if (registerBase.dynamicCast<RegisterFile>())
                    {
                        setChildItemValidity(registerBase, false);
                    }
                });

            registerDataIsValid = false;
        }
    }

    if (!registerDataIsValid)
    {
        return false;
    }

    // Validate registers and register files separately
    for (auto const& registerBase : *registerDataCopy)
    {
        if (QSharedPointer<Register> asRegister = registerBase.dynamicCast<Register>();
            asRegister && !registerValidator_->validate(asRegister))
        {
            return false;
        }
        else if (QSharedPointer<RegisterFile> asRegisterFile = registerBase.dynamicCast<RegisterFile>();
            asRegisterFile && !validate(asRegisterFile, addressUnitBits, addressBlockWidth))
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

    QMultiHash<QString, QSharedPointer<RegisterBase> > foundNames;

    bool aubChangeOk = true;
    qint64 aubInt = expressionParser_->parseExpression(addressUnitBits).toLongLong(&aubChangeOk);
    qint64 registerFileRangeInt = getTrueRegisterFileRange(selectedRegisterFile);
    qint64 addressBlockWidthInt = expressionParser_->parseExpression(addressBlockWidth).toLongLong();

    auto registerData = selectedRegisterFile->getRegisterData();

    if (aubChangeOk && aubInt != 0)
    {
        for (auto const& reg : *registerData)
        {
            if (auto asRegister = reg.dynamicCast<Register>())
            {
                findErrorsInChildRegister(errors, asRegister, context, aubInt, 
                    registerFileRangeInt, addressBlockWidthInt, reservedArea, registerNames, duplicateRegisterNames);
            }
            else if (auto asRegisterFile = reg.dynamicCast<RegisterFile>())
            {
                findErrorsInChildRegisterFile(errors, asRegisterFile, context, addressUnitBits, 
                    registerFileRangeInt, addressBlockWidth, reservedArea, registerFileNames, duplicateRegisterFileNames);
            }
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
    QString registerFileContext = QStringLiteral("register file ") + selectedRegisterFile->name() + QStringLiteral(" within ") + context;

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
// Function: RegisterFileValidator::findErrorsInChildRegister()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsInChildRegister(QStringList& errors, QSharedPointer<Register> childRegister,
    QString const& context, qint64 addressUnitBits, qint64 parentRegisterFileRange, 
    qint64 addressBlockWidth, MemoryReserve& reservedArea, QStringList& registerNames, 
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

    qint64 registerSize = expressionParser_->parseExpression(childRegister->getSize()).toInt();

    if (registerSize > addressBlockWidth)
    {
        errors.append(QObject::tr("Register %1 size must not be greater than the containing "
            "addressBlock width.").arg(childRegName));
    }

    qint64 realRegisterSize = getRegisterSizeInLAU(childRegister, addressUnitBits);

    qint64 registerBegin = expressionParser_->parseExpression(
        childRegister->getAddressOffset()).toLongLong();

    qint64 registerEnd = registerBegin + realRegisterSize - 1;

    if (registerBegin < 0 || registerBegin + realRegisterSize > parentRegisterFileRange)
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

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::findErrorsInChildRegisterFile()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsInChildRegisterFile(QStringList& errors, 
    QSharedPointer<RegisterFile> childRegisterFile, QString const& context, QString const& addressUnitBits, 
    qint64 parentRegisterFileRange, QString const& addressBlockWidth, MemoryReserve& reservedArea, 
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
    qint64 registerFileEnd = registerFileBegin + registerFileRangeInt - 1;

    if (registerFileBegin < 0 || registerFileBegin + registerFileEnd > parentRegisterFileRange)
    {
        errors.append(QObject::tr("Register file %1 not contained within %2.").arg(childRegisterFile->name()).arg(context));
    }

    if (childRegisterFile->getIsPresent().isEmpty() ||
        expressionParser_->parseExpression(childRegisterFile->getIsPresent()).toInt())
    {
        reservedArea.addArea(childRegisterFile->name(), registerFileBegin, registerFileEnd);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::markRegisterOverlap()
//-----------------------------------------------------------------------------
bool RegisterFileValidator::markRegisterOverlap(QList<QSharedPointer<RegisterBase> >::iterator regIter, 
    qint64& lastEndAddress, qint64 registerFileRange, qint64 addressUnitBits, bool targetIsRegister, 
    bool lastWasRegister)
{
    auto targetRegisterBase = *regIter;

    qint64 targetRegisterBaseBegin = expressionParser_->parseExpression(
        targetRegisterBase->getAddressOffset()).toLongLong();
    qint64 targetRegisterBaseRangeInt = -1;

    bool errorFound = false;

    if (targetIsRegister)
    {
        targetRegisterBaseRangeInt = getRegisterSizeInLAU(targetRegisterBase.staticCast<Register>(), addressUnitBits);
    }
    else
    {
        targetRegisterBaseRangeInt = getTrueRegisterFileRange(targetRegisterBase.staticCast<RegisterFile>());
    }

    qint64 registerBaseEnd = targetRegisterBaseBegin + targetRegisterBaseRangeInt - 1;

    if (targetRegisterBase->getIsPresent().isEmpty() ||
        expressionParser_->parseExpression(targetRegisterBase->getIsPresent()).toInt())
    {
        if (targetRegisterBaseBegin <= lastEndAddress && lastEndAddress != -1)
        {
            auto prevRegisterBase = *std::prev(regIter);

            if (lastWasRegister)
            {
                registerValidator_->setChildItemValidity(prevRegisterBase, false);
            }
            else
            {
                setChildItemValidity(prevRegisterBase, false);
            }

            if (targetIsRegister)
            {
                registerValidator_->setChildItemValidity(targetRegisterBase, false);
            }
            else
            {
                setChildItemValidity(targetRegisterBase, false);
            }

            errorFound = true;
        }

        if (registerBaseEnd > lastEndAddress)
        {
            lastEndAddress = registerBaseEnd;
        }
    }

    if (targetRegisterBaseBegin < 0 || targetRegisterBaseBegin + targetRegisterBaseRangeInt > registerFileRange)
    {
        targetIsRegister
            ? registerValidator_->setChildItemValidity(targetRegisterBase, false)
            : setChildItemValidity(targetRegisterBase, false);
        errorFound = true;
    }

    return errorFound;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::getTrueRegisterFileRange()
//-----------------------------------------------------------------------------
qint64 RegisterFileValidator::getTrueRegisterFileRange(QSharedPointer<RegisterFile> targetRegisterFile) const
{
    qint64 dimensionsProduct = 1;

    if (auto memArray = targetRegisterFile->getMemoryArray())
    {
        for (auto const& dimension : *memArray->getDimensions())
        {
            dimensionsProduct *= expressionParser_->parseExpression(dimension->value_).toLongLong();
        }
    }

    qint64 dimensionlessRange = expressionParser_->parseExpression(targetRegisterFile->getRange()).toLongLong();
    qint64 trueRange = dimensionlessRange * dimensionsProduct;

    return trueRange;
}
