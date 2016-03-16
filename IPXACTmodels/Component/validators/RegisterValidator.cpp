//-----------------------------------------------------------------------------
// File: RegisterValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.11.2015
//
// Description:
// Validator for ipxact:Register.
//-----------------------------------------------------------------------------

#include "RegisterValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/Component/validators/MemoryReserve.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/RegisterDefinition.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/AlternateRegister.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: RegisterValidator::RegisterValidator()
//-----------------------------------------------------------------------------
RegisterValidator::RegisterValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<FieldValidator> fieldValidator, QSharedPointer<ParameterValidator2014> parameterValidator):
expressionParser_(expressionParser),
fieldValidator_(fieldValidator),
parameterValidator_(parameterValidator)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::~RegisterValidator()
//-----------------------------------------------------------------------------
RegisterValidator::~RegisterValidator()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::getFieldValidator()
//-----------------------------------------------------------------------------
QSharedPointer<FieldValidator> RegisterValidator::getFieldValidator() const
{
    return fieldValidator_;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::validate()
//-----------------------------------------------------------------------------
bool RegisterValidator::validate(QSharedPointer<Register> selectedRegister) const
{
    return hasValidName(selectedRegister) && hasValidIsPresent(selectedRegister) &&
        hasValidDimension(selectedRegister) && hasValidAddressOffset(selectedRegister) &&
        hasValidSize(selectedRegister) && hasValidFields(selectedRegister, selectedRegister->getSize()) &&
        hasValidAlternateRegisters(selectedRegister) && hasValidParameters(selectedRegister);
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidName()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidName(QSharedPointer<RegisterBase> selectedRegister) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(selectedRegister->name());

    if (selectedRegister->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidIsPresent(QSharedPointer<RegisterBase> selectedRegister) const
{
    if (!selectedRegister->getIsPresent().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(selectedRegister->getIsPresent());

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
// Function: RegisterValidator::hasValidDimension()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidDimension(QSharedPointer<Register> selectedRegister) const
{
    if (!selectedRegister->getDimension().isEmpty())
    {
        QString solvedValue = expressionParser_->parseExpression(selectedRegister->getDimension());

        bool toIntOk = true;
        quint64 intValue = solvedValue.toULongLong(&toIntOk);

        if (!toIntOk || intValue < 0)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidAddressOffset()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidAddressOffset(QSharedPointer<Register> selectedRegister) const
{
    QString solvedValue = expressionParser_->parseExpression(selectedRegister->getAddressOffset());
    bool changeOk = true;
    quint64 offsetInt = solvedValue.toULongLong(&changeOk);

    return changeOk && offsetInt >= 0;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidSize()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidSize(QSharedPointer<Register> selectedRegister) const
{
    QString solvedValue = expressionParser_->parseExpression(selectedRegister->getSize());
    bool changeOk = true;
    quint64 sizeInt = solvedValue.toULongLong(&changeOk);

    return changeOk && sizeInt > 0;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidFields()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidFields(QSharedPointer<RegisterDefinition> selectedRegister,
    QString const& registerSize) const
{
    if (selectedRegister->getFields()->isEmpty())
    {
        return false;
    }

    qint64 registerSizeInt = expressionParser_->parseExpression(registerSize).toLongLong();

    MemoryReserve reservedArea;

    QVector<QString> fieldNames;
    QVector<QString> fieldTypeIdentifiers;
    for (int fieldIndex = 0; fieldIndex < selectedRegister->getFields()->size(); ++fieldIndex)
    {
        QSharedPointer<Field> field = selectedRegister->getFields()->at(fieldIndex);
        if (!fieldValidator_->validate(field) || fieldNames.contains(field->name()))
        {
            return false;
        }
        else
        {
            quint64 bitWidth = expressionParser_->parseExpression(field->getBitWidth()).toLongLong();

            qint64 rangeBegin = expressionParser_->parseExpression(field->getBitOffset()).toLongLong();
            qint64 rangeEnd = rangeBegin + bitWidth - 1;

            qint64 fieldEndPosition = registerSizeInt - bitWidth;

            if (rangeBegin < 0 || rangeBegin > fieldEndPosition)
            {
                return false;
            }

            reservedArea.addArea(field->name(), rangeBegin, rangeEnd);

            if (!field->getTypeIdentifier().isEmpty() && fieldTypeIdentifiers.contains(field->getTypeIdentifier()))
            {
                int typeIdIndex = fieldTypeIdentifiers.indexOf(field->getTypeIdentifier());

                QSharedPointer<Field> comparedField = selectedRegister->getFields()->at(typeIdIndex);
                if (!fieldsHaveSimilarDefinitionGroups(field, comparedField))
                {
                    return false;
                }
            }

            fieldNames.append(field->name());
            fieldTypeIdentifiers.append(field->getTypeIdentifier());

            if (field->getVolatile().toBool() == true && selectedRegister->getVolatile() == QLatin1String("false"))
            {
                return false;
            }

            if (!fieldHasValidAccess(selectedRegister, field))
            {
                return false;
            }
        }
    }

    return !reservedArea.hasOverlap();
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidAlternateRegisters()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidAlternateRegisters(QSharedPointer<Register> selectedRegister) const
{
    QStringList alternateRegisterNames;
    foreach (QSharedPointer<AlternateRegister> alternateRegister, *selectedRegister->getAlternateRegisters())
    {
        if (alternateRegisterNames.contains(alternateRegister->name()) ||
            !hasValidName(alternateRegister) ||
            !hasValidIsPresent(alternateRegister) ||
            !hasValidAlternateGroups(alternateRegister) ||
            !hasValidFields(alternateRegister, selectedRegister->getSize()) ||
            !hasValidParameters(alternateRegister))
        {
            return false;
        }
        else
        {
            alternateRegisterNames.append(alternateRegister->name());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidAlternateGroups()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidAlternateGroups(QSharedPointer<AlternateRegister> selectedRegister) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
        
    QStringList alternateGroups;
    bool alternateGroupsOk = false;
    foreach (QString group, *selectedRegister->getAlternateGroups())
    {
        if (group.isEmpty() || whiteSpaceExpression.match(group).hasMatch() || alternateGroups.contains(group))
        {
            return false;
        }
        else
        {
            alternateGroups.append(group);
            alternateGroupsOk = true;
        }
    }

    return alternateGroupsOk;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidParameters(QSharedPointer<RegisterBase> selectedRegister) const
{
    if (!selectedRegister->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *selectedRegister->getParameters())
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
// Function: RegisterValidator::fieldHasValidAccess()
//-----------------------------------------------------------------------------
bool RegisterValidator::fieldHasValidAccess(QSharedPointer<RegisterDefinition> selectedRegsiter,
    QSharedPointer<Field> field) const
{
    AccessTypes::Access registerAccess = selectedRegsiter->getAccess();
    AccessTypes::Access fieldAccess = field->getAccess();

     if ((registerAccess == AccessTypes::READ_ONLY && fieldAccess == AccessTypes::READ_ONLY) ||
        (registerAccess == AccessTypes::WRITE_ONLY && (fieldAccess == AccessTypes::WRITE_ONLY ||
            fieldAccess == AccessTypes::WRITEONCE)) ||
        (registerAccess == AccessTypes::READ_WRITEONCE && (fieldAccess == AccessTypes::READ_ONLY ||
            fieldAccess == AccessTypes::READ_WRITEONCE || fieldAccess == AccessTypes::WRITEONCE)) ||
        (registerAccess == AccessTypes::WRITEONCE && fieldAccess == AccessTypes::WRITEONCE) ||
        registerAccess == AccessTypes::ACCESS_COUNT ||
        registerAccess == AccessTypes::READ_WRITE)
     {
         return true;
     }

     return false;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::fieldsHaveSimilarDefinitionGroup()
//-----------------------------------------------------------------------------
bool RegisterValidator::fieldsHaveSimilarDefinitionGroups(QSharedPointer<Field> field,
    QSharedPointer<Field> comparedField) const
{
    int foundComparisons = 0;
    foreach (QSharedPointer<EnumeratedValue> enumeratedValue, *field->getEnumeratedValues())
    {
        foreach (QSharedPointer<EnumeratedValue> comparedEnumeratedValue, *comparedField->getEnumeratedValues())
        {
            if (enumeratedValue->name() == comparedEnumeratedValue->name() && 
                enumeratedValue->displayName() == comparedEnumeratedValue->displayName() &&
                enumeratedValue->description() == comparedEnumeratedValue->description() &&
                enumeratedValue->getValue() == comparedEnumeratedValue->getValue() &&
                enumeratedValue->getVendorExtensions()->size() ==
                comparedEnumeratedValue->getVendorExtensions()->size())
            {
                ++foundComparisons;
            }
        }
    }
    if (foundComparisons != field->getEnumeratedValues()->size() ||
        foundComparisons != comparedField->getEnumeratedValues()->size())
    {
        return false;
    }

    if (field->getWriteConstraint() && comparedField->getWriteConstraint())
    {
        if (field->getWriteConstraint()->getType() != comparedField->getWriteConstraint()->getType() ||
            field->getWriteConstraint()->getMinimum() != comparedField->getWriteConstraint()->getMinimum() ||
            field->getWriteConstraint()->getMaximum() != comparedField->getWriteConstraint()->getMaximum())
        {
            return false;
        }
    }
    else if (field->getWriteConstraint() || comparedField->getWriteConstraint())
    {
        return false;
    }

    return field->getBitWidth() == comparedField->getBitWidth() &&
        field->getVolatile().toString() == comparedField->getVolatile().toString() &&
        field->getAccess() == comparedField->getAccess() &&
        field->getModifiedWrite() == comparedField->getModifiedWrite() &&
        field->getModifiedWriteModify() == comparedField->getModifiedWriteModify() &&
        field->getReadAction() == comparedField->getReadAction() &&
        field->getReadActionModify() == comparedField->getReadActionModify() &&
        field->getTestable().toString() == comparedField->getTestable().toString() &&
        field->getTestConstraint() == comparedField->getTestConstraint() &&
        field->getReserved() == comparedField->getReserved();
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Register> selectedRegister,
    QString const& context) const
{
    QString registerContext = "register " + selectedRegister->name();

    findErrorsInName(errors, selectedRegister, context);
    findErrorsInIsPresent(errors, selectedRegister, context);
    findErrorsInDimension(errors, selectedRegister, context);
    findErrorsInAddressOffset(errors, selectedRegister, context);
    findErrorsInSize(errors, selectedRegister, context);
    findErrorsInFields(errors, selectedRegister, selectedRegister->getSize(), registerContext);
    findErrorsInAlternateRegisters(errors, selectedRegister);
    findErrorsInParameters(errors, selectedRegister, registerContext);
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<RegisterBase> selectedRegister,
    QString const& context) const
{
    if (!hasValidName(selectedRegister))
    {
        errors.append(
            QObject::tr("Invalid name specified for register '%1' within %2")
            .arg(selectedRegister->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInIsPresent(QVector<QString>& errors,
    QSharedPointer<RegisterBase> selectedRegister, QString const& context) const
{
    if (!hasValidIsPresent(selectedRegister))
    {
        errors.append(QObject::tr("Invalid isPresent set for %1 within %2").arg(selectedRegister->name()).
            arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInDimension()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInDimension(QVector<QString>& errors, QSharedPointer<Register> selectedRegister,
    QString const& context) const
{
    if (!hasValidDimension(selectedRegister))
    {
        errors.append(QObject::tr("Invalid dimension set for register %1 within %2").arg(selectedRegister->name()).
            arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInAddressOffset()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInAddressOffset(QVector<QString>& errors,
    QSharedPointer<Register> selectedRegister, QString const& context) const
{
    if (!hasValidAddressOffset(selectedRegister))
    {
        errors.append(QObject::tr("Invalid address offset set for register %1 within %2")
            .arg(selectedRegister->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInSize()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInSize(QVector<QString>& errors, QSharedPointer<Register> selectedRegister,
    QString const& context) const
{
    if (!hasValidSize(selectedRegister))
    {
        errors.append(QObject::tr("Invalid size specified for register %1 within %2").
            arg(selectedRegister->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInFields()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInFields(QVector<QString>& errors,
    QSharedPointer<RegisterDefinition> selectedRegister, QString const& registerSize, QString const& context) const
{
    if (!selectedRegister->getFields()->isEmpty())
    {
        QStringList fieldNames;
        QStringList fieldTypeIdentifiers;

        qint64 registerSizeInt = expressionParser_->parseExpression(registerSize).toLongLong();
        MemoryReserve reservedArea;

        foreach (QSharedPointer<Field> field, *selectedRegister->getFields())
        {
            fieldValidator_->findErrorsIn(errors, field, context);

            if (fieldNames.contains(field->name()))
            {
                errors.append(
                    QObject::tr("Name %1 of fields in %2 is not unique.").arg(field->name()).arg(context));
            }

            qint64 bitWidth = expressionParser_->parseExpression(field->getBitWidth()).toLongLong();

            qint64 rangeBegin = expressionParser_->parseExpression(field->getBitOffset()).toLongLong();
            qint64 rangeEnd = rangeBegin + bitWidth - 1;

            qint64 fieldEndPosition = registerSizeInt - bitWidth;

            if (rangeBegin < 0 || rangeBegin > fieldEndPosition)
            {
                errors.append(QObject::tr("Field %1 is not contained within %2").arg(field->name()).
                    arg(selectedRegister->name()));
            }

            reservedArea.addArea(field->name(), rangeBegin, rangeEnd);

            if (!field->getTypeIdentifier().isEmpty() && fieldTypeIdentifiers.contains(field->getTypeIdentifier()))
            {
                int typeIdIndex = fieldTypeIdentifiers.indexOf(field->getTypeIdentifier());
                QSharedPointer<Field> comparedField = selectedRegister->getFields()->at(typeIdIndex);
                if (!fieldsHaveSimilarDefinitionGroups(field, comparedField))
                {
                    errors.append(QObject::tr("Fields %1 and %2 have type identifier %3, but different "
                        "field definitions within %4").arg(comparedField->name()).arg(field->name())
                        .arg(field->getTypeIdentifier()).arg(context));
                }
            }

            fieldNames.append(field->name());
            fieldTypeIdentifiers.append(field->getTypeIdentifier());

            if (field->getVolatile().toBool() == true && selectedRegister->getVolatile() == QLatin1String("false"))
            {
                errors.append(QObject::tr("Volatile cannot be set to false in %1, where contained field %2 "
                    "has volatile true").arg(context).arg(field->name()));
            }

            if (!fieldHasValidAccess(selectedRegister, field))
            {
                errors.append(QObject::tr("Access cannot be set to %1 in field %2, where containing register %3 "
                    "has access %4").arg(AccessTypes::access2Str(field->getAccess())).arg(field->name())
                    .arg(selectedRegister->name()).arg(AccessTypes::access2Str(selectedRegister->getAccess())));
            }
        }

        reservedArea.findErrorsInOverlap(errors, QLatin1String("Fields"), context);
    }
    else
    {
        errors.append(QObject::tr("Register %1 must contain at least one field").arg(selectedRegister->name()));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInAlternateRegisters()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInAlternateRegisters(QVector<QString>& errors,
    QSharedPointer<Register> selectedRegister) const
{
    QString context = "register " + selectedRegister->name();
    QStringList alternateGroupNames;
    foreach (QSharedPointer<AlternateRegister> alternateRegister, *selectedRegister->getAlternateRegisters())
    {
        if (alternateGroupNames.contains(alternateRegister->name()))
        {
            errors.append(QObject::tr("Name %1 of alternate registers in %2 is not unique.").
                arg(alternateRegister->name()).arg("register " + selectedRegister->name()));
        }
        else
        {
            alternateGroupNames.append(alternateRegister->name());
        }

        QString registerContext = "alternate register " + alternateRegister->name();

        findErrorsInName(errors, alternateRegister, context);
        findErrorsInIsPresent(errors, alternateRegister, context);
        findErrorsInAlternateGroups(errors, alternateRegister, context);
        findErrorsInFields(errors, alternateRegister, selectedRegister->getSize(), registerContext);
        findErrorsInParameters(errors, alternateRegister, registerContext);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInAlternateGroups()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInAlternateGroups(QVector<QString>& errors,
    QSharedPointer<AlternateRegister> selectedRegister, QString const& context) const
{
    if (!hasValidAlternateGroups(selectedRegister))
    {
        errors.append(QObject::tr("Alternate groups are not unique or not empty in %1 within %2").
            arg(selectedRegister->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInParameters(QVector<QString>&errors,
    QSharedPointer<RegisterBase> selectedRegister, QString const& context) const
{
    if (!selectedRegister->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *selectedRegister->getParameters())
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

            parameterValidator_->findErrorsIn(errors, parameter, context);
        }
    }
}
