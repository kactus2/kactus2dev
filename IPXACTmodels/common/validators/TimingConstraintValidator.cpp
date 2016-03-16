//-----------------------------------------------------------------------------
// TimingConstraint: TimingConstraintValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 09.12.2015
//
// Description:
// Validator for ipxact:TimingConstraint.
//-----------------------------------------------------------------------------

#include "TimingConstraintValidator.h"

#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/common/TimingConstraint.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: TimingConstraintValidator::TimingConstraintValidator()
//-----------------------------------------------------------------------------
TimingConstraintValidator::TimingConstraintValidator()
{

}

//-----------------------------------------------------------------------------
// Function: TimingConstraintValidator::~TimingConstraintValidator()
//-----------------------------------------------------------------------------
TimingConstraintValidator::~TimingConstraintValidator()
{

}

//-----------------------------------------------------------------------------
// Function: TimingConstraintValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool TimingConstraintValidator::validate(QSharedPointer<TimingConstraint> timingConstraint,
	QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
	if (!hasValidValue(timingConstraint) || !hasValidEdge(timingConstraint))
	{
		return false;
	}

	// Must refer to a logical port.
	foreach (QSharedPointer<PortAbstraction> port, *ports)
	{
		if (port->getLogicalName() == timingConstraint->getClockName())
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraintValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void TimingConstraintValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<TimingConstraint> timingConstraint, QString const& context,
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports) const
{
	if (!hasValidValue(timingConstraint))
	{
		errors.append(QObject::tr("Invalid value '%1' in timing constraint in %2.").arg(
            QString::number(timingConstraint->getValue()), context));
	}
		
	if (!hasValidEdge(timingConstraint))
	{
		errors.append(QObject::tr("Invalid edge type in timing constraint in %1.").arg(context));
	}

	// Must refer to a logical port.
	foreach (QSharedPointer<PortAbstraction> port, *ports)
	{
		if (port->getLogicalName() == timingConstraint->getClockName())
		{
			return;
		}
	}

	errors.append(QObject::tr("Invalid port name '%1' referenced in timing constraint in %2.").arg(
        timingConstraint->getClockName(), context));
}

//-----------------------------------------------------------------------------
// Function: TimingConstraintValidator::hasValidValue()
//-----------------------------------------------------------------------------
bool TimingConstraintValidator::hasValidValue(QSharedPointer<TimingConstraint> timingConstraint) const
{
	// Value must be between 0 and 100.
	return 0 < timingConstraint->getValue() && timingConstraint->getValue() < 100;
}

//-----------------------------------------------------------------------------
// Function: TimingConstraintValidator::hasValidEdge()
//-----------------------------------------------------------------------------
bool TimingConstraintValidator::hasValidEdge(QSharedPointer<TimingConstraint> timingConstraint) const
{
	// Clock edge must be rise or fall.
	return timingConstraint->getClockEdge() == TimingConstraint::RISE ||
		timingConstraint->getClockEdge() == TimingConstraint::FALL;
}
