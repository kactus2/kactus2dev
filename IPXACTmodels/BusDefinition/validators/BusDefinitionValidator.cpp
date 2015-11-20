//-----------------------------------------------------------------------------
// File: BusDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 17.11.2015
//
// Description:
// Validator for ipxact:BusDefinition.
//-----------------------------------------------------------------------------

#include "BusDefinitionValidator.h"
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
BusDefinitionValidator::BusDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
//-----------------------------------------------------------------------------
BusDefinitionValidator::~BusDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinitionValidator::validate()
//-----------------------------------------------------------------------------
bool BusDefinitionValidator::validate(BusDefinition const* busDefinition) const
{
	bool valid = true;

	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	ParameterValidator2014 paraVali(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	if (!busDefinition->getVlnv().isValid())
	{
		valid = false;
	}
	else if ( !busDefinition->getMaxMasters().isEmpty() &&
		!parser->isValidExpression( busDefinition->getMaxMasters() ) )
	{
		valid = false;
	}
	else if ( !busDefinition->getMaxSlaves().isEmpty() &&
		!parser->isValidExpression( busDefinition->getMaxSlaves() ) )
	{
		valid = false;
	}

	foreach ( QSharedPointer<Parameter> currentPara, *busDefinition->getParameters() )
	{
		if ( !paraVali.hasValidValue(currentPara.data() ) )
		{
			valid = false;
			break;
		}
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void BusDefinitionValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<BusDefinition> busDefinition,
    QString const& context) const
{
	QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

	ParameterValidator2014 paraVali(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());

	busDefinition->getVlnv().isValid(errors, QObject::tr("containing bus definition") );

	foreach ( QSharedPointer<Parameter> currentPara, *busDefinition->getParameters() )
	{
		paraVali.findErrorsIn( errors, currentPara, context );
	}

	if ( !busDefinition->getMaxMasters().isEmpty() &&
		!parser->isValidExpression( busDefinition->getMaxMasters() ) )
	{
		errors.append(QObject::tr("MaxMasters is an invalid expression: %1"
			).arg(busDefinition->getMaxMasters()));
	}

	if ( !busDefinition->getMaxSlaves().isEmpty() &&
		!parser->isValidExpression( busDefinition->getMaxSlaves() ) )
	{
		errors.append(QObject::tr("MaxSlaves is an invalid expression: %1"
			).arg(busDefinition->getMaxSlaves()));
	}
}
