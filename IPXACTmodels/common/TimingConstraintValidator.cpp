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
	QSet<QString>& portNames) const
{
	if ( !hasValidValue(timingConstraint) || !hasValidEdge(timingConstraint) )
	{
		return false;
	}

	// Must refer to a logical port.
	foreach ( QString port, portNames )
	{
		if ( port == timingConstraint->getClockName() )
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
	QSharedPointer<TimingConstraint> timingConstraint, QString const&, QSet<QString>& ports) const
{
	if ( !hasValidValue(timingConstraint) )
	{
		errors.append(QObject::tr("Invalid value in timing constraint: %1").arg(timingConstraint->getValue()));
	}
		
	if ( !hasValidEdge(timingConstraint) )
	{
		errors.append(QObject::tr("Invalid edge type in timing constraint."));
	}

	// Must refer to a logical port.
	foreach ( QString port, ports )
	{
		if ( port == timingConstraint->getClockName() )
		{
			return;
		}
	}

	errors.append(QObject::tr("Invalid port name in timing constraint: %1").arg(timingConstraint->getClockName()));
}

//-----------------------------------------------------------------------------
// Function: TimingConstraintValidator::hasValidValue()
//-----------------------------------------------------------------------------
bool TimingConstraintValidator::hasValidValue(QSharedPointer<TimingConstraint> timingConstraint) const
{
	// Value must be between 0 and 100.
	return !( timingConstraint->getValue() < 0 || timingConstraint->getValue() > 100 );
}

//-----------------------------------------------------------------------------
// Function: TimingConstraintValidator::hasValidEdge()
//-----------------------------------------------------------------------------
bool TimingConstraintValidator::hasValidEdge(QSharedPointer<TimingConstraint> timingConstraint) const
{
	// Clock edge must be rise or fall.
	return !( timingConstraint->getClockEdge() != TimingConstraint::RISE
		&& timingConstraint->getClockEdge() != TimingConstraint::FALL );
}
