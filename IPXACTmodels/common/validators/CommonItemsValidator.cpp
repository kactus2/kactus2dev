//-----------------------------------------------------------------------------
// File: CommonItemsValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.05.2023
//
// Description:
// Functions for validating common items in IPXACT models.
//-----------------------------------------------------------------------------


#include "CommonItemsValidator.h"

#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/AccessPolicy.h>
#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/utilities/Search.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::hasValidName()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidName(QString const& name)
{
    QRegularExpression validNameRegex(QStringLiteral("^[a-zA-Z:_][a-zA-Z0-9-_:.]*$"));
    return validNameRegex.match(name).hasMatch();
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidIsPresent(QString const& isPresent, QSharedPointer<ExpressionParser> parser)
{
    if (isPresent.isEmpty())
    {
        return true;
    }

    QString solvedValue = parser->parseExpression(isPresent);

    bool toIntOk = true;
    int intValue = solvedValue.toInt(&toIntOk);

    return (toIntOk && intValue >= 0 && intValue < 2);
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::isValidExpression()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::isValidExpression(QString const& expression, QSharedPointer<ExpressionParser> parser)
{
    bool expressionOk = false;
    QString solvedValue = parser->parseExpression(expression, &expressionOk);

    if (expressionOk == false)
    {
        return false;
    }

    bool changeOk = true;
    quint64 intValue = solvedValue.toULongLong(&changeOk);

    if (changeOk && intValue > 0)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::hasValidModeRefs()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidModeRefs(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs, 
    QSharedPointer<QList<QSharedPointer<Mode> > > availableModes)
{
    QStringList checkedReferences;
    QList<uint> checkedPriorities;

    QStringList availableModeNames = Search::getNames(availableModes);

    for (auto const& modeRef : *modeRefs)
    {
        auto reference = modeRef->getReference();
        auto priority = modeRef->getPriority();

        if (!CommonItemsValidator::hasValidName(reference))
        {
            return false;
        }

        if (checkedPriorities.contains(priority))
        {
            return false;
        }
        else
        {
            checkedPriorities.append(priority);
        }

        if (checkedReferences.contains(reference))
        {
            return false;
        }
        else
        {
            checkedReferences.append(reference);
        }

        if (!availableModeNames.contains(reference))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::hasValidModeRefs()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidModeRefs(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefsToCheck,
    QSharedPointer<QList<QSharedPointer<ModeReference> > > otherModeReferencesInUse,
    QSharedPointer<QList<QSharedPointer<Mode> > > availableModes)
{
    QStringList checkedReferences;
    QList<uint> checkedPriorities;

    QStringList availableModeNames = Search::getNames(availableModes);

    std::for_each(otherModeReferencesInUse->cbegin(), otherModeReferencesInUse->cend(),
        [&checkedReferences, &checkedPriorities](auto modeReference)
        {
            checkedReferences << modeReference->getReference();
            checkedPriorities << modeReference->getPriority();
        });

    for (auto const& modeRef : *modeRefsToCheck)
    {
        auto reference = modeRef->getReference();
        auto priority = modeRef->getPriority();

        if (!CommonItemsValidator::hasValidName(reference))
        {
            return false;
        }

        if (checkedPriorities.contains(priority))
        {
            return false;
        }
        else
        {
            checkedPriorities.append(priority);
        }

        if (checkedReferences.contains(reference))
        {
            return false;
        }
        else
        {
            checkedReferences.append(reference);
        }

        if (!availableModeNames.contains(reference))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::findErrorsInModeRefs()
//-----------------------------------------------------------------------------
void CommonItemsValidator::findErrorsInModeRefs(QStringList& errors, 
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs, QString const& context,
    QStringList& checkedRefs, QList<unsigned int>& checkedPriorities, bool* duplicateRefErrorIssued, 
    bool* duplicatePriorityErrorIssued, QSharedPointer<QList<QSharedPointer<Mode> > > availableModes)
{
    QStringList availableModeNames = Search::getNames(availableModes);

    for (auto const& modeRef : *modeRefs)
    {
        auto reference = modeRef->getReference();
        auto priority = modeRef->getPriority();

        if (!hasValidName(reference) || !availableModeNames.contains(reference))
        {
            errors.append(QObject::tr(
                "Mode reference in %1 has invalid or empty reference value '%2'.").arg(context).arg(reference));
        }

        if (checkedPriorities.contains(priority) && *duplicatePriorityErrorIssued == false)
        {
            errors.append(QObject::tr(
                "One or more mode references in %1 contain duplicate priority values.").arg(context));
            *duplicatePriorityErrorIssued = true;
        }
        else
        {
            checkedPriorities.append(priority);
        }

        if (checkedRefs.contains(reference) && *duplicateRefErrorIssued == false)
        {
            errors.append(QObject::tr(
                "One or more mode references in %1 contain duplicate mode reference values.")
                .arg(context));
            *duplicateRefErrorIssued = true;
        }
        else
        {
            checkedRefs.append(reference);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::singleModeReferenceIsValid()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::singleModeReferenceIsValid(
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefsInContainingElem, 
    QSharedPointer<ModeReference> modeReferenceToCheck, bool isRemap /*= false*/)
{
    if (!CommonItemsValidator::hasValidName(modeReferenceToCheck->getReference()))
    {
        return false;
    }

    for (auto const& modeRef : *modeRefsInContainingElem)
    {
        auto reference = modeRef->getReference();
        auto priority = modeRef->getPriority();

        if (reference == modeReferenceToCheck->getReference())
        {
            return false;
        }

        // Check priority only if mode ref is contained in other element than memory remap. In remaps, priority
        // doesn't have to be unique.
        if (!isRemap && priority == modeReferenceToCheck->getPriority())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::modeReferencePriorityIsValid()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::modeReferencePriorityIsValid(QList<unsigned int> const& modeRefPrioritiesInContainingElem,
    unsigned int modeReferencePriorityToCheck, bool isRemap /*= false*/)
{
    if (!isRemap)
    {
        for (auto const& modeRef : modeRefPrioritiesInContainingElem)
        {  
            // Check priority only if mode ref is contained in other element than memory remap. In remaps, priority
            // doesn't have to be unique.
            if (modeRef == modeReferencePriorityToCheck)
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::modeReferenceValueIsValid()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::modeReferenceValueIsValid(std::vector<std::string> const& modeRefsInContainingElem, 
    std::string const& modeReferenceToCheck, QStringList const& availableModes)
{    
    auto modeRefToCheckQ = QString::fromStdString(modeReferenceToCheck);
    if (!CommonItemsValidator::hasValidName(modeRefToCheckQ))
    {
        return false;
    }

    // Check that there are no duplicate mode references in containing element and that a corresponding component
    // mode is found.
    return std::none_of(modeRefsInContainingElem.cbegin(), modeRefsInContainingElem.cend(),
        [&modeReferenceToCheck, &modeRefToCheckQ, &availableModes](auto const& modeRef)
        {
            return modeRef == modeReferenceToCheck || !availableModes.contains(modeRefToCheckQ);
        });
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::hasValidAccessPolicies()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidAccessPolicies(
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies,
    QSharedPointer<QList<QSharedPointer<Mode> > > availableModes)
{
    int accessPoliciesWithoutModeRef = 0;

    auto allModeRefs = QSharedPointer<QList<QSharedPointer<ModeReference> > >(
        new QList<QSharedPointer<ModeReference> >());

    for (auto const& accessPolicy : *accessPolicies)
    {
        auto modeRefs = accessPolicy->getModeReferences();
        if (modeRefs->isEmpty())
        {
            accessPoliciesWithoutModeRef++;
        }

        std::for_each(modeRefs->cbegin(), modeRefs->cend(), [&allModeRefs](auto modeRef)
            {
                allModeRefs->append(modeRef);
            });
    }

    // Check if the mode references of the access policy are valid. Also check for duplicate mode refs between
    // all access policies.
    if (!hasValidModeRefs(allModeRefs, availableModes))
    {
        return false;
    }

    // Number of access policies without mode references cannot be greater than one (no mode ref = applies to all
    // other modes not explicitly defined with mode references).
    if (accessPoliciesWithoutModeRef > 1)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::findErrorsInAccessPolicies()
//-----------------------------------------------------------------------------
void CommonItemsValidator::findErrorsInAccessPolicies(QStringList& errors, 
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies, QSharedPointer<QList<QSharedPointer<Mode> > > componentModes, QString const& context)
{
    int accessPoliciesWithoutModeRefs = 0;

    QString accessPolicyContext = QStringLiteral("access policies of ") + context;

    bool duplicateModeRefErrorIssued = false;
    bool duplicateModePriorityErrorIssued = false;

    QStringList checkedModeReferences;
    QList<unsigned int> checkedModePriorities;

    for (auto const& accessPolicy : *accessPolicies)
    {
        if (accessPolicy->getModeReferences()->isEmpty())
        {
            accessPoliciesWithoutModeRefs++;
        }

        // Check mode references in current access policy, and look for duplicate references.
        CommonItemsValidator::findErrorsInModeRefs(errors, accessPolicy->getModeReferences(), accessPolicyContext,
            checkedModeReferences, checkedModePriorities, &duplicateModeRefErrorIssued, &duplicateModePriorityErrorIssued, componentModes);
    }

    // Number of access policies without mode references cannot be greater than one (no mode ref = applies to all
    // other modes not explicitly defined with mode references).
    if (accessPoliciesWithoutModeRefs > 1)
    {
        errors.append(QObject::tr("In %1: multiple access policies without mode references are not allowed.")
            .arg(accessPolicyContext));
    }
}
