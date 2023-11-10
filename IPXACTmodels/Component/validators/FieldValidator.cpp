//-----------------------------------------------------------------------------
// File: FieldValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.11.2015
//
// Description:
// Validator for ipxact:Field.
//-----------------------------------------------------------------------------

#include "FieldValidator.h"
#include "FieldAccessPolicyValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/validators/MemoryArrayValidator.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/FieldReset.h>
#include <IPXACTmodels/Component/ResetType.h>
#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: FieldValidator::FieldValidator()
//-----------------------------------------------------------------------------
FieldValidator::FieldValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator,
    QSharedPointer<ParameterValidator> parameterValidator,
    Document::Revision docRevision):
expressionParser_(expressionParser),
enumeratedValueValidator_(enumeratedValueValidator),
parameterValidator_(parameterValidator),
docRevision_(docRevision)
{

}

//-----------------------------------------------------------------------------
// Function: FieldValidator::componentChange()
//-----------------------------------------------------------------------------
void FieldValidator::componentChange(QSharedPointer<Component> newComponent)
{
    availableResetTypes_.clear();
    if (newComponent)
    {
        availableResetTypes_ = newComponent->getResetTypes();
        docRevision_ = newComponent->getRevision();
        componentModes_ = newComponent->getModes();
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::getEnumeratedValueValidator()
//-----------------------------------------------------------------------------
QSharedPointer<EnumeratedValueValidator> FieldValidator::getEnumeratedValueValidator() const
{
    return enumeratedValueValidator_;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::validate()
//-----------------------------------------------------------------------------
bool FieldValidator::validate(QSharedPointer<Field> field) const
{
    if (docRevision_ == Document::Revision::Std14)
    {
        return hasValidName(field) && hasValidIsPresent(field) && 
            hasValidBitOffset(field) && hasValidResets(field) &&
            hasValidWriteValueConstraint(field) && hasValidReserved(field) && hasValidBitWidth(field) &&
            hasValidEnumeratedValues(field) && hasValidParameters(field) && hasValidAccess(field);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        FieldAccessPolicyValidator fieldAccessPolicyValidator(expressionParser_);

        for (auto const& fieldAccessPolicy : *field->getFieldAccessPolicies())
        {
            if (!fieldAccessPolicyValidator.validate(fieldAccessPolicy, componentModes_))
            {
                return false;
            }
        }

        // Check mode refs of all field access policies. Priority and reference must both be unique between
        // all field access policies.
        if (!hasValidFieldAccessPolicyModeRefs(field))
        {
            return false;
        }

        return hasValidName(field) && hasValidMemoryArray(field) && hasValidBitOffset(field) && hasValidResets(field) &&
            hasValidWriteValueConstraint(field) && hasValidReserved(field) && hasValidBitWidth(field) &&
            hasValidEnumeratedValues(field) && hasValidParameters(field) && hasValidAccess(field) &&
            hasValidStructure(field);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidName()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidName(QSharedPointer<Field> field) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(field->name());

    if (field->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidIsPresent(QSharedPointer<Field> field) const
{
    if (!field->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(field->getIsPresent());

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
// Function: FieldValidator::hasValidMemoryArray()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidMemoryArray(QSharedPointer<Field> field) const
{
    if (auto memArray = field->getMemoryArray(); memArray)
    {
        MemoryArrayValidator validator(expressionParser_);
        return validator.validate(field->getMemoryArray());
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidBitOffset()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidBitOffset(QSharedPointer<Field> field) const
{
    QString solvedValue = expressionParser_->parseExpression(field->getBitOffset());

    bool changeOk = true;
    solvedValue.toULongLong(&changeOk);

    return changeOk;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetTypeReference()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetTypeReference(QSharedPointer<FieldReset> fieldReset) const
{
    if (fieldReset->getResetTypeReference().isEmpty())
    {
        return true;
    }

    else if (availableResetTypes_)
    {
        for (auto resetType : *availableResetTypes_)
        {
            if (resetType->name() == fieldReset->getResetTypeReference())
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetValue()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetValue(QSharedPointer<FieldReset> fieldReset) const
{
    if (fieldReset->getResetValue().isEmpty() == false)
    {
        return isBitExpressionValid(fieldReset->getResetValue());
    }
    else if (fieldReset->getResetValue().isEmpty() &&
        (!fieldReset->getResetTypeReference().isEmpty() || !fieldReset->getResetMask().isEmpty()))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetMask()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetMask(QSharedPointer<FieldReset> fieldReset) const
{
    if (fieldReset->getResetMask().isEmpty())
    {
        return true;
    }
    else
    {
        return isBitExpressionValid(fieldReset->getResetMask());
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidWriteValueConstraint()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidWriteValueConstraint(QSharedPointer<Field> field) const
{
    if (field->getWriteConstraint())
    {
        QSharedPointer<WriteValueConstraint> writeConstraint = field->getWriteConstraint();

        if (writeConstraint->getType() == WriteValueConstraint::TYPE_COUNT)
        {
            return false;
        }
        else if (writeConstraint->getType() == WriteValueConstraint::MIN_MAX)
        {
            int minimumInt = expressionParser_->parseExpression(writeConstraint->getMinimum()).toInt();
            int maximumInt = expressionParser_->parseExpression(writeConstraint->getMaximum()).toInt();

            return !writeConstraint->getMinimum().isEmpty() && !writeConstraint->getMaximum().isEmpty() &&
                isBitExpressionValid(writeConstraint->getMinimum()) &&
                isBitExpressionValid(writeConstraint->getMaximum()) &&
                minimumInt <= maximumInt;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidReserved()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidReserved(QSharedPointer<Field> field) const
{
    if (!field->getReserved().isEmpty())
    {
        bool isValidReserved = false;
        QString solvedValue = expressionParser_->parseExpression(field->getReserved(), &isValidReserved);
        bool canConvert = false;
        int intValue = solvedValue.toInt(&canConvert);

        if (isValidReserved == false || canConvert ==false || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidBitWidth()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidBitWidth(QSharedPointer<Field> field) const
{
    QString solvedValue = expressionParser_->parseExpression(field->getBitWidth());

    bool changeOk = true;
    quint64 intValue = solvedValue.toULongLong(&changeOk);

    if (changeOk && intValue > 0)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidEnumeratedValues()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidEnumeratedValues(QSharedPointer<Field> field) const
{
    if (!field->getEnumeratedValues()->isEmpty())
    {
        QStringList enumeratedValueNames;

        bool constraintUsesEnums = false;
        if (field->getWriteConstraint())
        {
            if (field->getWriteConstraint()->getType() == WriteValueConstraint::USE_ENUM)
            {
                constraintUsesEnums = true;
            }
        }

        bool enumerationWriteUsage = false;
        for (QSharedPointer<EnumeratedValue> const& enumeratedValue : *field->getEnumeratedValues())
        {
            if (!enumerationWriteUsage && constraintUsesEnums &&
                (enumeratedValue->getUsage() == EnumeratedValue::WRITE ||
                enumeratedValue->getUsage() == EnumeratedValue::READWRITE))
            {
                enumerationWriteUsage = true;
            }

            if (!enumeratedValueValidator_->validate(enumeratedValue) ||
                enumeratedValueNames.contains(enumeratedValue->name()))
            {
                return false;
            }
            else
            {
                enumeratedValueNames.append(enumeratedValue->name());
            }
        }

        if (constraintUsesEnums && !enumerationWriteUsage)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidParameters(QSharedPointer<Field> field) const
{
    if (!field->getParameters()->isEmpty())
    {
        QStringList parameterNames;

        for (QSharedPointer<Parameter> const& parameter : *field->getParameters())
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
// Function: FieldValidator::hasValidAccess()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidAccess(QSharedPointer<Field> field) const
{
    if (field->getAccess() == AccessTypes::READ_ONLY)
    {
        return field->getModifiedWrite() == General::MODIFIED_WRITE_COUNT;
    }
    else if (field->getAccess() == AccessTypes::WRITE_ONLY || field->getAccess() == AccessTypes::WRITEONCE)
    {
        return field->getReadAction() == General::READ_ACTION_COUNT;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidFieldDefinitionRef()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidFieldDefinitionRef(QSharedPointer<Field> field) const
{
    if (!field->getFieldDefinitionRef().isEmpty() && field->getTypeDefinitionsRef().isEmpty())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidStructure()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidStructure(QSharedPointer<Field> field) const
{
    if (field->getFieldDefinitionRef().isEmpty())
    {
        return hasValidReferenceResetChoice(field);
    }
    else
    {
        if (field->getTypeIdentifier().isEmpty() == false ||
            field->getFieldAccessPolicies()->isEmpty() == false ||
            field->getEnumeratedValues()->isEmpty() == false ||
            field->getBitWidth().isEmpty() == false ||
            field->getVolatile().toString() == QStringLiteral("true") ||
            field->getResets()->isEmpty() == false ||
            field->getFieldReference())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Field> field, QString const& context)
    const
{
    findErrorsInName(errors, field, context);
    findErrorsInIsPresent(errors, field, context);
    findErrorsInBitOffset(errors, field, context);
    findErrorsInResets(errors, field, context);
    findErrorsInEnumeratedValues(errors, field);
    findErrorsInParameters(errors, field);

    if (docRevision_ == Document::Revision::Std14)
    {
        findErrorsInWriteValueConstraint(errors, field, context);
        findErrorsInReserved(errors, field, context);
        findErrorsInAccess(errors, field, context);
        findErrorsInBitWidth(errors, field, context);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        auto newContext = QStringLiteral("field ") + field->name() + QStringLiteral(" within ") + context;

        findErrorsInMemoryArray(errors, field, newContext);
        findErrorsInFieldDefinitionRef(errors, field, context);

        FieldAccessPolicyValidator fieldAccessPolicyValidator(expressionParser_);
        
        for (auto const& fieldAccessPolicy : *field->getFieldAccessPolicies())
        {
            fieldAccessPolicyValidator.findErrorsIn(errors, fieldAccessPolicy, newContext, componentModes_);
        }

        findErrorsInModeRefs(errors, field, newContext);

        findErrorsInStructure(errors, field, context);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidName(field))
    {
        errors.append(QObject::tr("Invalid name specified for %1 within %2").arg(field->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidIsPresent(field))
    {
        errors.append(QObject::tr("Invalid isPresent value specified for %1 within %2. Value should evaluate to "
            "0 or 1.").
            arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInMemoryArray()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInMemoryArray(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    MemoryArrayValidator validator(expressionParser_);
    if (auto memArray = field->getMemoryArray(); memArray)
    {
        validator.findErrorsIn(errors, field->getMemoryArray(), context);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInBitOffset()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInBitOffset(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidBitOffset(field))
    {
        errors.append(QObject::tr("Invalid bit offset set for field %1 within %2").arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResets()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResets(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    QString resetContext = QStringLiteral("field %1 in %2").arg(field->name(), context);

    if (!hasValidResetsTypeReferences(field))
    {
        errors.append(QObject::tr("Multiple references to default reset type in %1.").arg(resetContext));
    }

    for (auto fieldReset : *field->getResets())
    {
        findErrorsInResetTypeReference(errors, fieldReset, resetContext);
        findErrorsInResetValue(errors, fieldReset, resetContext);
        findErrorsInResetMask(errors, fieldReset, resetContext);
    }
}


//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResetTypeReference()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResetTypeReference(QVector<QString>& errors,
    QSharedPointer<FieldReset> fieldReset, QString const& context) const
{
    if (!hasValidResetTypeReference(fieldReset))
    {
        errors.append(QObject::tr("Reset type '%1' referenced in %2 does not exist.")
            .arg(fieldReset->getResetTypeReference()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResetValue()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResetValue(QVector<QString>& errors, QSharedPointer<FieldReset> fieldReset,
    QString const& context) const
{
    if (!hasValidResetValue(fieldReset))
    {
        errors.append(QObject::tr("Invalid reset value set within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResetMask()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResetMask(QVector<QString>& errors, QSharedPointer<FieldReset> fieldReset,
    QString const& context) const
{
    if (!hasValidResetMask(fieldReset))
    {
        errors.append(QObject::tr("Invalid reset mask set within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInWriteValueConstraint(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidWriteValueConstraint(field))
    {
        QSharedPointer<WriteValueConstraint> writeConstaint = field->getWriteConstraint();
        if (writeConstaint->getType() == WriteValueConstraint::TYPE_COUNT)
        {
            errors.append(QObject::tr("Invalid write value constraint set for field %1 within %2").
                arg(field->name()).arg(context));
        }
        else
        {
            bool minimumIsValid = isBitExpressionValid(writeConstaint->getMinimum());
            bool maximumIsValid = isBitExpressionValid(writeConstaint->getMaximum());

            if (!minimumIsValid)
            {
                errors.append(QObject::tr(
                    "Invalid minimum value set for write value constraint in field %1 within %2").
                    arg(field->name()).arg(context));
            }
            if (!maximumIsValid)
            {
                errors.append(QObject::tr(
                    "Invalid maximum value set for write value constraint in field %1 within %2").
                    arg(field->name()).arg(context));
            }

            if (minimumIsValid && maximumIsValid)
            {
                int minimumInt = expressionParser_->parseExpression(writeConstaint->getMinimum()).toInt();
                int maximumInt = expressionParser_->parseExpression(writeConstaint->getMaximum()).toInt();

                if (minimumInt > maximumInt)
                {
                    errors.append(QObject::tr(
                        "Maximum value must be greater than or equal to the minimum value in write value "
                        "constraint set for field %1 within %2").arg(field->name()).arg(context));
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInReserved()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInReserved(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidReserved(field))
    {
        errors.append(QObject::tr("Invalid reserved set for field %1 within %2").arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInBitWidth()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInBitWidth(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidBitWidth(field))
    {
        errors.append(QObject::tr("Invalid bit width set for field %1 within %2").arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInEnumeratedValues()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInEnumeratedValues(QVector<QString>& errors, QSharedPointer<Field> field) const
{
    if (!field->getEnumeratedValues()->isEmpty())
    {
        QString context = QStringLiteral("field ") + field->name();

        bool useEnumerations = false;
        if (field->getWriteConstraint())
        {
            if (field->getWriteConstraint()->getType() == WriteValueConstraint::USE_ENUM)
            {
                useEnumerations = true;
            }
        }

        bool writeEnumerationWasFound = false;
        QStringList enumeratedValueNames;
        for (QSharedPointer<EnumeratedValue> const& enumeratedValue : *field->getEnumeratedValues())
        {
            enumeratedValueValidator_->findErrorsIn(errors, enumeratedValue, context);

            if (enumeratedValueNames.contains(enumeratedValue->name()))
            {
                errors.append(QObject::tr("Name %1 of enumerated values in %2 is not unique.").
                    arg(enumeratedValue->name()).arg(context));
            }
            else
            {
                enumeratedValueNames.append(enumeratedValue->name());
            }

            if (useEnumerations && !writeEnumerationWasFound &&
                (enumeratedValue->getUsage() == EnumeratedValue::WRITE ||
                enumeratedValue->getUsage() == EnumeratedValue::READWRITE))
            {
                writeEnumerationWasFound = true;
            }
        }

        if (useEnumerations && !writeEnumerationWasFound)
        {
            errors.append(QObject::tr("Write value constraint of Use enumerated values needs an enumerated value "
                "with usage of write or read-write. Such an enumerated value was not found in field %1")
                .arg(field->name()));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInParameters(QVector<QString>& errors, QSharedPointer<Field> field) const
{
    if (!hasValidParameters(field))
    {
        QString context = QStringLiteral("field ") + field->name();

        QStringList parameterNames;
        for (QSharedPointer<Parameter> const& parameter : *field->getParameters())
        {
            parameterValidator_->findErrorsIn(errors, parameter, context);

            if (parameterNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Name %1 of parameters in %2 is not unique.").arg(parameter->name()).
                    arg(context));
            }
            else
            {
                parameterNames.append(parameter->name());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInAccess()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInAccess(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (field->getAccess() == AccessTypes::READ_ONLY)
    {
        if (field->getModifiedWrite() != General::MODIFIED_WRITE_COUNT)
        {
            errors.append(QObject::tr("In field %1 within %2, access type readOnly does not allow a field "
                "to include a modified write value.").arg(field->name()).arg(context));
        }
    }
    else if (field->getAccess() == AccessTypes::WRITE_ONLY || field->getAccess() == AccessTypes::WRITEONCE)
    {
        if (field->getReadAction() != General::READ_ACTION_COUNT)
        {
            errors.append(QObject::tr("In field %1 within %2, access type write only and write once do not "
                "allow a field to include a read action value.").arg(field->name()).arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInFieldDefinitionRef()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInFieldDefinitionRef(QStringList& errors, QSharedPointer<Field> field, QString const& context) const
{
    if (!hasValidFieldDefinitionRef(field))
    {
        errors.append(QObject::tr("Field %1 within %2 has no typeDefinitions reference "
            "defined for field definition reference.").arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidReferenceResetChoice()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidReferenceResetChoice(QSharedPointer<Field> field) const
{
    // Field can include bit width, volatility and resets OR field reference (aliasOf).
    if ((field->getBitWidth().isEmpty() == false ||
        field->getVolatile().toString() == QStringLiteral("true") ||
        field->getResets()->isEmpty() == false) &&
        field->getFieldReference()) 
    {
        return false;
    }

    // Field must have either bit width OR a field reference defined.
    if (field->getBitWidth().isEmpty() && !field->getFieldReference())
    {
        return false;
    }
    
    // Check bit width validity, if present.
    if (field->getBitWidth().isEmpty() == false && hasValidBitWidth(field) == false)
    {
        return false;
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInStructure()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInStructure(QStringList& errors, QSharedPointer<Field> field, QString const& context) const
{
    if (field->getFieldDefinitionRef().isEmpty())
    {
        if (field->getFieldReference())
        {
            if (field->getBitWidth().isEmpty() == false || field->getVolatile().toBool() ||
                field->getResets()->isEmpty() == false)
            {
                errors.append(QObject::tr("In field %1 within %2: Bit width, volatility, and resets must not be defined"
                    " for a field containing a reference to another field.").arg(field->name()).arg(context));
            }
        }
        else
        {
            if (field->getBitWidth().isEmpty())
            {
                errors.append(QObject::tr("In field %1 within %2: Bit width must be defined when no field definition reference"
                    " or field reference is defined.").arg(field->name()).arg(context));
            }
        }
    }
    else
    {
        if (field->getTypeIdentifier().isEmpty() == false)
        {
            errors.append(QObject::tr("In field %1 within %2: Type identifier must not be defined for a field "
                "with a field definition reference.").arg(field->name()).arg(context));
        }

        if (field->getFieldAccessPolicies()->isEmpty() == false)
        {
            errors.append(QObject::tr("In field %1 within %2: No field access policies can be defined for a field "
                "with a field definition reference.").arg(field->name()).arg(context));
        }

        if (field->getEnumeratedValues()->isEmpty() == false)
        {
            errors.append(QObject::tr("In field %1 within %2: No enumerated values can be defined for a field "
                "with a field definition reference.").arg(field->name()).arg(context));
        }

        if (field->getBitWidth().isEmpty() == false)
        {
            errors.append(QObject::tr("In field %1 within %2: Bit width must not be defined for a field "
                "with a field definition reference.").arg(field->name()).arg(context));
        }

        if (field->getVolatile().toBool())
        {
            errors.append(QObject::tr("In field %1 within %2: Volatility must not be defined for a field "
                "with a field definition reference.").arg(field->name()).arg(context));
        }

        if (field->getResets()->isEmpty() == false)
        {
            errors.append(QObject::tr("In field %1 within %2: Field resets must not be defined for a field "
                "with a field definition reference.").arg(field->name()).arg(context));
        }

        if (field->getFieldReference())
        {
            errors.append(QObject::tr("In field %1 within %2: Field reference must not be defined for a field "
                "with a field definition reference.").arg(field->name()).arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInModeRefs()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInModeRefs(QStringList& errors, QSharedPointer<Field> field, QString const& context) const
{
    QStringList checkedModeReferences;
    QList<unsigned int> checkedModePriorities;

    bool duplicateRefErrorIssued = false;
    bool duplicatePriorityErrorIssued = false;
    bool hasAccessPolicyWithoutModeRef = false;

    QString fieldAccessPolicyContext = QStringLiteral("field access policies of ") + context;

    for (auto const& accessPolicy : *field->getFieldAccessPolicies())
    {
        if (accessPolicy->getModeReferences()->isEmpty())
        {
            hasAccessPolicyWithoutModeRef = true;
        }

        CommonItemsValidator::findErrorsInModeRefs(errors, accessPolicy->getModeReferences(), 
            fieldAccessPolicyContext, checkedModeReferences, checkedModePriorities, 
            &duplicateRefErrorIssued, &duplicatePriorityErrorIssued, componentModes_);
    }

    if (hasAccessPolicyWithoutModeRef && field->getFieldAccessPolicies()->size() > 1)
    {
        errors.append(QObject::tr("In field %1 in %2, multiple field access policies are not allowed, if one "
            "of them lacks a mode reference.").arg(field->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResets()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResets(QSharedPointer<Field> field) const
{
    if (!hasValidResetsTypeReferences(field))
    {
        return false;
    }

    for (auto fieldReset : *field->getResets())
    {
        if (!hasValidResetValue(fieldReset) || !hasValidResetMask(fieldReset) ||
            !hasValidResetTypeReference(fieldReset))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetsTypeReferences()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetsTypeReferences(QSharedPointer<Field> field) const
{
    for (int i = 0; i < field->getResets()->size(); ++i)
    {
        QSharedPointer<FieldReset> fieldReset = field->getResets()->at(i);
        if (fieldReset->getResetTypeReference().isEmpty())
        {
            for (int j = i + 1; j < field->getResets()->size(); ++j)
            {
                if (field->getResets()->at(j)->getResetTypeReference().isEmpty())
                {
                    return false;
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::validateBitExpression()
//-----------------------------------------------------------------------------
bool FieldValidator::isBitExpressionValid(QString const& expression) const
{
    QString solvedValue = expressionParser_->parseExpression(expression);

    QRegularExpression bitExpression(QStringLiteral("^([0-9]+|[1-9]+[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$"));
    return bitExpression.match(expression).hasMatch() || bitExpression.match(solvedValue).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidAccessPolicyModeRefs()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidFieldAccessPolicyModeRefs(QSharedPointer<Field> field) const
{

    bool hasAccessPolicyWithoutModeRef = false;
    
    auto allModeRefs = QSharedPointer<QList<QSharedPointer<ModeReference> > >(
        new QList<QSharedPointer<ModeReference> >());

    for (auto const& accessPolicy : *field->getFieldAccessPolicies())
    {
        auto modeRefs = accessPolicy->getModeReferences();
        if (modeRefs->isEmpty())
        {
            hasAccessPolicyWithoutModeRef = true;
        }

        std::for_each(modeRefs->cbegin(), modeRefs->cend(), [&allModeRefs](auto modeRef)
            {
                allModeRefs->append(modeRef);
            });
    }

    if (!CommonItemsValidator::hasValidModeRefs(allModeRefs, componentModes_))
    {
        return false;
    }
    
    // Number of field access policies cannot be greater than one if a field access policy has no mode references.
    if (hasAccessPolicyWithoutModeRef && field->getFieldAccessPolicies()->size() > 1)
    {
        return false;
    }

    return true;
}
