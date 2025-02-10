//-----------------------------------------------------------------------------
// File: RegionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.6.2023
//
// Description:
// Validator for the ipxact:region.
//-----------------------------------------------------------------------------

#include "RegionValidator.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/MemoryReserve.h>
#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/Region.h>
#include <IPXACTmodels/Component/Segment.h>

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: RegionValidator::RegionValidator()
//-----------------------------------------------------------------------------
RegionValidator::RegionValidator(QSharedPointer<ExpressionParser> expressionParser):
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: RegionValidator::validate()
//-----------------------------------------------------------------------------
bool RegionValidator::validate(QSharedPointer<Region> region) const
{
    return hasValidName(region->name()) &&  hasValidRange(region) && hasValidAddressOffset(region);
}

//-----------------------------------------------------------------------------
// Function: RegionValidator::hasValidName()
//-----------------------------------------------------------------------------
bool RegionValidator::hasValidName(QString const& name) const
{
    return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: RegionValidator::hasValidRange()
//-----------------------------------------------------------------------------
bool RegionValidator::hasValidRange(QSharedPointer<Region> region) const
{
    if (!region->getRange().isEmpty())
    {
        bool rangeOk = true;
        bool expressionValid = false;
        quint64 range =
            expressionParser_->parseExpression(region->getRange(), &expressionValid).toULongLong(&rangeOk);

        return rangeOk && expressionValid && range != 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RegionValidator::hasValidAddressOffset()
//-----------------------------------------------------------------------------
bool RegionValidator::hasValidAddressOffset(QSharedPointer<Region> region) const
{
    if (!region->getAddressOffset().isEmpty())
    {
        bool offsetOk = true;
        expressionParser_->parseExpression(region->getAddressOffset()).toULongLong(&offsetOk);

        return offsetOk;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RegionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void RegionValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Region> region,
    QString const& context) const
{
    findErrorsInName(errors, region, context);
    findErrorsInRange(errors, region, context);
    findErrorsInOffset(errors, region, context);
}

//-----------------------------------------------------------------------------
// Function: RegionValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void RegionValidator::findErrorsInName(QVector<QString>& errors, QSharedPointer<Region> region,
    QString const& context) const
{
    if (!hasValidName(region->name()))
    {
        errors.append(QObject::tr("Invalid name specified for region %1 in %2.").arg(region->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegionValidator::findErrorsInRange()
//-----------------------------------------------------------------------------
void RegionValidator::findErrorsInRange(QVector<QString>& errors, QSharedPointer<Region> region,
    QString const& context) const
{
    if (!hasValidRange(region))
    {
        errors.append(QObject::tr("Invalid range set for region %1 in %2.").arg(region->name(), context));
    }
}

//-----------------------------------------------------------------------------
// Function: RegionValidator::findErrorsInOffset()
//-----------------------------------------------------------------------------
void RegionValidator::findErrorsInOffset(QVector<QString>& errors, QSharedPointer<Region> region,
    QString const& context) const
{
    if (!hasValidAddressOffset(region))
    {
        errors.append(QObject::tr("Invalid address offset set for region %1 in %2.").arg(region->name(), context));
    }
}
