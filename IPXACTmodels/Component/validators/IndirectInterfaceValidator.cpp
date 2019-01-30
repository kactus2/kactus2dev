//-----------------------------------------------------------------------------
// File: IndirectInterfaceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 10.08.2017
//
// Description:
// Validator for ipxact:indirectInterface.
//-----------------------------------------------------------------------------

#include "IndirectInterfaceValidator.h"


#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::IndirectInterfaceValidator()
//-----------------------------------------------------------------------------
IndirectInterfaceValidator::IndirectInterfaceValidator(QSharedPointer<Component> component,
    QSharedPointer<ExpressionParser> parser, QSharedPointer<ParameterValidator> parameterValidator): 
component_(component), expressionParser_(parser),
    parameterValidator_(parameterValidator)
{

}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::componentChange()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::componentChange(QSharedPointer<Component> component)
{
    component_ = component;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::validate()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::validate(QSharedPointer<IndirectInterface> indirectInterface) const
{
	return hasValidName(indirectInterface) && 
        hasValidAddressReference(indirectInterface) &&
        hasValidDataReference(indirectInterface) &&
        hasEitherMemoryMapReferenceOrTransparentBridge(indirectInterface) &&
        hasValidMemoryMapReference(indirectInterface) &&
        hasValidTransparentBridges(indirectInterface) &&
        hasValidBitsInLau(indirectInterface) &&
        hasValidEndianness(indirectInterface) &&
        hasValidParameters(indirectInterface);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasValidName()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasValidName(QSharedPointer<IndirectInterface> indirectInterface) const
{
    QRegularExpression whiteSpaceExpression(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(indirectInterface->name());

    return !indirectInterface->name().isEmpty() && !whiteSpaceMatch.hasMatch();
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasValidAddressReference()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasValidAddressReference(QSharedPointer<IndirectInterface> indirectInterface) const
{
    QSharedPointer<Field> addressField = findField(indirectInterface->getIndirectAddressRef());

    return isValidFieldReference(indirectInterface->getIndirectAddressRef(), indirectInterface->getMemoryMapRef()) &&
        addressField->getAccess() != AccessTypes::READ_ONLY &&
        addressField->getAccess() != AccessTypes::READ_WRITEONCE &&
        addressField->getAccess() != AccessTypes::WRITEONCE;;;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasValidDataReference()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasValidDataReference(QSharedPointer<IndirectInterface> indirectInterface) const
{
    return isValidFieldReference(indirectInterface->getIndirectDataRef(), indirectInterface->getMemoryMapRef());
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasValidMemoryMapReference()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasValidMemoryMapReference(QSharedPointer<IndirectInterface> indirectInterface) const
{
    if (indirectInterface->getMemoryMapRef().isEmpty())
    {
        return true;
    }

    for (QSharedPointer<MemoryMap> const& map : *component_->getMemoryMaps())
    {
        if (map->name().compare(indirectInterface->getMemoryMapRef()) == 0)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasValidTransparentBridges()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasValidTransparentBridges(QSharedPointer<IndirectInterface> indirectInterface) const
{
    int foundInterfaces = 0;
    for (QSharedPointer<TransparentBridge> const& bridge : *indirectInterface->getTransparentBridges())
    {
        for (QSharedPointer<BusInterface> const& busInterface : *component_->getBusInterfaces())
        {
            if (busInterface->name().compare(bridge->getMasterRef()) == 0 &&
                busInterface->getInterfaceMode() == General::MASTER)
            {
                foundInterfaces++;
                break;
            }
        }
    }

    return indirectInterface->getTransparentBridges()->count() == foundInterfaces;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasValidBitsInLau()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasValidBitsInLau(QSharedPointer<IndirectInterface> indirectInterface) const
{
    if (indirectInterface->getBitsInLau().isEmpty())
    {
        return true;
    }

    bool isValidBits = false;
    expressionParser_->parseExpression(indirectInterface->getBitsInLau(), &isValidBits);
    if (isValidBits == false)
    {
        return false;
    }

    bool canConvert = false;
    int value = expressionParser_->parseExpression(indirectInterface->getBitsInLau()).toInt(&canConvert);

    return canConvert && value >= 0;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasValidEndianness()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasValidEndianness(QSharedPointer<IndirectInterface> indirectInterface) const
{
    QString endianness = indirectInterface->getEndianness();
    return endianness.isEmpty() || endianness.compare(QStringLiteral("big")) == 0 ||
        endianness.compare(QStringLiteral("little")) == 0;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasValidParameters(QSharedPointer<IndirectInterface> indirectInterface) const
{
    QStringList parameterNames;
    for (QSharedPointer<Parameter> const& parameter : *indirectInterface->getParameters())
    {
        if (parameterNames.contains(parameter->name()) || !parameterValidator_->validate(parameter))
        {
            return false;
        }

        parameterNames.append(parameter->name());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsIn(QVector<QString>& errors, 
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    findErrorsInName(errors, indirectInterface, context);
    findErrorsInAddressReference(errors, indirectInterface, context);
    findErrorsInDataReference(errors, indirectInterface, context);
    findErrorsInMemoryReference(errors, indirectInterface, context);
    findErrorsInMemoryMap(errors, indirectInterface, context);
    findErrorsInTransparentBridges(errors, indirectInterface, context);
    findErrorsInBitsInLau(errors, indirectInterface, context);
    findErrorsInEndianness(errors, indirectInterface, context);
    findErrorsInParameters(errors, indirectInterface, context);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findField()
//-----------------------------------------------------------------------------
QSharedPointer<Field> IndirectInterfaceValidator::findField(QString const& fieldReference) const
{
    for (QSharedPointer<MemoryMap> const& map : *component_->getMemoryMaps())
    {
        for (QSharedPointer<MemoryBlockBase> const& block : *map->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addressBlock = block.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                for (QSharedPointer<RegisterBase> const& registerBase : *addressBlock->getRegisterData())
                {
                    QSharedPointer<Register> reg = registerBase.dynamicCast<Register>();
                    if (reg)
                    {
                        for (QSharedPointer<Field> const& field : *reg->getFields())
                        {
                            if (field->getId().compare(fieldReference) == 0)
                            {
                                return field;                                    
                            }
                        }
                    }
                }
            }
        }
    }

    return QSharedPointer<Field>(0);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::isValidFieldReference()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::isValidFieldReference(QString const& fieldReference,
    QString const& targetMemoryMap) const
{
    if (fieldReference.isEmpty())
    {
        return false;
    }

    QSharedPointer<Field> targetField = findField(fieldReference);

    return targetField && !memoryMapContainsField(targetMemoryMap, fieldReference);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findMemoryMapFor()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::memoryMapContainsField(QString const& memoryMapName,
    QString const& fieldReference) const
{
    for (QSharedPointer<MemoryMap> const& map : *component_->getMemoryMaps())
    {
        if (map->name().compare(memoryMapName) == 0)
        {
            for (QSharedPointer<MemoryBlockBase> const& block : *map->getMemoryBlocks())
            {
                QSharedPointer<AddressBlock> addressBlock = block.dynamicCast<AddressBlock>();
                if (addressBlock)
                {
                    for (QSharedPointer<RegisterBase> const& registerBase : *addressBlock->getRegisterData())
                    {
                        QSharedPointer<Register> reg = registerBase.dynamicCast<Register>();
                        if (reg)
                        {
                            for (QSharedPointer<Field> const& field : *reg->getFields())
                            {
                                if (field->getId().compare(fieldReference) == 0)
                                {
                                    return true;                                  
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::hasEitherMemoryMapReferenceOrTransparentBridge()
//-----------------------------------------------------------------------------
bool IndirectInterfaceValidator::hasEitherMemoryMapReferenceOrTransparentBridge(
    QSharedPointer<IndirectInterface> indirectInterface) const
{
    bool hasMemoryMap = !indirectInterface->getMemoryMapRef().isEmpty();
    bool hasBridges = !indirectInterface->getTransparentBridges()->isEmpty();

    return (hasMemoryMap && !hasBridges) || (!hasMemoryMap && hasBridges);
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    if (!hasValidName(indirectInterface))
    {
        errors.append(QStringLiteral("Invalid name specified for indirect interface '%1' within %2").arg(
            indirectInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInAddressReference()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInAddressReference(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    if (indirectInterface->getIndirectAddressRef().isEmpty())
    {
        errors.append(QStringLiteral("No field specified for address in indirect interface '%1' within %2")
            .arg(indirectInterface->name(), context));
        return;
    }
    
    QSharedPointer<Field> targetField = findField(indirectInterface->getIndirectAddressRef());
    if (targetField.isNull())
    {
        errors.append(QStringLiteral(
            "Field '%1' not found for address in indirect interface '%2' within %3").arg(
            indirectInterface->getIndirectAddressRef(), indirectInterface->name(), context));
    }
    else if (memoryMapContainsField(indirectInterface->getMemoryMapRef(), indirectInterface->getIndirectAddressRef()))
    {
        errors.append(QStringLiteral(
            "Field '%1' is defined within indirect memory map '%2' in indirect interface %3 within %4").arg(
            indirectInterface->getIndirectAddressRef(), indirectInterface->getMemoryMapRef(), 
            indirectInterface->name(), context));
    }

    if (targetField && (targetField->getAccess() == AccessTypes::READ_ONLY ||
        targetField->getAccess() == AccessTypes::READ_WRITEONCE ||
        targetField->getAccess() == AccessTypes::WRITEONCE))
    {
        errors.append(QStringLiteral(
            "Field '%1' has invalid access '%2' for address in indirect interface %3 within %4").arg(
            indirectInterface->getIndirectAddressRef(), AccessTypes::access2Str(targetField->getAccess()),
            indirectInterface->name(), indirectInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInDataReference()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInDataReference(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    if (indirectInterface->getIndirectDataRef().isEmpty())
    {
        errors.append(QStringLiteral("No field specified for data in indirect interface '%1' within %2")
            .arg(indirectInterface->name(), context));
    }
    else if (!findField(indirectInterface->getIndirectDataRef()))
    {
        errors.append(QStringLiteral(
            "Field '%1' not found for data in indirect interface '%2' within %3").arg(
            indirectInterface->getIndirectDataRef(), indirectInterface->name(), context));        
    }
    else if (memoryMapContainsField(indirectInterface->getMemoryMapRef(), indirectInterface->getIndirectDataRef()))
    {
        errors.append(QStringLiteral(
            "Field '%1' is defined within indirect memory map '%2' in indirect interface %3 within %4").arg(
            indirectInterface->getIndirectDataRef(), indirectInterface->getMemoryMapRef(), 
            indirectInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInMemoryReference()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInMemoryReference(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    bool hasMemoryMap = !indirectInterface->getMemoryMapRef().isEmpty();
    bool hasBridges = !indirectInterface->getTransparentBridges()->isEmpty();

    if (hasMemoryMap && hasBridges)
    {
        errors.append(QStringLiteral(
            "Both memory map and transparent bridges defined for indirect interface '%1' within %2.").arg(
            indirectInterface->name(), context));
    }
    else if (!hasMemoryMap && !hasBridges)
    {
        errors.append(QStringLiteral(
            "No memory map or transparent bridges defined for indirect interface '%1' within %2.").arg(
            indirectInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInMemoryMap()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInMemoryMap(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    if (!hasValidMemoryMapReference(indirectInterface))
    {
        errors.append(QStringLiteral(
            "Invalid memory map '%1' referenced in indirect interface '%2' within %3.").arg(
            indirectInterface->getMemoryMapRef(), indirectInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInTransparentBridges()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInTransparentBridges(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    for (QSharedPointer<TransparentBridge> const& bridge : *indirectInterface->getTransparentBridges())
    {
        bool foundInterface = false;
        for (QSharedPointer<BusInterface> const& busInterface : *component_->getBusInterfaces())
        {
            if (busInterface->name().compare(bridge->getMasterRef()) == 0)
            {
                foundInterface = true;
                if (busInterface->getInterfaceMode() != General::MASTER)
                {
                    errors.append(QStringLiteral(
                        "Transparent bridge references a non-master bus interface '%1' in indirect interface '%2' within %3.").arg(
                        bridge->getMasterRef(), indirectInterface->name(), context));
                }
            }
        }

        if (!foundInterface)
        {
            errors.append(QStringLiteral(
                "Transparent bridge references an invalid bus interface '%1' in indirect interface '%2' within %3.").arg(
                bridge->getMasterRef(), indirectInterface->name(), context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInBitsInLau()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInBitsInLau(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    if (!hasValidBitsInLau(indirectInterface))
    {
        errors.append(QStringLiteral(
            "Invalid bits in lau '%1' defined in indirect interface '%2' within %3.").arg(
            indirectInterface->getBitsInLau(), indirectInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInEndianness()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInEndianness(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    if (!hasValidEndianness(indirectInterface))
    {
        errors.append(QStringLiteral(
            "Invalid endianness '%1' defined in indirect interface '%2' within %3.").arg(
            indirectInterface->getEndianness(), indirectInterface->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: IndirectInterfaceValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void IndirectInterfaceValidator::findErrorsInParameters(QVector<QString>& errors,
    QSharedPointer<IndirectInterface> indirectInterface, QString const& context) const
{
    QVector<QString> parameterNames;
    for (QSharedPointer<Parameter> const& parameter : *indirectInterface->getParameters())
    {
        if (parameterNames.contains(parameter->name()))
        {
            errors.append(QObject::tr("Parameter name %1 is not unique in indirect interface %2 within %3.").arg(
                parameter->name(), indirectInterface->name(), context));
        }
        else
        {
            parameterNames.append(parameter->name());
        }

        parameterValidator_->findErrorsIn(errors, parameter, context);
    }
}
