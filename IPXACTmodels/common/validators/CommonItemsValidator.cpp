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

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::hasValidName()
//-----------------------------------------------------------------------------
bool CommonItemsValidator::hasValidName(QString const& name)
{
    return (name.isEmpty() == false && name.trimmed().isEmpty() == false);
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
bool CommonItemsValidator::hasValidModeRefs(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs, QStringList& checkedReferences, QStringList& checkedPriorities)
{
    QRegularExpression whiteSpaceRegex(QStringLiteral("\\s+"));

    for (auto const& modeRef : *modeRefs)
    {
        auto reference = modeRef->getReference();
        auto priorityStr = modeRef->getPriority();

        bool priorityValidInt = false;
        int priority = priorityStr.toUInt(&priorityValidInt);

        if (whiteSpaceRegex.match(modeRef->getReference()).hasMatch())
        {
            return false;
        }

        if (!priorityValidInt || priority < 0)
        {
            return false;
        }

        if (checkedPriorities.contains(modeRef->getPriority()))
        {
            return false;
        }
        else
        {
            checkedPriorities.append(modeRef->getPriority());
        }

        if (checkedReferences.contains(modeRef->getReference()))
        {
            return false;
        }
        else
        {
            checkedReferences.append(modeRef->getReference());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsValidator::findErrorsInModeRefs()
//-----------------------------------------------------------------------------
void CommonItemsValidator::findErrorsInModeRefs(QStringList& errors, 
    QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs, QString const& context,
    QStringList& checkedRefs, QStringList& checkedPriorities, bool* duplicateRefErrorIssued, 
    bool* duplicatePriorityErrorIssued)
{
    QRegularExpression whiteSpaceRegex(QStringLiteral("\\s+"));

    for (auto const& modeRef : *modeRefs)
    {
        auto reference = modeRef->getReference();
        auto priorityStr = modeRef->getPriority();

        bool priorityValidInt = false;
        int priority = priorityStr.toUInt(&priorityValidInt);

        if (whiteSpaceRegex.match(modeRef->getReference()).hasMatch())
        {
            errors.append(QObject::tr(
                "Mode reference in %1 has invalid reference value '%2'.").arg(context).arg(reference));
        }

        if (!priorityValidInt || priority < 0)
        {
            errors.append(QObject::tr(
                "Mode reference in %1 has invalid priority '%2'.").arg(context).arg(priorityStr));
        }

        if (checkedPriorities.contains(modeRef->getPriority()) && *duplicatePriorityErrorIssued == false)
        {
            errors.append(QObject::tr(
                "One or more mode references of %1 contain duplicate priority values.").arg(context));
            *duplicatePriorityErrorIssued = true;
        }
        else
        {
            checkedPriorities.append(modeRef->getPriority());
        }

        if (checkedRefs.contains(modeRef->getReference()) && *duplicateRefErrorIssued == false)
        {
            errors.append(QObject::tr(
                "One or more mode references of %1 contain duplicate mode reference values.")
                .arg(context));
            *duplicateRefErrorIssued = true;
        }
        else
        {
            checkedRefs.append(modeRef->getReference());
        }
    }
}
