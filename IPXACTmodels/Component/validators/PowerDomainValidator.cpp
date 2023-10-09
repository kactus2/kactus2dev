//-----------------------------------------------------------------------------
// File: PowerDomainValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 6.10.2023
//
// Description:
// Validator for the ipxact:powerDomain.
//-----------------------------------------------------------------------------

#include "PowerDomainValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/PowerDomain.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/utilities/Search.h>

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::PowerDomainValidator()
//-----------------------------------------------------------------------------
PowerDomainValidator::PowerDomainValidator(QSharedPointer<QList<QSharedPointer<PowerDomain> > > availableDomains,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ParameterValidator> parameterValidator):
availableDomains_(availableDomains),
    expressionParser_(expressionParser),
    parameterValidator_(parameterValidator)
{

}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::~PowerDomainValidator()
//-----------------------------------------------------------------------------
PowerDomainValidator::~PowerDomainValidator()
{

}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::validate()
//-----------------------------------------------------------------------------
bool PowerDomainValidator::validate(QSharedPointer<PowerDomain> powerDomain) const
{
    return hasValidName(powerDomain->name()) &&
        hasValidAlwaysOn(powerDomain) &&
        hasValidSubDomainOf(powerDomain) &&
        hasValidParameters(powerDomain);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::hasValidName()
//-----------------------------------------------------------------------------
bool PowerDomainValidator::hasValidName(QString const& name) const
{
    return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::hasValidAlwaysOn()
//-----------------------------------------------------------------------------
bool PowerDomainValidator::hasValidAlwaysOn(QSharedPointer<PowerDomain> powerDomain) const
{
    return CommonItemsValidator::hasValidIsPresent(powerDomain->getAlwaysOn(), expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::hasValidSubDomainOf()
//-----------------------------------------------------------------------------
bool PowerDomainValidator::hasValidSubDomainOf(QSharedPointer<PowerDomain> powerDomain) const
{
    if (powerDomain->getSubDomainOf().isEmpty())
    {
        return true;
    }

    auto referencedDomain = Search::findByName(powerDomain->getSubDomainOf(), *availableDomains_);

    return referencedDomain.isNull() == false;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::hasValidParameters()
//-----------------------------------------------------------------------------
bool PowerDomainValidator::hasValidParameters(QSharedPointer<PowerDomain> powerDomain) const
{
    if (!powerDomain->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        for (QSharedPointer<Parameter> const& parameter : *powerDomain->getParameters())
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
// Function: PowerDomainValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void PowerDomainValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<PowerDomain> powerDomain,
    QString const& context) const
{
    QString powerDomainContext = QStringLiteral("power domain ") + powerDomain->name();

    findErrorsInName(errors, powerDomain, context);
    findErrorsInAlwaysOn(errors, powerDomain, context);
    findErrorsInSubDomainOf(errors, powerDomain, context);
    findErrorsInParameters(errors, powerDomain, powerDomainContext);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::componentChange()
//-----------------------------------------------------------------------------
void PowerDomainValidator::componentChange(QSharedPointer<QList<QSharedPointer<PowerDomain>>> powerDomains)
{
    availableDomains_ = powerDomains;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void PowerDomainValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<PowerDomain> powerDomain,
    QString const& context) const
{
    if (!hasValidName(powerDomain->name()))
    {
        errors.append(QObject::tr("Invalid name specified for power domain %1 within %2").arg(
            powerDomain->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::findErrorsInAlwaysOn()
//-----------------------------------------------------------------------------
void PowerDomainValidator::findErrorsInAlwaysOn(QVector<QString>& errors, QSharedPointer<PowerDomain> powerDomain,
    QString const& context) const
{
    if (!hasValidAlwaysOn(powerDomain))
    {
        errors.append(QObject::tr("Invalid expression set for always on in power domain %1 within %2").arg(
            powerDomain->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::findErrorsInSubDomainOf()
//-----------------------------------------------------------------------------
void PowerDomainValidator::findErrorsInSubDomainOf(QVector<QString>& errors, 
    QSharedPointer<PowerDomain> powerDomain, QString const& context) const
{
    if (!hasValidSubDomainOf(powerDomain))
    {
        errors.append(QObject::tr("Invalid power domain '%1' referenced in power domain %2 within %3").arg(
            powerDomain->getSubDomainOf(), powerDomain->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: PowerDomainValidator::findErrorsInParameters()
//-----------------------------------------------------------------------------
void PowerDomainValidator::findErrorsInParameters(QVector<QString>& errors,
    QSharedPointer<PowerDomain> powerDomain, QString const& context) const
{
    if (!powerDomain->getParameters()->isEmpty())
    {
        QStringList parameterNames;
        for (QSharedPointer<Parameter> const& parameter : *powerDomain->getParameters())
        {
            if (parameterNames.contains(parameter->name()))
            {
                errors.append(QObject::tr("Name %1 of parameters in %2 is not unique.").arg(parameter->name(),
                    context));
            }
            else
            {
                parameterNames.append(parameter->name());
            }

            parameterValidator_->findErrorsIn(errors, parameter, context);
        }
    }
}
