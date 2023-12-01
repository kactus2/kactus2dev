//-----------------------------------------------------------------------------
// File: AddressBlockValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.11.2015
//
// Description:
// Validator for ipxact:addressBlock.
//-----------------------------------------------------------------------------

#include "AddressBlockValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/MemoryReserve.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/RegisterFileValidator.h>
#include <IPXACTmodels/Component/validators/MemoryArrayValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MemoryArray.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::AddressBlockValidator()
//-----------------------------------------------------------------------------
AddressBlockValidator::AddressBlockValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterValidator> registerValidator,
    QSharedPointer<RegisterFileValidator> registerFileValidator,
    QSharedPointer<ParameterValidator> parameterValidator,
    Document::Revision docRevision) :
MemoryBlockValidator(expressionParser, parameterValidator, docRevision),
registerValidator_(registerValidator),
registerFileValidator_(registerFileValidator)
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::getBlockType()
//-----------------------------------------------------------------------------
QString AddressBlockValidator::getBlockType() const
{
    return QStringLiteral("address block");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::componentChange()
//-----------------------------------------------------------------------------
void AddressBlockValidator::componentChange(QSharedPointer<Component> newComponent)
{
    registerValidator_->componentChange(newComponent);
    registerFileValidator_->componentChange(newComponent);

    if (newComponent)
    {
        docRevision_ = newComponent->getRevision();
        componentModes_ = newComponent->getModes();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::getRegisterValidator()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterValidator> AddressBlockValidator::getRegisterValidator() const
{
    return registerValidator_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::getRegisterFileValidator()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterFileValidator> AddressBlockValidator::getRegisterFileValidator() const
{
    return registerFileValidator_;
}
//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::validate()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::validate(QSharedPointer<AddressBlock> addressBlock, QString const& addressUnitBits)
    const
{

    if (docRevision_ == Document::Revision::Std14)
    {
        return MemoryBlockValidator::validate(addressBlock) &&
            hasValidRange(addressBlock) &&
            hasValidWidth(addressBlock) &&
            hasValidRegisterData(addressBlock, addressUnitBits) &&
            hasValidUsage(addressBlock);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        // Check mandatory elements, if there is no definition reference.
        if (addressBlock->getAddressBlockDefinitionRef().isEmpty() &&
            (!hasValidRange(addressBlock) || !hasValidWidth(addressBlock)))
        {
            return false;
        }

        return MemoryBlockValidator::validate(addressBlock) &&
            hasValidRegisterAlignment(addressBlock) && hasValidMemoryArray(addressBlock) &&
            hasValidUsage(addressBlock) && hasValidAccessPolicies(addressBlock) &&
            hasValidRegisterData(addressBlock, addressUnitBits) &&
            hasValidStructure(addressBlock);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidRange()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidRange(QSharedPointer<AddressBlock> addressBlock) const
{
    bool toIntOk = true;
    quint64 range = getExpressionParser()->parseExpression(addressBlock->getRange()).toULongLong(&toIntOk);

    return toIntOk && range > 0;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidWidth()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidWidth(QSharedPointer<AddressBlock> addressBlock) const
{
    bool toIntOk = true;
    getExpressionParser()->parseExpression(addressBlock->getWidth()).toULongLong(&toIntOk);

    return toIntOk;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidRegisterData()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidRegisterData(QSharedPointer<AddressBlock> addressBlock,
    QString const& addressUnitBits) const
{
    if (addressBlock->getRegisterData()->isEmpty())
    {
        return true;
    }

    QStringList registerNames;
    QStringList registerFileNames;

    QStringList typeIdentifiers;
    MemoryReserve reservedArea;

    bool aubChangeOk = true;
    qint64 aubInt = getExpressionParser()->parseExpression(addressUnitBits).toLongLong(&aubChangeOk);
    qint64 addressBlockRange = getExpressionParser()->parseExpression(addressBlock->getRange()).toLongLong();

    for (QSharedPointer<RegisterBase> registerData : *addressBlock->getRegisterData())
    {
        QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
        if (targetRegister)
        {
            if (registerNames.contains(targetRegister->name()) ||
                !registerValidator_->validate(targetRegister) ||
                registerSizeIsNotWithinBlockWidth(targetRegister, addressBlock) ||
                !hasValidVolatileForRegister(addressBlock, targetRegister) ||
                !hasValidAccessWithRegister(addressBlock, targetRegister))
            {
                return false;
            }
            
            if (!targetRegister->getTypeIdentifier().isEmpty() &&
                typeIdentifiers.contains(targetRegister->getTypeIdentifier()))
            {
                int typeIdentifierIndex = typeIdentifiers.indexOf(targetRegister->getTypeIdentifier());
                if (!registersHaveSimilarDefinitionGroups(targetRegister, addressBlock,
                    typeIdentifierIndex))
                {
                    return false;
                }
            }

            if (aubChangeOk && aubInt != 0)
            {
                qint64 registerSize = getRegisterSizeInLAU(targetRegister, aubInt);

                qint64 registerBegin = getExpressionParser()->parseExpression(
                    targetRegister->getAddressOffset()).toLongLong();

                qint64 registerEnd = registerBegin + registerSize - 1;

                if (registerBegin < 0 || registerBegin + registerSize > addressBlockRange)
                {
                    return false;
                }


                if (targetRegister->getIsPresent().isEmpty() ||
                    getExpressionParser()->parseExpression(targetRegister->getIsPresent()).toInt())
                {
                    reservedArea.addArea(targetRegister->name(), registerBegin, registerEnd);
                }
            }

            registerNames.append(targetRegister->name());
            typeIdentifiers.append(targetRegister->getTypeIdentifier());
        }
        else if (QSharedPointer<RegisterFile> targetRegisterFile = registerData.dynamicCast<RegisterFile>())
        {
            if (!registerFileValidator_->validate(targetRegisterFile, addressUnitBits, addressBlock->getWidth()) || 
                registerFileNames.contains(targetRegisterFile->name()))
            {
                return false;
            }

            qint64 registerFileBegin = getExpressionParser()->parseExpression(
                targetRegisterFile->getAddressOffset()).toLongLong();

            qint64 registerFileRangeInt = getTrueRegisterFileRange(targetRegisterFile);

            qint64 registerFileEnd = registerFileBegin + registerFileRangeInt - 1;

            if (targetRegisterFile->getIsPresent().isEmpty() ||
                getExpressionParser()->parseExpression(targetRegisterFile->getIsPresent()).toInt())
            {
                reservedArea.addArea(targetRegisterFile->name(), registerFileBegin, registerFileEnd);
            }

            if (registerFileBegin < 0 || registerFileBegin + registerFileRangeInt > addressBlockRange)
            {
                return false;
            }

            registerFileNames.append(targetRegisterFile->name());
        }
    }

    return !reservedArea.hasOverlap();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::registerIsNotContainedWithinBlock()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::registerSizeIsNotWithinBlockWidth(QSharedPointer<Register> targetRegister,
    QSharedPointer<AddressBlock> addressBlock) const
{
    int registerSize = getExpressionParser()->parseExpression(targetRegister->getSize()).toInt();
    int addressBlockWidth = getExpressionParser()->parseExpression(addressBlock->getWidth()).toInt();

    return registerSize > addressBlockWidth;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasInvalidVolatileForRegister()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidVolatileForRegister(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<Register> targetRegister) const
{
    if (targetRegister->getVolatile() == QLatin1String("true") &&
        addressBlock->getVolatile() == QLatin1String("false"))
    {
        return false;
    }

    for (QSharedPointer<Field> const& field : *targetRegister->getFields())
    {
        if (field->getVolatile().toBool() == true && addressBlock->getVolatile() == QLatin1String("false"))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidAccessWithRegister()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidAccessWithRegister(QSharedPointer<AddressBlock> addressBlock,
    QSharedPointer<Register> targetRegister) const
{
    AccessTypes::Access blockAccess = addressBlock->getAccess();
    AccessTypes::Access registerAccess = targetRegister->getAccess();
    if ((blockAccess == AccessTypes::READ_ONLY && registerAccess != AccessTypes::READ_ONLY) ||
        (blockAccess == AccessTypes::WRITE_ONLY && (registerAccess != AccessTypes::WRITE_ONLY &&
            registerAccess != AccessTypes::WRITEONCE)) ||
        (blockAccess == AccessTypes::READ_WRITEONCE && (registerAccess != AccessTypes::READ_ONLY &&
            registerAccess != AccessTypes::READ_WRITEONCE && registerAccess != AccessTypes::WRITEONCE)) ||
        (blockAccess == AccessTypes::WRITEONCE && registerAccess != AccessTypes::WRITEONCE))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasAddressBlockDefinitionGroupDefined()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasAddressBlockDefinitionGroupDefined(QSharedPointer<AddressBlock> addressBlock) const
{
    return !addressBlock->getTypeIdentifier().isEmpty() || !addressBlock->getRange().isEmpty() ||
        !addressBlock->getWidth().isEmpty() || hasMemoryBlockDataGroupDefined(addressBlock) ||
        !addressBlock->getRegisterData()->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasMemoryBlockDataGroupDefined()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasMemoryBlockDataGroupDefined(QSharedPointer<AddressBlock> addressBlock) const
{
    return addressBlock->getUsage() != General::USAGE_COUNT || !addressBlock->getVolatile().isEmpty() ||
        !addressBlock->getAccessPolicies()->isEmpty() || !addressBlock->getParameters()->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::registersHaveSimilarDefinitionGroups()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::registersHaveSimilarDefinitionGroups(QSharedPointer<Register> targetRegister,
    QSharedPointer<AddressBlock> addressBlock, int indexOfSimilarDefinition) const
{
    if (indexOfSimilarDefinition <= addressBlock->getRegisterData()->size())
    {
        QSharedPointer<RegisterBase> registerData = addressBlock->getRegisterData()->at(indexOfSimilarDefinition);
        QSharedPointer<Register> comparisonRegister = registerData.dynamicCast<Register>();
        if (comparisonRegister)
        {
            if (targetRegister->getSize() != comparisonRegister->getSize() ||
                targetRegister->getVolatile() != comparisonRegister->getVolatile() ||
                targetRegister->getAccess() != comparisonRegister->getAccess())
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidUsage()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidUsage(QSharedPointer<AddressBlock> addressBlock) const
{
    if (!addressBlock->getRegisterData()->isEmpty())
    {
        if (addressBlock->getUsage() == General::RESERVED)
        {
            return false;
        }
        else if (addressBlock->getUsage() == General::MEMORY)
        {
            for (QSharedPointer<RegisterBase> const& registerData : *addressBlock->getRegisterData())
            {
                QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
                if (targetRegister)
                {
                    if (!targetRegister->getVolatile().isEmpty() ||
                        targetRegister->getAccess() != AccessTypes::ACCESS_COUNT)
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidRegisterAlignment()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidRegisterAlignment(QSharedPointer<AddressBlock> addressBlock) const
{
    // Check scr 7.27 (for each register in address block, if misalignment == false 
    // then the following should be true: (register bit offset % addressBlock width) + register size <= address block width)

    if (docRevision_ != Document::Revision::Std22 || General::str2Bool(addressBlock->getMisalignmentAllowed(), true))
    {
        return true;
    }

    bool addressBlockExpressionOk;
    qint64 addressBlockWidth = getExpressionParser()->parseExpression(addressBlock->getWidth(), &addressBlockExpressionOk).toLongLong();

    for (auto const& singleRegisterData : *addressBlock->getRegisterData())
    {
        if (auto singleRegister = singleRegisterData.dynamicCast<Register>())
        {
            bool registerSizeOk;
            qint64 registerSize = getExpressionParser()->parseExpression(singleRegister->getSize(), &registerSizeOk).toLongLong();

            bool registerOffsetOk;
            qint64 registerOffset = getExpressionParser()->parseExpression(singleRegister->getAddressOffset(), &registerOffsetOk).toLongLong();

            // SCR 7.27
            qint64 valueToCheck = (registerOffset % addressBlockWidth) + registerSize;

            if (addressBlockExpressionOk && registerOffsetOk && registerSizeOk &&
                valueToCheck > addressBlockWidth)
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidMemoryArray()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidMemoryArray(QSharedPointer<AddressBlock> addressBlock) const
{
    if (auto memArray = addressBlock->getMemoryArray())
    {
        MemoryArrayValidator validator(getExpressionParser());

        return validator.validate(memArray);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidAccessPolicies()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidAccessPolicies(QSharedPointer<AddressBlock> addressBlock) const
{
    return CommonItemsValidator::hasValidAccessPolicies(addressBlock->getAccessPolicies(), componentModes_);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidStructure()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidStructure(QSharedPointer<AddressBlock> addressBlock) const
{
    if (docRevision_ == Document::Revision::Std22)
    {
        if (!addressBlock->getTypeDefinitionsRef().isEmpty() && hasAddressBlockDefinitionGroupDefined(addressBlock))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
    QString const& addressUnitBits, QString const& context) const
{    
    QString addressBlockContext = getBlockType() + QStringLiteral(" ") + addressBlock->name();
    findErrorsInName(errors, addressBlock, context);
    findErrorsInBaseAddress(errors, addressBlock, context);
    findErrorsInUsage(errors, addressBlock, context);
    findErrorsInParameters(errors, addressBlock, addressBlockContext);
    findErrorsInRegisterData(errors, addressBlock, addressUnitBits, addressBlockContext);

    if (docRevision_ == Document::Revision::Std14)
    {
        findErrorsInIsPresent(errors, addressBlock, context);
        findErrorsInRange(errors, addressBlock, context);
        findErrorsInWidth(errors, addressBlock, context);
    }

    else if (docRevision_ == Document::Revision::Std22)
    {
        if (addressBlock->getAddressBlockDefinitionRef().isEmpty())
        {
            findErrorsInRange(errors, addressBlock, context);
            findErrorsInWidth(errors, addressBlock, context);
        }

        QString preciseContext = QStringLiteral("address block ") + addressBlock->name() + QStringLiteral(" within ") + context;
        findErrorsInMemoryArray(errors, addressBlock, preciseContext);
        findErrorsInAccessPolicies(errors, addressBlock, preciseContext);
        findErrorsInRegisterAlignment(errors, addressBlock, preciseContext);
        findErrorsInStructure(errors, addressBlock, context);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInRange()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInRange(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
    QString const& context) const
{
    if (!hasValidRange(addressBlock))
    {
        errors.append(QObject::tr("Invalid range set for address block %1 within %2").
            arg(addressBlock->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInWidth()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInWidth(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
    QString const& context) const
{
    if (!hasValidWidth(addressBlock))
    {
        errors.append(QObject::tr("Invalid width set for address block %1 within %2").arg(addressBlock->name())
            .arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInUsage()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInUsage(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
    QString const& context) const
{
    if (addressBlock->getUsage() == General::RESERVED && !addressBlock->getRegisterData()->isEmpty())
    {
        errors.append(QObject::tr("Registers cannot be contained in address block %1 with usage %2 within %3")
            .arg(addressBlock->name()).arg(General::usage2Str(addressBlock->getUsage())).arg(context));
    }
    else if (addressBlock->getUsage() == General::MEMORY)
    {
        for (QSharedPointer<RegisterBase> const& registerData : *addressBlock->getRegisterData())
        {
            QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
            if (targetRegister)
            {
                if (!targetRegister->getVolatile().isEmpty() ||
                    targetRegister->getAccess() != AccessTypes::ACCESS_COUNT)
                {
                    errors.append(QObject::tr("Access and volatile values must be empty for register %1 in "
                        "address block %2 with usage %3 within %4").arg(targetRegister->name())
                        .arg(addressBlock->name()).arg(General::usage2Str(addressBlock->getUsage())).arg(context));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInRegisterData()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInRegisterData(QVector<QString>& errors,
    QSharedPointer<AddressBlock> addressBlock, QString const& addressUnitBits, QString const& context) const
{
    if (addressBlock->getRegisterData()->isEmpty())
    {
        return;
    }

    QStringList registerNames;
    QStringList registerFileNames;
    QStringList duplicateRegisterNames;
    QStringList duplicateRegisterFileNames;
    QStringList typeIdentifiers;

    MemoryReserve reservedArea;
    bool aubChangeOk = true;
    qint64 aubInt = getExpressionParser()->parseExpression(addressUnitBits).toLongLong(&aubChangeOk);
    qint64 addressBlockRange = getExpressionParser()->parseExpression(addressBlock->getRange()).toLongLong();

    for (QSharedPointer<RegisterBase> const& registerData : *addressBlock->getRegisterData())
    {
        QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
        if (targetRegister)
        {
            if (registerNames.contains(targetRegister->name()) &&
                !duplicateRegisterNames.contains(targetRegister->name()))
            {
                errors.append(QObject::tr("Name %1 of registers in addressBlock %2 is not unique.")
                    .arg(targetRegister->name()).arg(addressBlock->name()));
                duplicateRegisterNames.append(targetRegister->name());
            }
            else
            {
                registerNames.append(targetRegister->name());
            }

            registerValidator_->findErrorsIn(errors, targetRegister, context);

            if (registerSizeIsNotWithinBlockWidth(targetRegister, addressBlock))
            {
                errors.append(QObject::tr("Register %1 size must not be greater than the containing "
                    "addressBlock %2 width.").arg(targetRegister->name()).arg(addressBlock->name()));
            }
                
            if (!hasValidVolatileForRegister(addressBlock, targetRegister))
            {
                errors.append(QObject::tr("Volatile value cannot be set to false for addressBlock %1 "
                    "containing a register or register field with volatile true").arg(addressBlock->name()));
            }

            if (!targetRegister->getTypeIdentifier().isEmpty() &&
                typeIdentifiers.contains(targetRegister->getTypeIdentifier()))
            {
                int typeIdentifierIndex = typeIdentifiers.indexOf(targetRegister->getTypeIdentifier());

                if (!registersHaveSimilarDefinitionGroups(targetRegister, addressBlock, typeIdentifierIndex))
                {
                    errors.append(QObject::tr("Registers containing the same type identifiers must contain "
                        "similar register definitions within %1").arg(context));
                }
            }
            typeIdentifiers.append(targetRegister->getTypeIdentifier());

            if (docRevision_ == Document::Revision::Std14 && !hasValidAccessWithRegister(addressBlock, targetRegister))
            {
                errors.append(QObject::tr(
                    "Access cannot be set to %1 in register %2, where containing address block %3 has access %4")
                    .arg(AccessTypes::access2Str(targetRegister->getAccess()))
                    .arg(targetRegister->name())
                    .arg(addressBlock->name())
                    .arg(AccessTypes::access2Str(addressBlock->getAccess())));
            }

            if (aubChangeOk && aubInt != 0)
            {
                qint64 registerSize = getRegisterSizeInLAU(targetRegister, aubInt);

                qint64 registerBegin =
                    getExpressionParser()->parseExpression(targetRegister->getAddressOffset()).toLongLong();

                qint64 registerEnd = registerBegin + registerSize - 1;

                if(targetRegister->getIsPresent().isEmpty() ||
                    getExpressionParser()->parseExpression(targetRegister->getIsPresent()).toInt())
                {
                    reservedArea.addArea(targetRegister->name(), registerBegin, registerEnd);
                }

                if ( registerBegin < 0 || registerBegin + registerSize > addressBlockRange)
                {
                    errors.append(QObject::tr("Register %1 is not contained within %2")
                        .arg(targetRegister->name()).arg(context));
                }
            }
        }
        else if (QSharedPointer<RegisterFile> targetRegisterFile = registerData.dynamicCast<RegisterFile>())
        {
            if (registerFileNames.contains(targetRegisterFile->name()) &&
                !duplicateRegisterFileNames.contains(targetRegisterFile->name()))
            {
                errors.append(QObject::tr("Name %1 of register files in addressBlock %2 is not unique.")
                    .arg(targetRegisterFile->name()).arg(addressBlock->name()));
                duplicateRegisterFileNames.append(targetRegisterFile->name());
            }
            else
            {
                registerFileNames.append(targetRegisterFile->name());
            }

            registerFileValidator_->findErrorsIn(errors, targetRegisterFile, context, addressUnitBits, addressBlock->getWidth());

            qint64 registerFileBegin = getExpressionParser()->parseExpression(
                targetRegisterFile->getAddressOffset()).toLongLong();

            qint64 registerFileRangeInt = getTrueRegisterFileRange(targetRegisterFile);

            qint64 registerFileEnd = registerFileBegin + registerFileRangeInt - 1;

            if (targetRegisterFile->getIsPresent().isEmpty() ||
                getExpressionParser()->parseExpression(targetRegisterFile->getIsPresent()).toInt())
            {
                reservedArea.addArea(targetRegisterFile->name(), registerFileBegin, registerFileEnd);
            }

            if (registerFileBegin < 0 || registerFileBegin + registerFileRangeInt > addressBlockRange)
            {
                errors.append(QObject::tr("Register file %1 is not contained within %2")
                    .arg(targetRegisterFile->name()).arg(context));
            }
        }
    }

    reservedArea.findErrorsInOverlap(errors, QLatin1String("Register data"), context);
    
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInMemoryArray()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInMemoryArray(QStringList& errors, QSharedPointer<AddressBlock> addressBlock, QString const& context) const
{
    if (auto memArray = addressBlock->getMemoryArray())
    {
        MemoryArrayValidator validator(getExpressionParser());

        validator.findErrorsIn(errors, memArray, context);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInAccessPolicies()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInAccessPolicies(QStringList& errors, QSharedPointer<AddressBlock> addressBlock, QString const& context) const
{
    bool hasAccessPolicyWithoutModeRef = false;

    QString accessPolicyContext = QStringLiteral("access policies of ") + context;

    bool duplicateModeRefErrorIssued = false;
    bool duplicateModePriorityErrorIssued = false;

    QStringList checkedModeReferences;
    QList<unsigned int> checkedModePriorities;

    for (auto const& accessPolicy : *addressBlock->getAccessPolicies())
    {
        if (accessPolicy->getModeReferences()->isEmpty())
        {
            hasAccessPolicyWithoutModeRef = true;
        }

        // Check mode references in current access policy, and look for duplicate references.
        CommonItemsValidator::findErrorsInModeRefs(errors, accessPolicy->getModeReferences(), accessPolicyContext,
            checkedModeReferences, checkedModePriorities, &duplicateModeRefErrorIssued, &duplicateModePriorityErrorIssued, componentModes_);
    }

    // Number of access policies cannot be greater than one if one access policy has no mode references.
    if (hasAccessPolicyWithoutModeRef && addressBlock->getAccessPolicies()->size() > 1)
    {
        errors.append(QObject::tr("In %1, multiple access policies are not allowed if one "
            "of them lacks a mode reference.").arg(addressBlock->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInRegisterAlignment()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInRegisterAlignment(QStringList& errors, QSharedPointer<AddressBlock> addressBlock, QString const& context) const
{
    if (!hasValidRegisterAlignment(addressBlock))
    {
        errors.append(QObject::tr("Register misalignment set to false for %1, "
            "where one or more registers are fully or partly offset outside of the address block width.")
            .arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInStructure()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInStructure(QStringList& errors, QSharedPointer<AddressBlock> addressBlock,
    QString const& context) const
{
    if (!hasValidStructure(addressBlock))
    {
        errors.append(QObject::tr("Address block %1 in %2 must not be explicitly defined while also containing" 
            " a definition reference.").arg(addressBlock->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::getRegisterSizeInLAU()
//-----------------------------------------------------------------------------
qint64 AddressBlockValidator::getRegisterSizeInLAU(QSharedPointer<Register> targetRegister, qint64 addressUnitBits) const
{
    qint64 size = getExpressionParser()->parseExpression(targetRegister->getSize()).toLongLong();
    qint64 dimensionsProduct = 1;

    if (auto memArray = targetRegister->getMemoryArray())
    {
        for (auto const& dimension : *memArray->getDimensions())
        {
            dimensionsProduct *= getExpressionParser()->parseExpression(dimension->value_).toLongLong();
        }
    }

    // Round register size up to closest multiple of AUB to get size in least addressable units/AUB.
    qint64 dimensionlessSize = static_cast<qint64>(std::ceil(size / static_cast<double>(addressUnitBits)));

    qint64 trueSize = dimensionlessSize * dimensionsProduct;

    return trueSize;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::getRealRegisterFileRange()
//-----------------------------------------------------------------------------
qint64 AddressBlockValidator::getTrueRegisterFileRange(QSharedPointer<RegisterFile> targetRegisterFile) const
{
    qint64 dimensionsProduct = 1;

    if (auto memArray = targetRegisterFile->getMemoryArray())
    {
        for (auto const& dimension : *memArray->getDimensions())
        {
            dimensionsProduct *= getExpressionParser()->parseExpression(dimension->value_).toLongLong();
        }
    }

    qint64 dimensionlessRange = getExpressionParser()->parseExpression(targetRegisterFile->getRange()).toLongLong();
    qint64 trueRange = dimensionlessRange * dimensionsProduct;

    return trueRange;
}
