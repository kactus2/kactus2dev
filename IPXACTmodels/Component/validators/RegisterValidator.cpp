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
#include <IPXACTmodels/Component/MemoryArray.h>
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
        componentModes_ = newComponent->getModes();
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
    return hasValidSize(selectedRegister) &&
           hasValidFields(selectedRegister, selectedRegister->getSize()) &&
           hasValidAlternateRegisters(selectedRegister) &&
            RegisterBaseValidator::validate(selectedRegister) &&
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

    quint64 registerSizeUInt = expressionParser_->parseExpression(registerSize).toULongLong();

    QVector<QString> fieldTypeIdentifiers;

    QHash<QString, QSharedPointer<Field> > foundFieldNames;

    bool fieldsAreValid = true;
    auto fields = selectedRegister->getFields();

    // Setup memory areas for register fields
    MemoryReserve memReserve;
    setupMemoryAreas(selectedRegister, memReserve);
    
    // Check for overlapping and uncontained areas
    QSet<QString> errorAreas;
    memReserve.checkOverlapAndContainment(errorAreas, registerSizeUInt);

    for (int fieldIndex = 0; fieldIndex < fields->size(); ++fieldIndex)
    {
        bool currentValid = true;
        QSharedPointer<Field> field = fields->at(fieldIndex);

        // Mark each field with non-unique name as invalid
        if (foundFieldNames.contains(field->name()))
        {
            fieldValidator_->setChildItemValidity(foundFieldNames[field->name()], false);
            currentValid = false;
        }
        else
        {
            foundFieldNames.insert(field->name(), field);
        }

        // Mark invalid if overlapping or not contained
        if (errorAreas.contains(field->name()))
        {
            fieldValidator_->setChildItemValidity(field, false);
        }

        // Mark invalid if there is a field with same type identifier but differing definitions
        if (!field->getTypeIdentifier().isEmpty() && fieldTypeIdentifiers.contains(field->getTypeIdentifier()))
        {
            int typeIdIndex = fieldTypeIdentifiers.indexOf(field->getTypeIdentifier());

            QSharedPointer<Field> comparedField = fields->at(typeIdIndex);
            if (!fieldsHaveSimilarDefinitionGroups(field, comparedField))
            {
                fieldValidator_->setChildItemValidity(comparedField, false);
                currentValid = false;
            }
        }

        // Mark invalid if volatility of field is not equal to volatility of register
        if (field->getVolatile().toBool() == true && selectedRegister->getVolatile() == QLatin1String("false"))
        {
            currentValid = false;
        }

        if (docRevision_ == Document::Revision::Std14 && !fieldHasValidAccess(selectedRegister, field))
        {
            currentValid = false;
        }

        if (currentValid == false)
        {
            fieldValidator_->setChildItemValidity(field, false);
            fieldsAreValid = false;
        }

        fieldTypeIdentifiers.append(field->getTypeIdentifier());
    }
    
    if (!fieldsAreValid)
    {
        return false;
    }

    // Validate fields separately.
    for (auto const& field : *fields)
    {
        if (!fieldValidator_->validate(field))
        {
            return false;
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

    QStringList checkedModeReferences;
    QStringList checkedModePriorities;

    auto allModeRefs = QSharedPointer<QList<QSharedPointer<ModeReference> > >(
        new QList<QSharedPointer<ModeReference> >());

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

        auto modeRefs = alternateRegister->getModeReferences();
        std::for_each(modeRefs->cbegin(), modeRefs->cend(), [&allModeRefs](auto modeRef)
            {
                allModeRefs->append(modeRef);
            });

        if (!alternateRegisterIsValid(alternateRegister, selectedRegister))
        {
            return false;
        }
    }
    
    if (!CommonItemsValidator::hasValidModeRefs(allModeRefs, componentModes_))
    {
        return false;
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
    if (selectedRegister->getFields()->isEmpty())
    {
        errors.append(QObject::tr("Register %1 must contain at least one field").arg(selectedRegister->name()));
        return;
    }

    QStringList fieldNames;
    QStringList fieldTypeIdentifiers;

    quint64 registerSizeUInt = expressionParser_->parseExpression(registerSize).toULongLong();

    // Create memory areas for fields for checking overlap and containment
    MemoryReserve reservedArea;
    setupMemoryAreas(selectedRegister, reservedArea);

    for (auto const& field : *selectedRegister->getFields())
    {
        fieldValidator_->findErrorsIn(errors, field, context);

        if (fieldNames.contains(field->name()))
        {
            errors.append(
                QObject::tr("Name %1 of fields in %2 is not unique.").arg(field->name()).arg(context));
        }

        reservedArea.findErrorsInContainment(errors, field->name(), QStringLiteral("Field"), 
            selectedRegister->name(), 0, registerSizeUInt - 1);
        
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
    QList<unsigned int> checkedModePriorities;

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
void RegisterValidator::findErrorsInAlternateRegisterModeRefs(QStringList& errors, QSharedPointer<AlternateRegister> selectedRegister, QString const& context, QStringList& checkedModeRefs, QList<unsigned int>& checkedPriorities, bool* duplicateRefErrorIssued, bool* duplicatePriorityErrorIssued) const
{
    if (selectedRegister->getModeReferences()->isEmpty())
    {
        errors.append(QObject::tr("Alternate register %1 within %2 must have at least one mode reference defined.")
            .arg(selectedRegister->name()).arg(context));
    }

    QString altRegisterContext = QStringLiteral("alternate register ") + selectedRegister->name()
        + QStringLiteral(" within ") + context;

    CommonItemsValidator::findErrorsInModeRefs(errors, selectedRegister->getModeReferences(), altRegisterContext, 
        checkedModeRefs, checkedPriorities, duplicateRefErrorIssued, duplicatePriorityErrorIssued, componentModes_);
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

//-----------------------------------------------------------------------------
// Function: RegisterValidator::getTrueFieldBitWidth()
//-----------------------------------------------------------------------------
quint64 RegisterValidator::getTrueFieldBitWidth(QSharedPointer<Field> field) const
{
    quint64 bitWidth = expressionParser_->parseExpression(field->getBitWidth()).toULongLong();

    // Calculate true bit width taking memory array dimensions into account.
    if (auto memArray = field->getMemoryArray();
        memArray && docRevision_ == Document::Revision::Std22)
    {
        auto fieldDimensions = field->getMemoryArray()->getDimensions();

        std::for_each(fieldDimensions->cbegin(), fieldDimensions->cend(), [&bitWidth, this](auto dim)
            {
                bitWidth *= expressionParser_->parseExpression(dim->value_).toULongLong();
            });
    }

    return bitWidth;
}

//-----------------------------------------------------------------------------
// Function: RegisterValidator::setupMemoryAreas()
//-----------------------------------------------------------------------------
void RegisterValidator::setupMemoryAreas(QSharedPointer<RegisterDefinition> reg, MemoryReserve& reserve) const
{
    // Create memory areas for all fields and their possible replicas (dimensions)
    for (auto const& field : *reg->getFields())
    {
        auto dimension = field->getDimension();
        auto fieldOffset = expressionParser_->parseExpression(field->getBitOffset()).toULongLong();
        auto fieldSize = fieldOffset + expressionParser_->parseExpression(field->getBitWidth()).toULongLong() - 1;
        
        // Reset validity
        fieldValidator_->setChildItemValidity(field, true);
        
        if (field->getIsPresent().isEmpty() || expressionParser_->parseExpression(field->getIsPresent()).toInt() == 1)
        {
            if (dimension.isEmpty() || expressionParser_->parseExpression(dimension).toULongLong() == 1)
            {
                reserve.addArea(field->name(), fieldOffset, fieldSize);
            }
            else
            {
                quint64 stride = 0;
                auto allDimensions = field->getMemoryArray()->getDimensions();

                // If stride is empty, set field width as stride
                if (!field->getStride().isEmpty())
                {
                    stride = expressionParser_->parseExpression(field->getStride()).toULongLong();
                }
                else
                {
                    stride = expressionParser_->parseExpression(field->getBitWidth()).toULongLong();
                }

                // Create replicas and calculate their addressing based on stride
                quint64 replicas = 1;
                std::for_each(allDimensions->cbegin(), allDimensions->cend(),
                    [&replicas, this](QSharedPointer<MemoryArray::Dimension> dim)
                    {
                        replicas *= expressionParser_->parseExpression(dim->value_).toULongLong();
                    });

                for (int i = 0; i < replicas; i++)
                {
                    // start from fieldOffset, then add i multiples of stride
                    reserve.addArea(field->name(), fieldOffset + i * stride,  i * stride + fieldSize);
                }
            }
        }
    }
}
