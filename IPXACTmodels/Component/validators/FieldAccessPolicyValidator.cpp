//-----------------------------------------------------------------------------
// File: FieldAccessPolicyValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 3.8.2023
//
// Description:
// Validator for ipxact:fieldAccessPolicy.
//-----------------------------------------------------------------------------

#include "FieldAccessPolicyValidator.h"
#include "FieldReferenceValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::FieldAccessPolicyValidator()
//-----------------------------------------------------------------------------
FieldAccessPolicyValidator::FieldAccessPolicyValidator(QSharedPointer<ExpressionParser> expressionParser):
    expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::validate()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::validate(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const
{
    return hasValidAccess(fieldAccessPolicy) &&
        hasValidDefinitionRef(fieldAccessPolicy) && hasValidWriteValueConstraint(fieldAccessPolicy) &&
        hasValidReadResponse(fieldAccessPolicy) && hasValidBroadcasts(fieldAccessPolicy) &&
        hasValidAccessRestrictions(fieldAccessPolicy) && hasValidReserved(fieldAccessPolicy);
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void FieldAccessPolicyValidator::findErrorsIn(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, QString const& context) const
{
    finderrorsInDefinitionRef(errors, fieldAccessPolicy, context);
    findErrorsInAccess(errors, fieldAccessPolicy, context);
    findErrorsInWriteValueConstraint(errors, fieldAccessPolicy, context);
    findErrorsInReadResponse(errors, fieldAccessPolicy, context);
    findErrorsInBroadcasts(errors, fieldAccessPolicy, context);
    findErrorsInAccessRestrictions(errors, fieldAccessPolicy, context);
    findErrorsInReserved(errors, fieldAccessPolicy, context);
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::hasValidDefinitionRef()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::hasValidDefinitionRef(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const
{
    if (!fieldAccessPolicy->getFieldAccessPolicyDefinitionRef().isEmpty() &&
        fieldAccessPolicy->getFieldAccessPolicyTypeDefinitionRef().isEmpty())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::hasValidAcces()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::hasValidAccess(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const
{
    if (fieldAccessPolicy->getAccess() == AccessTypes::READ_ONLY)
    {
        return fieldAccessPolicy->getModifiedWrite() == General::MODIFIED_WRITE_COUNT;
    }
    else if (fieldAccessPolicy->getAccess() == AccessTypes::WRITE_ONLY ||
        fieldAccessPolicy->getAccess() == AccessTypes::WRITEONCE)
    {
        return fieldAccessPolicy->getReadAction() == General::READ_ACTION_COUNT;
    }
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::hasValidWriteValueConstraint()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::hasValidWriteValueConstraint(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const
{
    if (fieldAccessPolicy->getWriteValueConstraint())
    {
        QSharedPointer<WriteValueConstraint> writeConstraint = fieldAccessPolicy->getWriteValueConstraint();

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
// Function: FieldAccessPolicyValidator::hasValidReadResponse()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::hasValidReadResponse(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const
{
    if (auto const& readResponse = fieldAccessPolicy->getReadResponse(); readResponse.isEmpty() == false)
    {
        if (isBitExpressionValid(readResponse) == false)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::hasValidBroadcasts()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::hasValidBroadcasts(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const
{
    for (auto const& fieldRef : *fieldAccessPolicy->getBroadcasts())
    {
        if (FieldReferenceValidator::validate(fieldRef) == false)
        {
            return false;
        }
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::hasValidAccessRestrictions()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::hasValidAccessRestrictions(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const
{
    if (fieldAccessPolicy->getAccessRestrictions()->isEmpty())
    {
        return true;
    }

    // Check mode refs. Refrence and priority must both be unique in all modes inside accessRestrictions.
    QSharedPointer<QList<QSharedPointer<ModeReference> > > allModeRefs(new QList<QSharedPointer<ModeReference> >());

    for (auto const& accessRestriction : *fieldAccessPolicy->getAccessRestrictions())
    {
        allModeRefs->append(*accessRestriction->modeRefs_);

        if (!isBitExpressionValid(accessRestriction->readAccessMask_) ||
            !isBitExpressionValid(accessRestriction->writeAccessMask_))
        {
            return false;
        }
    }

    return hasValidAccessRestrictionsModeRefs(allModeRefs);
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::hasValidAccessRestrictionsModeRefs()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::hasValidAccessRestrictionsModeRefs(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs) const
{
    QStringList refs;
    QStringList priorities;

    for (auto const& modeRef : *modeRefs)
    {
        if (modeRef->getPriority().isEmpty())
        {
            return false;
        }

        if (!refs.contains(modeRef->getReference()))
        {
            refs.append(modeRef->getReference());
        }
        else
        {
            return false;
        }

        if (!priorities.contains(modeRef->getPriority()))
        {
            priorities.append(modeRef->getPriority());
        }
        else
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::hasValidReserved()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::hasValidReserved(QSharedPointer<FieldAccessPolicy> fieldAccessPolicy) const
{
    if (!fieldAccessPolicy->getReserved().isEmpty())
    {
        bool isValidReserved = false;
        QString solvedValue = expressionParser_->parseExpression(fieldAccessPolicy->getReserved(), &isValidReserved);
        bool canConvert = false;
        int intValue = solvedValue.toInt(&canConvert);

        if (isValidReserved == false || canConvert == false || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::finderrorsInDefinitionRef()
//-----------------------------------------------------------------------------
void FieldAccessPolicyValidator::finderrorsInDefinitionRef(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, QString const& context) const
{
    if (!hasValidDefinitionRef(fieldAccessPolicy))
    {
        errors.append(QObject::tr("Field access policy in %1 has invalid or missing reference to definition reference type definitions.").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::findErrorsInAccess()
//-----------------------------------------------------------------------------
void FieldAccessPolicyValidator::findErrorsInAccess(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, QString const& context) const
{
    if (fieldAccessPolicy->getAccess() == AccessTypes::READ_ONLY)
    {
        if (fieldAccessPolicy->getModifiedWrite() != General::MODIFIED_WRITE_COUNT)
        {
            errors.append(QObject::tr("In field access policy within %1, access type readOnly does not "
                "allow the field access policy to include a modified write value.").arg(context));
        }
    }
    else if (fieldAccessPolicy->getAccess() == AccessTypes::WRITE_ONLY || fieldAccessPolicy->getAccess() == AccessTypes::WRITEONCE)
    {
        if (fieldAccessPolicy->getReadAction() != General::READ_ACTION_COUNT)
        {
            errors.append(QObject::tr("In field access policy within %1, access type write only and write once "
                "do not allow the field access policy to include a read action value.").arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::findErrorsInWriteValueConstraint()
//-----------------------------------------------------------------------------
void FieldAccessPolicyValidator::findErrorsInWriteValueConstraint(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, QString const& context) const
{
    if (hasValidWriteValueConstraint(fieldAccessPolicy))
    {
        return;
    }
    
    QSharedPointer<WriteValueConstraint> writeConstaint = fieldAccessPolicy->getWriteValueConstraint();
    if (writeConstaint->getType() == WriteValueConstraint::TYPE_COUNT)
    {
        errors.append(QObject::tr("Invalid write value constraint set for field access policy within %1")
            .arg(context));
    }
    else
    {
        bool minimumIsValid = isBitExpressionValid(writeConstaint->getMinimum());
        bool maximumIsValid = isBitExpressionValid(writeConstaint->getMaximum());

        if (!minimumIsValid)
        {
            errors.append(QObject::tr(
                "Invalid minimum value set for write value constraint in field access policy within %1")
                .arg(context));
        }
        if (!maximumIsValid)
        {
            errors.append(QObject::tr(
                "Invalid maximum value set for write value constraint in field access policy within %1")
                .arg(context));
        }

        if (minimumIsValid && maximumIsValid)
        {
            int minimumInt = expressionParser_->parseExpression(writeConstaint->getMinimum()).toInt();
            int maximumInt = expressionParser_->parseExpression(writeConstaint->getMaximum()).toInt();

            if (minimumInt > maximumInt)
            {
                errors.append(QObject::tr(
                    "Maximum value must be greater than or equal to the minimum value in "
                    "write value constraint set for field access policy within %1").arg(context));
            }
        }
    }
    
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::findErrorsInReadResponse()
//-----------------------------------------------------------------------------
void FieldAccessPolicyValidator::findErrorsInReadResponse(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, QString const& context) const
{
    if (!hasValidReadResponse(fieldAccessPolicy))
    {
        errors.append(QObject::tr("Invalid read response value for field access policy within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::findErrorsInBroadcasts()
//-----------------------------------------------------------------------------
void FieldAccessPolicyValidator::findErrorsInBroadcasts(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, QString const& context) const
{
    for (auto const& fieldRef : *fieldAccessPolicy->getBroadcasts())
    {
        FieldReferenceValidator::findErrorsIn(errors, fieldRef, context);
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::findErrorsInAccessRestrictions()
//-----------------------------------------------------------------------------

void FieldAccessPolicyValidator::findErrorsInAccessRestrictions(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, QString const& context) const
{
    if (fieldAccessPolicy->getAccessRestrictions()->isEmpty())
    {
        return;
    }

    // Check mode refs. Refrence and priority must both be unique in all modes inside accessRestrictions.
    QSharedPointer<QList<QSharedPointer<ModeReference> > > allModeRefs(new QList<QSharedPointer<ModeReference> >());

    for (auto const& accessRestriction : *fieldAccessPolicy->getAccessRestrictions())
    {
        allModeRefs->append(*accessRestriction->modeRefs_);

        if (!isBitExpressionValid(accessRestriction->readAccessMask_) && !accessRestriction->readAccessMask_.isEmpty())
        {
            errors.append(QObject::tr("A field access policy access restriction in %1 has an invalid "
                "read access mask value.").arg(context));
        }

        if (!isBitExpressionValid(accessRestriction->writeAccessMask_) && !accessRestriction->writeAccessMask_.isEmpty())
        {
            errors.append(QObject::tr("A field access policy access restriction in %1 has an invalid "
                "write access mask value.").arg(context));
        }
    }

    if (!hasValidAccessRestrictionsModeRefs(allModeRefs))
    {
        errors.append(QObject::tr("Field access policy access restrictions in %1"
            " cannot have empty or duplicate mode references or priorities.").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::findErrorsInReserved()
//-----------------------------------------------------------------------------
void FieldAccessPolicyValidator::findErrorsInReserved(QStringList& errors, QSharedPointer<FieldAccessPolicy> fieldAccessPolicy, QString const& context) const
{
    if (!hasValidReserved(fieldAccessPolicy))
    {
        errors.append(QObject::tr("Invalid reserved set for field access policy within %1").arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: FieldAccessPolicyValidator::isBitExpressionValid()
//-----------------------------------------------------------------------------
bool FieldAccessPolicyValidator::isBitExpressionValid(QString const& expression) const
{
    QString solvedValue = expressionParser_->parseExpression(expression);

    QRegularExpression bitExpression(QStringLiteral("^([0-9]+|[1-9]+[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$"));
    return bitExpression.match(expression).hasMatch() || bitExpression.match(solvedValue).hasMatch();
}
