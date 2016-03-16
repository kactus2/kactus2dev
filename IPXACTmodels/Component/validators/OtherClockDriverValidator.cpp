//-----------------------------------------------------------------------------
// File: OtherClockDriverValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.12.2015
//
// Description:
// Validator for the ipxact:otherClockDriver.
//-----------------------------------------------------------------------------

#include "OtherClockDriverValidator.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/OtherClockDriver.h>
#include <IPXACTmodels/common/ClockUnit.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::OtherClockDriverValidator()
//-----------------------------------------------------------------------------
OtherClockDriverValidator::OtherClockDriverValidator(QSharedPointer<ExpressionParser> expressionParser):
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::~OtherClockDriverValidator()
//-----------------------------------------------------------------------------
OtherClockDriverValidator::~OtherClockDriverValidator()
{

}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::validate()
//-----------------------------------------------------------------------------
bool OtherClockDriverValidator::validate(QSharedPointer<OtherClockDriver> otherClockDriver) const
{
    return hasValidName(otherClockDriver->getClockName()) &&
        hasValidClockValue(otherClockDriver->getClockPeriod()) &&
        hasValidClockValue(otherClockDriver->getClockPulseOffset()) &&
        hasValidClockValue(otherClockDriver->getClockPulseDuration()) &&
        hasValidClockPulseValue(otherClockDriver);
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::hasValidName()
//-----------------------------------------------------------------------------
bool OtherClockDriverValidator::hasValidName(QString const& name) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern("^\\s*$");
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(name);

    if (name.isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::hasValidClockValue()
//-----------------------------------------------------------------------------
bool OtherClockDriverValidator::hasValidClockValue(QSharedPointer<ClockUnit> clockUnit) const
{
    if (clockUnit)
    {
        if (!clockUnit->getValue().isEmpty())
        {
            bool valueToRealOk = true;
            expressionParser_->parseExpression(clockUnit->getValue()).toDouble(&valueToRealOk);

            return valueToRealOk;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::hasValidClockPulseValue()
//-----------------------------------------------------------------------------
bool OtherClockDriverValidator::hasValidClockPulseValue(QSharedPointer<OtherClockDriver> otherClockDriver) const
{
    if (!otherClockDriver->getClockPulseValue().isEmpty())
    {
        bool bitVectorOk = true;

        QString clockPulse = otherClockDriver->getClockPulseValue();
        int pulseValueBit = expressionParser_->parseExpression(clockPulse).toInt(&bitVectorOk);

        return bitVectorOk && pulseValueBit >= 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void OtherClockDriverValidator::findErrorsIn(QVector<QString>& errors,
    QSharedPointer<OtherClockDriver> otherClockDriver, QString const& context) const
{
    findErrorsInName(errors, otherClockDriver, context);
    findErrorsInClockUnits(errors, otherClockDriver, context);
    findErrorsInClockPulseValue(errors, otherClockDriver, context);
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void OtherClockDriverValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<OtherClockDriver> otherClockDriver, QString const& context) const
{
    if (!hasValidName(otherClockDriver->getClockName()))
    {
        errors.append(QObject::tr("Invalid name '%1' set for other clock driver within %2")
            .arg(otherClockDriver->getClockName()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::findErrorsInClockUnit()
//-----------------------------------------------------------------------------
void OtherClockDriverValidator::findErrorsInClockUnits(QVector<QString>& errors,
    QSharedPointer<OtherClockDriver> otherClockDriver, QString const& context) const
{
    if (!hasValidClockValue(otherClockDriver->getClockPeriod()))
    {
        errors.append(QObject::tr("Invalid clock period %1 set for other clock driver %2 within %3")
            .arg(otherClockDriver->getClockPeriod()->getValue()).arg(otherClockDriver->getClockName())
            .arg(context));
    }
    
    if (!hasValidClockValue(otherClockDriver->getClockPulseOffset()))
    {
        errors.append(QObject::tr("Invalid clock pulse offset %1 set for other clock driver %2 within %3")
            .arg(otherClockDriver->getClockPulseOffset()->getValue()).arg(otherClockDriver->getClockName())
            .arg(context));
    }

    if (!hasValidClockValue(otherClockDriver->getClockPulseDuration()))
    {
        errors.append(QObject::tr("Invalid clock pulse duration %1 set for other clock driver %2 within %3")
            .arg(otherClockDriver->getClockPulseDuration()->getValue()).arg(otherClockDriver->getClockName())
            .arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriverValidator::findErrorsInClockPulseValue()
//-----------------------------------------------------------------------------
void OtherClockDriverValidator::findErrorsInClockPulseValue(QVector<QString>& errors,
    QSharedPointer<OtherClockDriver> otherClockDriver, QString const& context) const
{
    if (!hasValidClockPulseValue(otherClockDriver))
    {
        errors.append(QObject::tr("Invalid clock pulse value %1 set for other clock driver %2 within %3")
            .arg(otherClockDriver->getClockPulseValue()).arg(otherClockDriver->getClockName()).arg(context));
    }
}