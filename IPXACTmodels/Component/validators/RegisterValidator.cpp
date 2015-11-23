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
//     QSharedPointer<ParameterFinder> finder,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices):
expressionParser_(expressionParser),
// parameterFinder_(finder),
availableChoices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::~RegisterValidator()
//-----------------------------------------------------------------------------
RegisterValidator::~RegisterValidator()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::validate()
//-----------------------------------------------------------------------------
bool RegisterValidator::validate(QSharedPointer<Register> selectedRegister) const
{
    return hasValidName(selectedRegister) && hasValidIsPresent(selectedRegister) &&
        hasValidDimension(selectedRegister) && hasValidAddressOffset(selectedRegister) &&
        hasValidSize(selectedRegister) && hasValidFields(selectedRegister) &&
        hasValidAlternateRegisters(selectedRegister) && hasValidParameters(selectedRegister) &&
        bitFieldsAreWithinRegister(selectedRegister->getFields(), selectedRegister->getSize());
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidName()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidName(QSharedPointer<RegisterBase> selectedRegister) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^ *$");
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
        int intValue = solvedValue.toInt(&toIntOk);

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
    int offsetInt = solvedValue.toInt(&changeOk);

    return changeOk && offsetInt >= 0;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidSize()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidSize(QSharedPointer<Register> selectedRegister) const
{
    QString solvedValue = expressionParser_->parseExpression(selectedRegister->getSize());
    bool changeOk = true;
    int sizeInt = solvedValue.toInt(&changeOk);

    return changeOk && sizeInt > 0;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidFields()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidFields(QSharedPointer<RegisterDefinition> selectedRegister) const
{
    if (selectedRegister->getFields()->isEmpty())
    {
        return false;
    }

    FieldValidator validator(expressionParser_, availableChoices_);

    QStringList fieldNames;

    for (int fieldIndex = 0; fieldIndex < selectedRegister->getFields()->size(); ++fieldIndex)
    {
        QSharedPointer<Field> field = selectedRegister->getFields()->at(fieldIndex);
        if (!validator.validate(field) || fieldNames.contains(field->name()))
        {
            return false;
        }
        else
        {
            fieldNames.append(field->name());

            int rangeBegin = expressionParser_->parseExpression(field->getBitOffset()).toInt();
            int rangeEnd = rangeBegin + expressionParser_->parseExpression(field->getBitWidth()).toInt() - 1;

            QString fieldTypeIdentifier = field->getTypeIdentifier();

            for (int comparisonIndex = fieldIndex + 1; comparisonIndex < selectedRegister->getFields()->size();
                ++comparisonIndex)
            {
                QSharedPointer<Field> comparedField = selectedRegister->getFields()->at(comparisonIndex);

                int comparedBegin = expressionParser_->parseExpression(comparedField->getBitOffset()).toInt();
                int comparedEnd = comparedBegin +
                    expressionParser_->parseExpression(comparedField->getBitWidth()).toInt() - 1;

                if (comparedBegin < rangeBegin && ((rangeBegin >= comparedBegin && rangeBegin <= comparedEnd) ||
                    (rangeEnd >= comparedBegin && rangeEnd <= comparedEnd)))
                {
                    return false;
                }
                else if ((comparedBegin >= rangeBegin && comparedBegin <= rangeEnd) ||
                    (comparedEnd >= rangeBegin && comparedEnd <= rangeEnd))
                {
                    return false;
                }
                
                if (!fieldTypeIdentifier.isEmpty() && fieldTypeIdentifier == comparedField->getTypeIdentifier())
                {
                    if (!fieldsHaveSimilarDefinitionGroups(field, comparedField))
                    {
                        return false;
                    }
                }
            }

            if (field->getVolatile().toBool() == false && selectedRegister->getVolatile() == QLatin1String("true"))
            {
                return false;
            }

            if (!fieldHasValidAccess(selectedRegister, field))
            {
                return false;
            }
        }
    }

    return true;
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
            !hasValidFields(alternateRegister) ||
            !hasValidParameters(alternateRegister) ||
            !bitFieldsAreWithinRegister(alternateRegister->getFields(), selectedRegister->getSize()))
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
        ParameterValidator2014 validator(expressionParser_, availableChoices_);
        QStringList parameterNames;
        foreach (QSharedPointer<Parameter> parameter, *selectedRegister->getParameters())
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
// Function: RegisterValidator::bitFieldsAreWithinRegister()
//-----------------------------------------------------------------------------
bool RegisterValidator::bitFieldsAreWithinRegister(QSharedPointer<QList<QSharedPointer<Field> > > fields,
    QString const& registerSize) const
{
    int registerSizeInt = expressionParser_->parseExpression(registerSize).toInt();

    foreach(QSharedPointer<Field> currentField, *fields)
    {
        int bitOffset = expressionParser_->parseExpression(currentField->getBitOffset()).toInt();
        int bitWidth = expressionParser_->parseExpression(currentField->getBitWidth()).toInt();

        if (bitOffset < 0 || bitOffset > registerSizeInt - bitWidth)
        {
            return false;
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
     if ((selectedRegsiter->getAccess() == General::READ_ONLY && field->getAccess() == General::READ_ONLY) ||
        (selectedRegsiter->getAccess() == General::WRITE_ONLY && (field->getAccess() == General::WRITE_ONLY ||
            field->getAccess() == General::WRITEONCE)) ||
        (selectedRegsiter->getAccess() == General::READ_WRITEONCE && (field->getAccess() == General::READ_ONLY ||
            field->getAccess() == General::READ_WRITEONCE || field->getAccess() == General::WRITEONCE)) ||
        (selectedRegsiter->getAccess() == General::WRITEONCE && field->getAccess() == General::WRITEONCE) ||
        selectedRegsiter->getAccess() == General::ACCESS_COUNT)
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
            QObject::tr("Invalid name specified for %1 within %2").arg(selectedRegister->name(), context));
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
        errors.append(QObject::tr("Invalid dimension set for %1 within %2").arg(selectedRegister->name()).
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
        FieldValidator validator(expressionParser_, availableChoices_);
        QStringList fieldNames;

        for (int fieldIndex = 0; fieldIndex < selectedRegister->getFields()->size(); ++fieldIndex)
        {
            QSharedPointer<Field> field = selectedRegister->getFields()->at(fieldIndex);
            validator.findErrorsIn(errors, field, context);

            if (fieldNames.contains(field->name()))
            {
                errors.append(
                    QObject::tr("Name %1 of fields in %2 is not unique.").arg(field->name()).arg(context));
            }
            else
            {
                fieldNames.append(field->name());

                int rangeBegin = expressionParser_->parseExpression(field->getBitOffset()).toInt();
                int rangeEnd = rangeBegin + expressionParser_->parseExpression(field->getBitWidth()).toInt() - 1;

                QString fieldTypeIdentifier = field->getTypeIdentifier();

                for (int comparisonIndex = fieldIndex + 1; comparisonIndex < selectedRegister->getFields()->size();
                    ++comparisonIndex)
                {
                    QSharedPointer<Field> comparedField = selectedRegister->getFields()->at(comparisonIndex);

                    int comparedBegin = expressionParser_->parseExpression(comparedField->getBitOffset()).toInt();
                    int comparedEnd = comparedBegin +
                        expressionParser_->parseExpression(comparedField->getBitWidth()).toInt() - 1;

                    if (comparedBegin < rangeBegin && ((rangeBegin >= comparedBegin && rangeBegin <= comparedEnd) ||
                        (rangeEnd >= comparedBegin && rangeEnd <= comparedEnd)))
                    {
                        errors.append(QObject::tr("Fields %1 and %2 are overlapping in %3")
                            .arg(field->name()).arg(comparedField->name()).arg(context));
                    }
                    else if ((comparedBegin >= rangeBegin && comparedBegin <= rangeEnd) ||
                        (comparedEnd >= rangeBegin && comparedEnd <= rangeEnd))
                    {
                        errors.append(QObject::tr("Fields %1 and %2 are overlapping in %3")
                            .arg(field->name()).arg(comparedField->name()).arg(context));
                    }

                    if (!fieldTypeIdentifier.isEmpty() && fieldTypeIdentifier == comparedField->getTypeIdentifier())
                    {
                        if (!fieldsHaveSimilarDefinitionGroups(field, comparedField))
                        {
                            errors.append(QObject::tr("Fields %1 and %2 have type identifier %3, but different "
                                "field definitions within %4").arg(field->name()).arg(comparedField->name())
                                .arg(fieldTypeIdentifier).arg(context));
                        }
                    }
                }

                int registerSizeInt = expressionParser_->parseExpression(registerSize).toInt();
                int bitOffset = expressionParser_->parseExpression(field->getBitOffset()).toInt();
                int bitWidth = expressionParser_->parseExpression(field->getBitWidth()).toInt();

                if (bitOffset < 0 || bitOffset > registerSizeInt - bitWidth)
                {
                    errors.append(QObject::tr("Field %1 is not contained within %2").arg(field->name()).
                        arg(selectedRegister->name()));
                }

                if (field->getVolatile().toBool() == false && selectedRegister->getVolatile() == "true")
                {
                    errors.append(QObject::tr("Volatile cannot be set to false in %1, where containing field %2 "
                        "has volatile true").arg(context).arg(field->name()));
                }

                if (!fieldHasValidAccess(selectedRegister, field))
                {
                    errors.append(QObject::tr("Access cannot be set to %1 in %2, where containing register %3 "
                        "has access %4").arg(General::access2Str(field->getAccess())).arg(field->name())
                        .arg(selectedRegister->name()).arg(General::access2Str(selectedRegister->getAccess())));
                }
            }
        }
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
        ParameterValidator2014 validator(expressionParser_, availableChoices_);
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

            validator.findErrorsIn(errors, parameter, context);
        }
    }
}
