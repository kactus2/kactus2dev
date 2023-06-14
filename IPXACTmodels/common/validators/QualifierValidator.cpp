//-----------------------------------------------------------------------------
// File: QualifierValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.06.2023
//
// Description:
// Validator for ipxact:qualifier.
//-----------------------------------------------------------------------------

#include "QualifierValidator.h"

//-----------------------------------------------------------------------------
// Function: QualifierValidator::validate()
//-----------------------------------------------------------------------------
bool QualifierValidator::validate(QSharedPointer<Qualifier> qualifier, Document::Revision revision)
{
    // Std14 qualifiers must be checked, std22 can have any combination of qualifier information types
    if (revision != Document::Revision::Std22)
    {
        auto oldRevisionAllowedTypes = QList(
            {
                Qualifier::Address,
                Qualifier::Data,
                Qualifier::Clock,
                Qualifier::Reset,
                Qualifier::Any
            });

        auto qualifierTypes = *qualifier->getTypes();

        for (auto const& type : qualifierTypes)
        {
            if (!oldRevisionAllowedTypes.contains(type))
            {
                return false;
            }
        }

        if (qualifierTypes.size() > 2)
        {
            return false;
        }

        // Only data and address may be combined in a qualifier within an IP-XACT 2014 document
        if (qualifierTypes.size() == 2)
        {
            if (!qualifierTypes.contains(Qualifier::Type::Address) &&
                !qualifierTypes.contains(Qualifier::Type::Data))
            {
                return false;
            }
        }
    }

    return Details::hasValidAttributes(qualifier);
}

//-----------------------------------------------------------------------------
// Function: QualifierValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void QualifierValidator::findErrorsIn(QStringList& errors, QSharedPointer<Qualifier> qualifier, QString const& context,
    Document::Revision revision)
{
    if (revision != Document::Revision::Std22)
    {
        auto revisionAllowedTypes = QList(
            {
                Qualifier::Address,
                Qualifier::Data,
                Qualifier::Clock,
                Qualifier::Reset,
                Qualifier::Any
            });

        auto qualifierTypes = *qualifier->getTypes();

        for (auto const& type : qualifierTypes)
        {
            if (!revisionAllowedTypes.contains(type))
            {
                errors.append(QObject::tr("Qualifier in %1 contains illegal qualifier type for IP-XACT revision 2014.").arg(context));
            }
        }

        if (qualifierTypes.size() > 2)
        {
            errors.append(QObject::tr("Qualifier in %1 contains an illegal number of types for IP-XACT revision 2014.").arg(context));
        }

        if (qualifierTypes.size() == 2)
        {
            if (!qualifierTypes.contains(Qualifier::Type::Address) &&
                !qualifierTypes.contains(Qualifier::Type::Data))
            {
                errors.append(QObject::tr("Qualifier in %1 contains an illegal pairing of types for IP-XACT revision 2014.").arg(context));
            }
        }
    }

    if (revision == Document::Revision::Std22)
    {
        Details::findErrorsInAttributes(errors, qualifier, context);
    }
}

//-----------------------------------------------------------------------------
// Function: QualifierValidator::Details::hasValidAttributes()
//-----------------------------------------------------------------------------
bool QualifierValidator::Details::hasValidAttributes(QSharedPointer<Qualifier> qualifier)
{
    auto qualifierTypes = qualifier->getTypes();

    QStringList validResetLevels(QStringList()
        << QStringLiteral("low")
        << QStringLiteral("high")
        << QStringLiteral("")
    );

    QStringList validFlowTypes(QStringList()
        << QStringLiteral("creditReturn")
        << QStringLiteral("ready")
        << QStringLiteral("busy")
        << QStringLiteral("user"));

    if (auto const& resetLevel = qualifier->getAttribute(Qualifier::Attribute::ResetLevel);
        !resetLevel.isEmpty())
    {
        if (!validResetLevels.contains(resetLevel))
        {
            return false;
        }
    }

    if (auto const& clockEnableLevel = qualifier->getAttribute(Qualifier::Attribute::ClockEnableLevel);
        !clockEnableLevel.isEmpty())
    {
        if (!validResetLevels.contains(clockEnableLevel))
        {
            return false;
        }
    }

    if (auto const& powerEnableLevel = qualifier->getAttribute(Qualifier::Attribute::PowerEnableLevel);
        !powerEnableLevel.isEmpty())
    {
        if (!powerEnableLevel.isEmpty() && !validResetLevels.contains(powerEnableLevel))
        {
            return false;
        }
    }

    if (auto const& flowType = qualifier->getAttribute(Qualifier::Attribute::FlowType);
        !flowType.isEmpty())
    {
        if (!validFlowTypes.contains(flowType))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: QualifierValidator::Details::findErrorsInAttributes()
//-----------------------------------------------------------------------------
void QualifierValidator::Details::findErrorsInAttributes(QVector<QString>& errorList,
    QSharedPointer<Qualifier> qualifier,
    QString const& context)
{
    auto qualifierTypes = qualifier->getTypes();

    QStringList validResetLevels(QStringList()
        << QStringLiteral("low")
        << QStringLiteral("high")
        << QStringLiteral("")
    );

    QStringList validFlowTypes(QStringList()
        << QStringLiteral("creditReturn")
        << QStringLiteral("ready")
        << QStringLiteral("busy")
        << QStringLiteral("user"));

    if (auto const& resetLevel = qualifier->getAttribute(Qualifier::Attribute::ResetLevel);
        !resetLevel.isEmpty())
    {
        if (!validResetLevels.contains(resetLevel))
        {
            errorList.append(QObject::tr("Qualifier isReset in %1 contains an invalid level attribute.").arg(context));
        }
    }

    if (auto const& clockEnableLevel = qualifier->getAttribute(Qualifier::Attribute::ClockEnableLevel);
        !clockEnableLevel.isEmpty())
    {
        if (!validResetLevels.contains(clockEnableLevel))
        {
            errorList.append(QObject::tr("Qualifier isClockEn in %1 contains an invalid level attribute.").arg(context));
        }
    }
    
    if (auto const& powerEnableLevel = qualifier->getAttribute(Qualifier::Attribute::PowerEnableLevel);
        !powerEnableLevel.isEmpty())
    {
        if (!powerEnableLevel.isEmpty() && !validResetLevels.contains(powerEnableLevel))
        {
            errorList.append(QObject::tr("Qualifier isPowerEn in %1 contains an invalid level attribute.").arg(context));
        }
    }

    if (auto const& flowType = qualifier->getAttribute(Qualifier::Attribute::FlowType);
        !flowType.isEmpty())
    {
        if (!validFlowTypes.contains(flowType))
        {
            errorList.append(QObject::tr("Qualifier isFlowType in %1 contains an invalid flowType attribute.").arg(context));
        }
    }
}
