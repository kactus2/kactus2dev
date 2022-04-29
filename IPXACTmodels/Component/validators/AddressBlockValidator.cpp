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
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::AddressBlockValidator()
//-----------------------------------------------------------------------------
AddressBlockValidator::AddressBlockValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<RegisterValidator> registerValidator,
    QSharedPointer<RegisterFileValidator> registerFileValidator,
    QSharedPointer<ParameterValidator> parameterValidator):
MemoryBlockValidator(expressionParser, parameterValidator),
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
    return MemoryBlockValidator::validate(addressBlock) &&
        hasValidRange(addressBlock) &&
        hasValidWidth(addressBlock) &&
        hasValidRegisterData(addressBlock, addressUnitBits) &&
        hasValidUsage(addressBlock);
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
    if (!addressBlock->getRegisterData()->isEmpty())
    {
        QStringList registerNames;

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
                else
                {
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

                        if ( registerBegin < 0 || registerBegin + registerSize > addressBlockRange)
                        {
                            return false;
                        }


                        if(targetRegister->getIsPresent().isEmpty() || 
                            getExpressionParser()->parseExpression(targetRegister->getIsPresent()).toInt())
                        {
                          reservedArea.addArea(targetRegister->name(), registerBegin, registerEnd);
                        }
                    }

                    registerNames.append(targetRegister->name());
                    typeIdentifiers.append(targetRegister->getTypeIdentifier());
                }
            }
        }

        return !reservedArea.hasOverlap();
    }

    return true;
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
// Function: AddressBlockValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
    QString const& addressUnitBits, QString const& context) const
{
    QString addressBlockContext = getBlockType() + QStringLiteral(" ") + addressBlock->name();

    findErrorsInName(errors, addressBlock, context);
    findErrorsInIsPresent(errors, addressBlock, context);
    findErrorsInBaseAddress(errors, addressBlock, context);
    findErrorsInRange(errors, addressBlock, context);
    findErrorsInWidth(errors, addressBlock, context);
    findErrorsInUsage(errors, addressBlock, context);
    findErrorsInParameters(errors, addressBlock, addressBlockContext);
    findErrorsInRegisterData(errors, addressBlock, addressUnitBits, addressBlockContext);
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
    if (!addressBlock->getRegisterData()->isEmpty())
    {
        QStringList registerNames;
        QStringList duplicateNames;
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
                    !duplicateNames.contains(targetRegister->name()))
                {
                    errors.append(QObject::tr("Name %1 of registers in addressBlock %2 is not unique.")
                        .arg(targetRegister->name()).arg(addressBlock->name()));
                    duplicateNames.append(targetRegister->name());
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

                if (!hasValidAccessWithRegister(addressBlock, targetRegister))
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
        }

        reservedArea.findErrorsInOverlap(errors, QLatin1String("Registers"), context);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::getRegisterSizeInLAU()
//-----------------------------------------------------------------------------
qint64 AddressBlockValidator::getRegisterSizeInLAU(QSharedPointer<Register> targetRegister, int addressUnitBits)
    const
{
    qint64 size = getExpressionParser()->parseExpression(targetRegister->getSize()).toLongLong();
    qint64 registerDimension = getExpressionParser()->parseExpression(targetRegister->getDimension()).toLongLong();

    if (registerDimension == 0)
    {
        registerDimension = 1;
    }

    qint64 topPart = size + addressUnitBits -1;
    qint64 dimensionlessSize = topPart / addressUnitBits;

    qint64 trueSize = dimensionlessSize * registerDimension;

    return trueSize;
}
