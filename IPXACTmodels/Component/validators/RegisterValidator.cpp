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

#include <KactusAPI/include/ExpressionParser.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/Component/validators/MemoryReserve.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/MemoryArrayValidator.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/RegisterDefinition.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/AlternateRegister.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: RegisterValidator::RegisterValidator()
//-----------------------------------------------------------------------------
RegisterValidator::RegisterValidator(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<FieldValidator> fieldValidator, QSharedPointer<ParameterValidator> parameterValidator,
    Document::Revision docRevision) :
    RegisterBaseValidator(expressionParser, parameterValidator, docRevision),
    fieldValidator_(fieldValidator)
{

}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::componentChange()
//-----------------------------------------------------------------------------
void RegisterValidator::componentChange(QSharedPointer<Component> newComponent)
{
    fieldValidator_->componentChange(newComponent);

    if (newComponent)
    {
        docRevision_ = newComponent->getRevision();
    }
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
    return RegisterBaseValidator::validate(selectedRegister) &&
           hasValidSize(selectedRegister) &&
           hasValidFields(selectedRegister, selectedRegister->getSize()) &&
           hasValidAlternateRegisters(selectedRegister) &&
           hasValidStructure(selectedRegister);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------// Function: RegisterValidator::hasValidSize()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidSize(QSharedPointer<Register> selectedRegister) const
{
    bool changeOk = true;
    bool expressionValid = false;

    QString solvedValue = expressionParser_->parseExpression(selectedRegister->getSize(), &expressionValid);
    quint64 sizeInt = solvedValue.toULongLong(&changeOk);

    return changeOk && expressionValid && sizeInt != 0;
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


            if(field->getIsPresent().isEmpty() || expressionParser_->parseExpression(field->getIsPresent()).toInt())
            {
              reservedArea.addArea(field->name(), rangeBegin, rangeEnd);
            }

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

            if (docRevision_ == Document::Revision::Std14 && !fieldHasValidAccess(selectedRegister, field))
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
    bool shouldResetCheckedModeReferences = true;
    QStringList alternateRegisterNames;

    QStringList checkedModeReferences;
    QStringList checkedModePriorities;

    for (QSharedPointer<AlternateRegister> alternateRegister : *selectedRegister->getAlternateRegisters())
    {
        if (alternateRegisterNames.contains(alternateRegister->name()))
        {
            return false;
        }
        else
        {
            alternateRegisterNames.append(alternateRegister->name());
        }

        if (!alternateRegisterIsValid(alternateRegister, selectedRegister))
        {
            return false;
        }

        if (!CommonItemsValidator::hasValidModeRefs(alternateRegister->getModeReferences(), 
            checkedModeReferences, checkedModePriorities))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::hasValidAlternateGroups()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidAlternateGroups(QSharedPointer<AlternateRegister> selectedRegister) const
{
    if (selectedRegister->getAlternateGroups()->isEmpty() && docRevision_ == Document::Revision::Std22)
    {
        return false;
    }

    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));

    QStringList alternateGroups;
    bool alternateGroupsOk = false;
    for (QString const& group : *selectedRegister->getAlternateGroups())
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
// Function: RegisterValidator::hasValidStructure()
//-----------------------------------------------------------------------------
bool RegisterValidator::hasValidStructure(QSharedPointer<Register> selectedRegister) const
{
    if (docRevision_ == Document::Revision::Std22)
    {
        // Either definition reference or register definition group can be defined.
        if (selectedRegister->getRegisterDefinitionReference().isEmpty() == false &&
            (!selectedRegister->getTypeIdentifier().isEmpty() || !selectedRegister->getSize().isEmpty() ||
                !selectedRegister->getVolatile().isEmpty() || !selectedRegister->getAccessPolicies()->isEmpty() ||
                !selectedRegister->getFields()->isEmpty()))
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
    AccessTypes::Access registerAccess = selectedRegsiter->getAccess();
    AccessTypes::Access fieldAccess = field->getAccess();

     if ((registerAccess == AccessTypes::READ_ONLY && fieldAccess == AccessTypes::READ_ONLY) ||
        (registerAccess == AccessTypes::WRITE_ONLY && (fieldAccess == AccessTypes::WRITE_ONLY ||
            fieldAccess == AccessTypes::WRITEONCE)) ||
        (registerAccess == AccessTypes::READ_WRITEONCE && (fieldAccess == AccessTypes::READ_ONLY ||
            fieldAccess == AccessTypes::READ_WRITEONCE || fieldAccess == AccessTypes::WRITEONCE)) ||
        (registerAccess == AccessTypes::WRITEONCE && fieldAccess == AccessTypes::WRITEONCE) ||
        registerAccess == AccessTypes::ACCESS_COUNT || fieldAccess == AccessTypes::ACCESS_COUNT ||
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
    for (QSharedPointer<EnumeratedValue> enumeratedValue : *field->getEnumeratedValues())
    {
        for (QSharedPointer<EnumeratedValue> comparedEnumeratedValue : *comparedField->getEnumeratedValues())
        {
            if (enumeratedValue->name() == comparedEnumeratedValue->name() &&
                enumeratedValue->displayName() == comparedEnumeratedValue->displayName() &&
                enumeratedValue->shortDescription() == comparedEnumeratedValue->shortDescription() &&
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
    QString registerContext = QStringLiteral("register ") + selectedRegister->name();
    QString completeRegisterContext = QStringLiteral("register '") + selectedRegister->name() + QStringLiteral("' within ") + context;

    if (docRevision_ == Document::Revision::Std14)
    {
        findErrorsInName(errors, selectedRegister, completeRegisterContext);
        findErrorsInIsPresent(errors, selectedRegister, completeRegisterContext);
        findErrorsInDimension(errors, selectedRegister, completeRegisterContext);
        findErrorsInAddressOffset(errors, selectedRegister, completeRegisterContext);
        findErrorsInSize(errors, selectedRegister, completeRegisterContext);
        findErrorsInFields(errors, selectedRegister, selectedRegister->getSize(), registerContext);
        findErrorsInAlternateRegisters(errors, selectedRegister);
        findErrorsInParameters(errors, selectedRegister, registerContext);
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        findErrorsInName(errors, selectedRegister, completeRegisterContext);
        findErrorsInMemoryArray(errors, selectedRegister, completeRegisterContext);
        findErrorsInAddressOffset(errors, selectedRegister, completeRegisterContext);
        findErrorsInSize(errors, selectedRegister, completeRegisterContext);
        findErrorsInAccessPolicies(errors, selectedRegister, completeRegisterContext);
        findErrorsInFields(errors, selectedRegister, selectedRegister->getSize(), registerContext);
        findErrorsInAlternateRegisters(errors, selectedRegister);
        if (!hasValidStructure(selectedRegister))
        {
            errors.append(QObject::tr("Register %1 in %2 contains both a register definition reference and register definition values.").arg(selectedRegister->name()).arg(context));
        }
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
        errors.append(QObject::tr("Invalid size specified for %1").arg(context));
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

        for (auto const& field : *selectedRegister->getFields())
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

            if(field->getIsPresent().isEmpty() || expressionParser_->parseExpression(field->getIsPresent()).toInt())
            {
              reservedArea.addArea(field->name(), rangeBegin, rangeEnd);
            }

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

            if (docRevision_ == Document::Revision::Std14)
            {
                if (!fieldHasValidAccess(selectedRegister, field))
                {
                    errors.append(QObject::tr("Access cannot be set to %1 in field %2, where containing register %3 "
                        "has access %4").arg(AccessTypes::access2Str(field->getAccess())).arg(field->name())
                        .arg(selectedRegister->name()).arg(AccessTypes::access2Str(selectedRegister->getAccess())));
                }
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
    QString context = QStringLiteral("register ") + selectedRegister->name();
    QStringList alternateGroupNames;
    
    // Keep track of checked mode references.
    bool duplicateRefErrorIssued = false;
    bool duplicatePriorityErrorIssued = false;

    QStringList checkedModeReferences;
    QStringList checkedModePriorities;

    for (QSharedPointer<AlternateRegister> alternateRegister : *selectedRegister->getAlternateRegisters())
    {
        if (alternateGroupNames.contains(alternateRegister->name()))
        {
            errors.append(QObject::tr("Name %1 of alternate registers in %2 is not unique.").arg(
                alternateRegister->name(), context));
        }
        else
        {
            alternateGroupNames.append(alternateRegister->name());
        }

        QString registerContext = QStringLiteral("alternate register ") + alternateRegister->name() + QStringLiteral(" within ") + context;

        findErrorsInName(errors, alternateRegister, registerContext);
        
        if (docRevision_ == Document::Revision::Std14)
        {
            findErrorsInIsPresent(errors, alternateRegister, registerContext);
            findErrorsInAlternateGroups(errors, alternateRegister, registerContext);
        }
        else if (docRevision_ == Document::Revision::Std22)
        {
            findErrorsInAlternateRegisterModeRefs(errors, alternateRegister, context, checkedModeReferences, 
                checkedModePriorities, &duplicateRefErrorIssued, &duplicatePriorityErrorIssued);
            findErrorsInAccessPolicies(errors, alternateRegister, registerContext);
        }

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
        errors.append(QObject::tr("Alternate groups are not unique or not empty in %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::findErrorsInAlternateRegisterModeRefs()
//-----------------------------------------------------------------------------
void RegisterValidator::findErrorsInAlternateRegisterModeRefs(QStringList& errors, QSharedPointer<AlternateRegister> selectedRegister, QString const& context, QStringList& checkedModeRefs, QStringList& checkedPriorities, bool* duplicateRefErrorIssued, bool* duplicatePriorityErrorIssued) const
{
    if (selectedRegister->getModeReferences()->isEmpty())
    {
        errors.append(QObject::tr("Alternate register %1 within %2 must have at least one mode reference defined.")
            .arg(selectedRegister->name()).arg(context));
    }

    QString altRegisterContext = QStringLiteral("alternate register ") + selectedRegister->name()
        + QStringLiteral(" within ") + context;

    CommonItemsValidator::findErrorsInModeRefs(errors, selectedRegister->getModeReferences(), altRegisterContext, 
        checkedModeRefs, checkedPriorities, duplicateRefErrorIssued, duplicatePriorityErrorIssued);
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::alternateRegisterIsValid()
//-----------------------------------------------------------------------------
bool RegisterValidator::alternateRegisterIsValid(QSharedPointer<AlternateRegister> alternateRegister, 
    QSharedPointer<Register> selectedRegister) const
{   
    if (!hasValidName(alternateRegister) ||
        !hasValidFields(alternateRegister, selectedRegister->getSize()) ||
        !hasValidParameters(alternateRegister))
    {
        return false;
    }

    if (docRevision_ == Document::Revision::Std14)
    {
        if (!hasValidIsPresent(alternateRegister) ||
            !hasValidAlternateGroups(alternateRegister))
        {
            return false;
        }
    }
    else if (docRevision_ == Document::Revision::Std22)
    {
        if (alternateRegister->getModeReferences()->isEmpty() ||
            !RegisterBaseValidator::hasValidAccessPolicies(alternateRegister))
        {
            return false;
        }
    }

    return true;
}
