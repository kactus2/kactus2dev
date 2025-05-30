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

    QString correctedAUB = addressUnitBits;

    // Avoid divisions by 0, default AUB to 8 bits
    if (addressUnitBits.isEmpty())
    {
        correctedAUB = QStringLiteral("8");
    }

    bool aubChangeOk = true;
    quint64 aubInt = expressionParser_->parseExpression(correctedAUB).toULongLong(&aubChangeOk);
    quint64 registerFileRange = expressionParser_->parseExpression(selectedRegisterFile->getRange()).toULongLong();

    if (!aubChangeOk || aubInt == 0)
    {
        return false;
    }
    
    auto registerData = selectedRegisterFile->getRegisterData();
    bool registerDataIsValid = true;

    QHash<QString, QSharedPointer<RegisterBase> > foundNames;
    QSet<QString> erroneousAreas;

    // Create memory areas for child registers and register files
    MemoryReserve memReserve;
    setupMemoryAreas(selectedRegisterFile, memReserve, aubInt);

    memReserve.checkOverlapAndContainment(erroneousAreas, registerFileRange);

    auto markItemInvalid = [this](QSharedPointer<RegisterBase> regBase)
        {
            if (auto asReg = regBase.dynamicCast<Register>())
            {
                registerValidator_->setChildItemValidity(asReg, false);
            }
            else if (auto asRegFile = regBase.dynamicCast<RegisterFile>())
            {
                setChildItemValidity(asRegFile, false);
            }
        };


    // Mark erroneous registers and register files
    for (auto const& regBase : *registerData)
    {
        // If duplicate name, mark current and first found reg with this name as invalid
        if (auto found = foundNames[regBase->name()])
        {
            markItemInvalid(regBase);
            markItemInvalid(found);

            registerDataIsValid = false;
        }
        else
        {
            foundNames.insert(regBase->name(), regBase);
        }

        // mark invalid, if register (file) overlaps or is not contained within parent reg file
        if (erroneousAreas.contains(regBase->name()))
        {
            markItemInvalid(regBase);
            registerDataIsValid = false;
        }
    }

    if (!registerDataIsValid)
    {
        return false;
    }

    // Validate registers and register files separately
    for (auto const& registerBase : *registerData)
    {
        if (QSharedPointer<Register> asRegister = registerBase.dynamicCast<Register>(); 
            asRegister && !registerValidator_->validate(asRegister))
        {
            return false;   
        }
        else if (QSharedPointer<RegisterFile> asRegisterFile = registerBase.dynamicCast<RegisterFile>();
            asRegisterFile && !validate(asRegisterFile, correctedAUB, addressBlockWidth))
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
    QString const& context, QString const& addressUnitBits, QString const& addressBlockWidth)
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
    QString const& addressBlockWidth)
{
    if (selectedRegisterFile->getRegisterData()->isEmpty())
    {
        return;
    }

    QStringList registerNames;
    QStringList registerFileNames;
    QStringList duplicateRegisterNames;
    QStringList duplicateRegisterFileNames;

    QString correctedAUB = addressUnitBits;

    // Avoid divisions by 0, default AUB to 8 bits
    if (addressUnitBits.isEmpty())
    {
        correctedAUB = QStringLiteral("8");
    }

    QMultiHash<QString, QSharedPointer<RegisterBase> > foundNames;

    bool aubChangeOk = true;
    quint64 aubInt = expressionParser_->parseExpression(correctedAUB).toLongLong(&aubChangeOk);
    
    quint64 registerFileRange = expressionParser_->parseExpression(selectedRegisterFile->getRange()).toULongLong();
    registerFileRange = getTrueRegisterFileRange(selectedRegisterFile, registerFileRange);
    
    quint64 addressBlockWidthInt = expressionParser_->parseExpression(addressBlockWidth).toLongLong();

    auto registerData = selectedRegisterFile->getRegisterData();

    // Create memory areas of child register data for overlap comparison
    MemoryReserve reservedArea;
    setupMemoryAreas(selectedRegisterFile, reservedArea, aubInt);

    for (auto const& reg : *registerData)
    {
        if (auto asRegister = reg.dynamicCast<Register>())
        {
            findErrorsInChildRegister(errors, asRegister, context, aubInt, 
                registerFileRange, addressBlockWidthInt, reservedArea, registerNames, duplicateRegisterNames);
        }
        else if (auto asRegisterFile = reg.dynamicCast<RegisterFile>())
        {
            findErrorsInChildRegisterFile(errors, asRegisterFile, context, correctedAUB, 
                registerFileRange, addressBlockWidth, reservedArea, registerFileNames, duplicateRegisterFileNames);
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
quint64 RegisterFileValidator::getRegisterSizeInLAU(QSharedPointer<Register> targetRegister, quint64 addressUnitBits) const
{
    quint64 size = expressionParser_->parseExpression(targetRegister->getSize()).toULongLong();

    // Round register size up to closest multiple of AUB to get size in least addressable units/AUB.
    return static_cast<quint64>(std::ceil(size / static_cast<double>(addressUnitBits)));
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

    // Check if register is contained within containing register file
    reservedArea.findErrorsInContainment(errors, childRegName, QStringLiteral("Register"),
        context, 0, parentRegisterFileRange - 1);
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::findErrorsInChildRegisterFile()
//-----------------------------------------------------------------------------
void RegisterFileValidator::findErrorsInChildRegisterFile(QStringList& errors, 
    QSharedPointer<RegisterFile> childRegisterFile, QString const& context, QString const& addressUnitBits, 
    qint64 parentRegisterFileRange, QString const& addressBlockWidth, MemoryReserve& reservedArea, 
    QStringList& registerFileNames, QStringList& duplicateRegisterFileNames)
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
    
    // Find errors in nested reg file
    findErrorsIn(errors, childRegisterFile, context, addressUnitBits, addressBlockWidth);

    // Check if nested reg file is contained within parent
    reservedArea.findErrorsInContainment(errors, childRegFileName, QStringLiteral("Register file"), 
        context, 0, parentRegisterFileRange - 1);
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::getTrueRegisterFileRange()
//-----------------------------------------------------------------------------
qint64 RegisterFileValidator::getTrueRegisterFileRange(QSharedPointer<RegisterFile> targetRegisterFile, quint64 singleReplicaRange) const
{
    qint64 dimensionsProduct = 1;

    if (auto memArray = targetRegisterFile->getMemoryArray())
    {
        // Give range as total range of consecutive register file replicas, if stride == single replica range. 
        // Otherwise range is just single replica range.
        auto regFileStrideStr = targetRegisterFile->getStride();
        quint64 regFileStride = expressionParser_->parseExpression(regFileStrideStr).toULongLong();

        if (regFileStrideStr.isEmpty() || regFileStride == singleReplicaRange)
        {
            for (auto const& dimension : *memArray->getDimensions())
            {
                dimensionsProduct *= expressionParser_->parseExpression(dimension->value_).toLongLong();
            }
        }
    }

    return singleReplicaRange * dimensionsProduct;
}

//-----------------------------------------------------------------------------
// Function: RegisterFileValidator::setupMemoryAreas()
//-----------------------------------------------------------------------------
void RegisterFileValidator::setupMemoryAreas(QSharedPointer<RegisterFile> regFile, MemoryReserve& reserve, quint64 addressUnitBits)
{
    if (regFile->getIsPresent().isEmpty() == false && expressionParser_->parseExpression(regFile->getIsPresent()).toInt() != 1 && addressUnitBits != 0)
    {
        return;
    }

    // Create memory areas for all registers and register files and their possible replicas (dimensions)
    // All calculations should be done using address unit bits (unit for range)
    for (auto const& regBase : *regFile->getRegisterData())
    {
        auto dimension = regBase->getDimension(); // Just first dimension which is most commonly in use. Empty if no mem array.
        auto addressOffset = expressionParser_->parseExpression(regBase->getAddressOffset()).toULongLong();
        quint64 regBaseSize = 0; // size of register(file) in AUB
        quint64 regBaseStride = 0; // stride of register(file) in AUB

        if (regBase->getIsPresent().isEmpty() == false &&
            expressionParser_->parseExpression(regBase->getIsPresent()).toInt() != 1)
        {
            continue;
        }

        if (auto asRegister = regBase.dynamicCast<Register>())
        {
            // Register size (in bits) needs to be rounded up to multiple of AUB for overlap checks using AUBs
            regBaseSize = getRegisterSizeInLAU(asRegister, addressUnitBits);
            registerValidator_->setChildItemValidity(asRegister, true);
        }
        else if (auto asRegFile = regBase.dynamicCast<RegisterFile>())
        {
            regBaseSize = expressionParser_->parseExpression(asRegFile->getRange()).toULongLong();
            setChildItemValidity(asRegFile, true);
        }

        // Stride empty
        // - For register: stride = reg size rounded up to closest multiple of AUB
        // - For register file: stride = range
        if (auto regStrideStr = regBase->getStride(); regStrideStr.isEmpty())
        {
            regBaseStride = regBaseSize;
        }
        else
        {
            regBaseStride = expressionParser_->parseExpression(regBase->getStride()).toULongLong();
        }

        // Register has single dimension
        if (dimension.isEmpty() || expressionParser_->parseExpression(dimension).toInt() == 1)
        {
            reserve.addArea(regBase->name(), addressOffset, addressOffset + regBaseSize - 1);
        }
        // Multiple dimensions, calculate number of replicas with dimensions
        else
        {
            auto allDimensions = regBase->getMemoryArray()->getDimensions();

            quint64 replicas = 1;
            std::for_each(allDimensions->cbegin(), allDimensions->cend(),
                [&replicas, this](QSharedPointer<MemoryArray::Dimension> dim)
                {
                    replicas *= expressionParser_->parseExpression(dim->value_).toULongLong();
                });

            for (int i = 0; i < replicas; i++)
            {
                // start from fieldOffset, then add i multiples of stride
                quint64 replicaStart = addressOffset + i * regBaseStride;
                quint64 replicaEnd = replicaStart + regBaseSize - 1;

                reserve.addArea(regBase->name(), replicaStart, replicaEnd);
            }
        }
    }
}
