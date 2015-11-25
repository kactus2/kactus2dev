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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

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
    QSharedPointer<QList<QSharedPointer<Choice> > > choices):
expressionParser_(expressionParser),
availableChoices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::~AddressBlockValidator()
//-----------------------------------------------------------------------------
AddressBlockValidator::~AddressBlockValidator()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::validate()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::validate(QSharedPointer<AddressBlock> addressBlock) const
{
    return hasValidName(addressBlock) && hasValidIsPresent(addressBlock) && hasValidBaseAddress(addressBlock) &&
        hasValidRange(addressBlock) && hasValidWidth(addressBlock) && hasValidParameters(addressBlock) &&
        hasValidRegisterData(addressBlock) && hasValidUsage(addressBlock);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidName()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidName(QSharedPointer<AddressBlock> addressBlock) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(addressBlock->name());

    if (addressBlock->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidIsPresent(QSharedPointer<AddressBlock> addressBlock) const
{
    if (!addressBlock->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(addressBlock->getIsPresent());

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
// Function: AddressBlockValidator::hasValidBaseAddress()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidBaseAddress(QSharedPointer<AddressBlock> addressBlock) const
{
    bool toIntOk = true;
    int baseAddress = expressionParser_->parseExpression(addressBlock->getBaseAddress()).toInt(&toIntOk);

    return toIntOk && baseAddress >= 0;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidRange()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidRange(QSharedPointer<AddressBlock> addressBlock) const
{
    bool toIntOk = true;
    int range = expressionParser_->parseExpression(addressBlock->getRange()).toInt(&toIntOk);

    return toIntOk && range > 0;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidWidth()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidWidth(QSharedPointer<AddressBlock> addressBlock) const
{
    bool toIntOk = true;
    int width = expressionParser_->parseExpression(addressBlock->getWidth()).toInt(&toIntOk);

    return toIntOk && width >= 0;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidParameters(QSharedPointer<AddressBlock> addressBlock) const
{
    if (!addressBlock->getParameters()->isEmpty())
    {
        ParameterValidator2014 validator(expressionParser_, availableChoices_);
        QStringList parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *addressBlock->getParameters())
        {
            if (parameterNames.contains(parameter->name()) || !validator.validate(parameter.data()))
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
// Function: AddressBlockValidator::hasValidRegisterData()
//-----------------------------------------------------------------------------
bool AddressBlockValidator::hasValidRegisterData(QSharedPointer<AddressBlock> addressBlock) const
{
    if (!addressBlock->getRegisterData()->isEmpty())
    {
        RegisterValidator validator(expressionParser_, availableChoices_);
        QStringList registerNames;

        QStringList typeIdentifiers;

        foreach (QSharedPointer<RegisterBase> registerData, *addressBlock->getRegisterData())
        {
            QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
            if (targetRegister)
            {
                if (registerNames.contains(targetRegister->name()) ||
                    registerSizeIsNotWithinBlockWidth(targetRegister, addressBlock) ||
                    !hasValidVolatileForRegister(addressBlock, targetRegister) ||
                    !hasValidAccessWithRegister(addressBlock, targetRegister) ||
                    !validator.validate(targetRegister))
                {
                    return false;
                }
                else
                {
                    if (typeIdentifiers.contains(targetRegister->getTypeIdentifier()))
                    {
                        int typeIdentifierIndex = typeIdentifiers.indexOf(targetRegister->getTypeIdentifier());
                        if (!registersHaveSimilarDefinitionGroups(targetRegister, addressBlock,
                            typeIdentifierIndex))
                        {
                            return false;
                        }
                    }

                    registerNames.append(targetRegister->name());
                    typeIdentifiers.append(targetRegister->getTypeIdentifier());
                }
            }
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
    int registerSize = expressionParser_->parseExpression(targetRegister->getSize()).toInt();
    int addressBlockWidth = expressionParser_->parseExpression(addressBlock->getWidth()).toInt();

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

    foreach (QSharedPointer<Field> field, *targetRegister->getFields())
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
    General::Access blockAccess = addressBlock->getAccess();
    General::Access registerAccess = targetRegister->getAccess();
    if ((blockAccess == General::READ_ONLY && registerAccess != General::READ_ONLY) ||
        (blockAccess == General::WRITE_ONLY && (registerAccess != General::WRITE_ONLY &&
            registerAccess != General::WRITEONCE)) ||
        (blockAccess == General::READ_WRITEONCE && (registerAccess != General::READ_ONLY &&
            registerAccess != General::READ_WRITEONCE && registerAccess != General::WRITEONCE)) ||
        (blockAccess == General::WRITEONCE && registerAccess != General::WRITEONCE))
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
            foreach (QSharedPointer<RegisterBase> registerData, *addressBlock->getRegisterData())
            {
                QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
                if (targetRegister)
                {
                    if (!targetRegister->getVolatile().isEmpty() ||
                        targetRegister->getAccess() != General::ACCESS_COUNT)
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
    QString const& context) const
{
    QString addressBlockContext = "addressBlock " + addressBlock->name();

    findErrorsInName(errors, addressBlock, context);
    findErrorsInIsPresent(errors, addressBlock, context);
    findErrorsInBaseAddress(errors, addressBlock, context);
    findErrorsInRange(errors, addressBlock, context);
    findErrorsInWidth(errors, addressBlock, context);
    findErrorsInUsage(errors, addressBlock, context);
    findErrorsInParameters(errors, addressBlock, addressBlockContext);
    findErrorsInRegisterData(errors, addressBlock, addressBlockContext);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<AddressBlock> addressBlock,
    QString const& context) const
{
    if (!hasValidName(addressBlock))
    {
        errors.append(
            QObject::tr("Invalid name specified for address block %1 within %2").arg(addressBlock->name(),
            context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<AddressBlock> addressBlock, QString const& context) const
{
    if (!hasValidIsPresent(addressBlock))
    {
        errors.append(QObject::tr("Invalid isPresent set for address block %1 within %2").arg(addressBlock->name())
            .arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInBaseAddress()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInBaseAddress(QVector<QString>& errors,
    QSharedPointer<AddressBlock> addressBlock, QString const& context) const
{
    if (!hasValidBaseAddress(addressBlock))
    {
        errors.append(QObject::tr("Invalid baseAddress set for address block %1 within %2").arg(addressBlock->name(), context));
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
        foreach (QSharedPointer<RegisterBase> registerData, *addressBlock->getRegisterData())
        {
            QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
            if (targetRegister)
            {
                if (!targetRegister->getVolatile().isEmpty() ||
                    targetRegister->getAccess() != General::ACCESS_COUNT)
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
// Function: AddressBlockValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInParameters(QVector<QString>&errors,
    QSharedPointer<AddressBlock> addressBlock, QString const& context) const
{
    if (!addressBlock->getParameters()->isEmpty())
    {
        ParameterValidator2014 validator(expressionParser_, availableChoices_);
        QStringList parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *addressBlock->getParameters())
        {
            if (parameterNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Name %1 of parameters in %2 is not unique.").arg(parameter->name()).
                    arg(context));
            }
            else
            {
                parameterNames.append(parameter->name());
            }

            validator.findErrorsIn(errors, parameter, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockValidator::findErrorsInRegisterData()
//-----------------------------------------------------------------------------
void AddressBlockValidator::findErrorsInRegisterData(QVector<QString>& errors,
    QSharedPointer<AddressBlock> addressBlock, QString const& context) const
{
    if (!addressBlock->getRegisterData()->isEmpty())
    {
        RegisterValidator validator(expressionParser_, availableChoices_);
        QStringList registerNames;
        QStringList typeIdentifiers;
        foreach (QSharedPointer<RegisterBase> registerData, *addressBlock->getRegisterData())
        {
            QSharedPointer<Register> targetRegister = registerData.dynamicCast<Register>();
            if (targetRegister)
            {
                if (registerNames.contains(targetRegister->name()))
                {
                    errors.append(QObject::tr("Name %1 of registers in addressBlock %2 is not unique.")
                        .arg(targetRegister->name()).arg(addressBlock->name()));
                }
                else
                {
                    registerNames.append(targetRegister->name());
                }

                validator.findErrorsIn(errors, targetRegister, context);

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

                if (typeIdentifiers.contains(targetRegister->getTypeIdentifier()))
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
                    errors.append(QObject::tr("Access cannot be set to %1 in register %2, where containing address "
                        "block %3 has access %4")
                        .arg(General::access2Str(targetRegister->getAccess()))
                        .arg(targetRegister->name())
                        .arg(addressBlock->name())
                        .arg(General::access2Str(addressBlock->getAccess())));
                }
            }
        }
    }
}