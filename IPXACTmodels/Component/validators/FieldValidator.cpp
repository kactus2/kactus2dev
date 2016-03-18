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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: FieldValidator::FieldValidator()
//-----------------------------------------------------------------------------
FieldValidator::FieldValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator,
    QSharedPointer<ParameterValidator2014> parameterValidator):
expressionParser_(expressionParser),
enumeratedValueValidator_(enumeratedValueValidator),
parameterValidator_(parameterValidator)
{

}

//-----------------------------------------------------------------------------
// Function: FieldValidator::~FieldValidator()
//-----------------------------------------------------------------------------
FieldValidator::~FieldValidator()
{

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
    return hasValidName(field) && hasValidIsPresent(field) && hasValidBitOffset(field) &&
        hasValidResetValue(field) && hasValidResetMask(field) && hasValidWriteValueConstraint(field) &&
        hasValidWriteValueConstraint(field) && hasValidReserved(field) && hasValidBitWidth(field) &&
        hasValidEnumeratedValues(field) && hasValidParameters(field) && hasValidAccess(field);
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidName()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidName(QSharedPointer<Field> field) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
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
// Function: FieldValidator::hasValidBitOffset()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidBitOffset(QSharedPointer<Field> field) const
{
    QString solvedValue = expressionParser_->parseExpression(field->getBitOffset());

    bool changeOk = true;
    quint64 intValue = solvedValue.toULongLong(&changeOk);

    if (intValue < 0 || !changeOk)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetValue()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetValue(QSharedPointer<Field> field) const
{
    if (!field->getResetValue().isEmpty())
    {
        return isBitExpressionValid(field->getResetValue());
    }
    else if (field->getResetValue().isEmpty() && !field->getResetMask().isEmpty())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::hasValidResetMask()
//-----------------------------------------------------------------------------
bool FieldValidator::hasValidResetMask(QSharedPointer<Field> field) const
{
    if (field->getResetMask().isEmpty())
    {
        return true;
    }
    else
    {
        return isBitExpressionValid(field->getResetMask());
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
        QString solvedValue = expressionParser_->parseExpression(field->getReserved());
        int intValue = solvedValue.toInt();

        if (!expressionParser_->isValidExpression(field->getReserved()) || intValue < 0 || intValue > 1)
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
        foreach (QSharedPointer<EnumeratedValue> enumeratedValue, *field->getEnumeratedValues())
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

        foreach (QSharedPointer<Parameter> parameter, *field->getParameters())
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
// Function: FieldValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Field> field, QString const& context)
    const
{
    findErrorsInName(errors, field, context);
    findErrorsInIsPresent(errors, field, context);
    findErrorsInBitOffset(errors, field, context);
    findErrorsInResetValue(errors, field, context);
    findErrorsInResetMask(errors, field, context);
    findErrorsInWriteValueConstraint(errors, field, context);
    findErrorsInReserved(errors, field, context);
    findErrorsInBitWidth(errors, field, context);
    findErrorsInEnumeratedValues(errors, field);
    findErrorsInParameters(errors, field);
    findErrorsInAccess(errors, field, context);
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
// Function: FieldValidator::findErrorsInResetValue()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResetValue(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidResetValue(field))
    {
        errors.append(QObject::tr("Invalid reset value set for field %1 within %2").arg(field->name()).
            arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldValidator::findErrorsInResetMask()
//-----------------------------------------------------------------------------
void FieldValidator::findErrorsInResetMask(QVector<QString>& errors, QSharedPointer<Field> field,
    QString const& context) const
{
    if (!hasValidResetMask(field))
    {
        errors.append(QObject::tr("Invalid reset mask set for field %1 within %2").arg(field->name()).arg(context));
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
        QString context = "field " + field->name();

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
        foreach (QSharedPointer<EnumeratedValue> enumeratedValue, *field->getEnumeratedValues())
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
        QString context = "field " + field->name();

        QStringList parameterNames;

        foreach (QSharedPointer<Parameter> parameter, *field->getParameters())
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
// Function: FieldValidator::validateBitExpression()
//-----------------------------------------------------------------------------
bool FieldValidator::isBitExpressionValid(QString const& expression) const
{
    QString solvedValue = expressionParser_->parseExpression(expression);
    
    QRegularExpression bitExpression("^([0-9]+|[1-9]+[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$");
    return bitExpression.match(expression).hasMatch() || bitExpression.match(solvedValue).hasMatch();
}