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
#include <IPXACTmodels/Component/validators/CollectionValidators.h>

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
{
    bool validAmongSiblings = validComparedToSiblings(addressBlock);

    if (docRevision_ == Document::Revision::Std14)
    {
        return MemoryBlockValidator::validate(addressBlock) &&
            hasValidRange(addressBlock) &&
            hasValidWidth(addressBlock) &&
            hasValidRegisterData(addressBlock, addressUnitBits) &&
            hasValidUsage(addressBlock) && validAmongSiblings;
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        // Check mandatory elements, if there is no definition reference.
        if (addressBlock->getAddressBlockDefinitionRef().isEmpty() &&
            (!hasValidRange(addressBlock) || !hasValidWidth(addressBlock)))
        {
            return false;
        }

        return validAmongSiblings && MemoryBlockValidator::validate(addressBlock) &&
            hasValidRegisterAlignment(addressBlock) && hasValidMemoryArray(addressBlock) &&
            hasValidUsage(addressBlock) && hasValidAccessPolicies(addressBlock) &&
            hasValidRegisterData(addressBlock, addressUnitBits) &&
            hasValidStructure(addressBlock) && validAmongSiblings;
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
    QString const& addressUnitBits)
{
    if (addressBlock->getRegisterData()->isEmpty())
    {
        return true;
    }

    QHash<QString, QSharedPointer<RegisterBase> > foundNames;

    QStringList typeIdentifiers;

    // Default AUB to 8 if empty
    QString correctedAUB = addressUnitBits;
    if (addressUnitBits.isEmpty())
    {
        correctedAUB = QStringLiteral("8");
    }

    bool aubChangeOk = true;
    quint64 aubUInt = getExpressionParser()->parseExpression(correctedAUB).toULongLong(&aubChangeOk);
    quint64 addressBlockRange = getExpressionParser()->parseExpression(addressBlock->getRange()).toULongLong();
   
    if (!aubChangeOk || aubUInt == 0)
    {
        return false;
    }

    auto registerData = addressBlock->getRegisterData();
    bool errorFound = false;

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


    // Create memory areas for AB registers and register files
    MemoryReserve memReserve;
    setupMemoryAreas(addressBlock, memReserve, aubUInt);

    // Check memory areas for overlap and containment issues
    QSet<QString> erroneousAreas;
    memReserve.checkOverlapAndContainment(erroneousAreas, addressBlockRange);

    // Register and register file validity together must be checked before separately
    for (auto regIter = registerData->begin(); regIter != registerData->end(); ++regIter)
    {
        if (auto targetRegister = (*regIter).dynamicCast<Register>())
        {
            // Mark name as valid, if not found in map.
            if (auto found = foundNames[targetRegister->name()])
            {
                registerValidator_->setChildItemValidity(targetRegister, false);
                markItemInvalid(foundNames[targetRegister->name()]);
                errorFound = true;
            }
            else
            {
                foundNames.insert(targetRegister->name(), targetRegister);
            }

            if (registerSizeIsNotWithinBlockWidth(targetRegister, addressBlock) ||
                !hasValidVolatileForRegister(addressBlock, targetRegister) ||
                !hasValidAccessWithRegister(addressBlock, targetRegister))
            {
                registerValidator_->setChildItemValidity(targetRegister, false);
                errorFound = true;
            }

            if (!targetRegister->getTypeIdentifier().isEmpty() &&
                typeIdentifiers.contains(targetRegister->getTypeIdentifier()))
            {
                int typeIdentifierIndex = typeIdentifiers.indexOf(targetRegister->getTypeIdentifier());
                if (!registersHaveSimilarDefinitionGroups(targetRegister, addressBlock,
                    typeIdentifierIndex))
                {
                    errorFound = true;
                }
            }

            if (erroneousAreas.contains(targetRegister->name()))
            {
                registerValidator_->setChildItemValidity(targetRegister, false);
                errorFound = true;
            }

            typeIdentifiers.append(targetRegister->getTypeIdentifier());
        }
        else if (auto targetRegisterFile = (*regIter).dynamicCast<RegisterFile>())
        {
            if (auto found = foundNames[targetRegisterFile->name()])
            {
                registerFileValidator_->setChildItemValidity(targetRegisterFile, false);
                markItemInvalid(foundNames[targetRegisterFile->name()]);
            }
            else
            {
                foundNames.insert(targetRegisterFile->name(), targetRegisterFile);
            }

            if (erroneousAreas.contains(targetRegisterFile->name()))
            {
                registerFileValidator_->setChildItemValidity(targetRegisterFile, false);
                errorFound = true;
            }
        }
    }

    if (errorFound)
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
            asRegisterFile && !registerFileValidator_->validate(asRegisterFile, correctedAUB, addressBlock->getWidth()))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::registerIsNotContainedWithinBlock()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::registerSizeIsNotWithinBlockWidth(QSharedPointer<Register> targetRegister,
    QSharedPointer<AddressBlock> addressBlock) const
{
    quint64 registerSize = getExpressionParser()->parseExpression(targetRegister->getSize()).toULongLong();
    quint64 addressBlockWidth = getExpressionParser()->parseExpression(addressBlock->getWidth()).toULongLong();

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
                registerValidator_->setChildItemValidity(targetRegister, false);
                registerValidator_->setChildItemValidity(comparisonRegister, false);
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
    QString const& addressUnitBits, QString const& context)
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
    QSharedPointer<AddressBlock> addressBlock, QString const& addressUnitBits, QString const& context)
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

    QString correctedAUB = addressUnitBits;

    // Avoid divisions by 0, default AUB to 8 bits
    if (addressUnitBits.isEmpty())
    {
        correctedAUB = QStringLiteral("8");
    }

    bool aubChangeOk = true;
    quint64 aubUInt = getExpressionParser()->parseExpression(correctedAUB).toULongLong(&aubChangeOk);
    quint64 addressBlockRange = getExpressionParser()->parseExpression(addressBlock->getRange()).toULongLong();

    if (!aubChangeOk)
        return;

    MemoryReserve reservedArea;
    setupMemoryAreas(addressBlock, reservedArea, aubUInt);

    for (QSharedPointer<RegisterBase> const& registerData : *addressBlock->getRegisterData())
    {
        if (auto targetRegister = registerData.dynamicCast<Register>())
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

            reservedArea.findErrorsInContainment(errors, targetRegister->name(), QStringLiteral("Register"), 
                context, 0, addressBlockRange - 1);
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

            reservedArea.findErrorsInContainment(errors, targetRegisterFile->name(), QStringLiteral("Register file"),
                context, 0, addressBlockRange - 1);
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
    CommonItemsValidator::findErrorsInAccessPolicies(errors, addressBlock->getAccessPolicies(), componentModes_, context);
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
// Function: AddressBlockValidator::setupMemoryAreas()
//-----------------------------------------------------------------------------
void AddressBlockValidator::setupMemoryAreas(QSharedPointer<AddressBlock> addressBlock, MemoryReserve& reserve, 
    quint64 addressUnitBits)
{
    if (addressBlock->getIsPresent().isEmpty() == false && getExpressionParser()->parseExpression(addressBlock->getIsPresent()).toInt() != 1)
    {
        return;
    }

    // Create memory areas for all registers and register files and their possible replicas (dimensions)
    // All calculations should be done using address unit bits (unit for range)
    for (auto const& regBase : *addressBlock->getRegisterData())
    {
        auto dimension = regBase->getDimension(); // Just first dimension which is most commonly in use. Empty if no mem array.
        auto addressOffset = getExpressionParser()->parseExpression(regBase->getAddressOffset()).toULongLong();
        quint64 regBaseSize = 0; // size of register(file) in AUB
        quint64 regBaseStride = 0; // stride of register(file) in AUB

        if (regBase->getIsPresent().isEmpty() == false &&
            getExpressionParser()->parseExpression(regBase->getIsPresent()).toInt() != 1)
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
            regBaseSize = getExpressionParser()->parseExpression(asRegFile->getRange()).toULongLong();
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
            regBaseStride = getExpressionParser()->parseExpression(regBase->getStride()).toULongLong();
        }

        // Register has single dimension
        if (dimension.isEmpty() || getExpressionParser()->parseExpression(dimension).toInt() == 1)
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
                    replicas *= getExpressionParser()->parseExpression(dim->value_).toULongLong();
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

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::getRegisterSizeInLAU()
//-----------------------------------------------------------------------------
quint64 AddressBlockValidator::getRegisterSizeInLAU(QSharedPointer<Register> targetRegister, quint64 addressUnitBits) const
{
    quint64 size = getExpressionParser()->parseExpression(targetRegister->getSize()).toULongLong();

    // Round register size up to closest multiple of AUB to get size in least addressable units/AUB.
    return static_cast<quint64>(std::ceil(size / static_cast<double>(addressUnitBits)));
}
