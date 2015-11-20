//-----------------------------------------------------------------------------
// File: CPUValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 18.11.2015
//
// Description:
// Validator for ipxact:CPU.
//-----------------------------------------------------------------------------

#include "CPUValidator.h"
#include "../../validators/ParameterValidator2014.h"

#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/Enumeration.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
CPUValidator::CPUValidator()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
//-----------------------------------------------------------------------------
CPUValidator::~CPUValidator()
{

}

//-----------------------------------------------------------------------------
// Function: CPUValidator::validate()
//-----------------------------------------------------------------------------
bool CPUValidator::validate(QSharedPointer<Cpu> CPU) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	ParameterValidator2014 paraVali(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	if ( !hasValidName( CPU->name() ) )
	{
		return false;
	}
	else if ( !CPU->getIsPresent().isEmpty() &&
		!parser->isValidExpression( CPU->getIsPresent() ) )
	{
		return false;
	}
	else if ( CPU->getAddressSpaceReferences()->count() < 1 )
	{
		return false;
	}

	foreach ( QSharedPointer<Cpu::AddressSpaceRef> currentRef, *CPU->getAddressSpaceReferences() )
	{
		if ( !currentRef->getAddressSpaceRef().isEmpty() &&
			!hasValidName( currentRef->getAddressSpaceRef() ) )
		{
			return false;
		}
		else if ( !currentRef->getIsPresent().isEmpty() &&
			!parser->isValidExpression( currentRef->getIsPresent() ) )
		{
			return false;
		}
	}

	foreach ( QSharedPointer<Parameter> currentPara, *CPU->getParameters() )
	{
		if ( !paraVali.hasValidValue( currentPara.data() ) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: CPUValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void CPUValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<Cpu> CPU,
    QString const& context) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	ParameterValidator2014 paraVali(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	if ( !hasValidName( CPU->name() ) )
	{
		errors.append(QObject::tr("The name is invalid or in-existing: %1").arg(CPU->name()));
	}

	if ( !CPU->getIsPresent().isEmpty() && !parser->isValidExpression( CPU->getIsPresent() ) )
	{
		errors.append(QObject::tr("The presence is invalid: %1").arg(CPU->getIsPresent()));
	}
	
	if ( CPU->getAddressSpaceReferences()->count() < 1 )
	{
		errors.append(QObject::tr("Must have at least one address space reference."));
	}

	foreach ( QSharedPointer<Cpu::AddressSpaceRef> currentRef, *CPU->getAddressSpaceReferences() )
	{
		if ( !currentRef->getAddressSpaceRef().isEmpty() &&
			!hasValidName( currentRef->getAddressSpaceRef() ) )
		{
			errors.append(QObject::tr("An address space reference is an invalid expression: %1"
				).arg(currentRef->getAddressSpaceRef()));
		}
		
		if ( !currentRef->getIsPresent().isEmpty() &&
			!parser->isValidExpression( currentRef->getIsPresent() ) )
		{
			errors.append(QObject::tr("Presence of reference %1 is an invalid expression: %2"
				).arg(currentRef->getAddressSpaceRef().arg(currentRef->getIsPresent())));
		}
	}

	foreach ( QSharedPointer<Parameter> currentPara, *CPU->getParameters() )
	{
		paraVali.findErrorsIn( errors, currentPara, context );
	}
}

//-----------------------------------------------------------------------------
// Function: EnumeratedValueValidator::hasValidName()
//-----------------------------------------------------------------------------
bool CPUValidator::hasValidName(QString const& name) const
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
