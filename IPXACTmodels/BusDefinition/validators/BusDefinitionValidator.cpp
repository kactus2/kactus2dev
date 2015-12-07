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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <IPXACTmodels/Component/Choice.h>

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
BusDefinitionValidator::BusDefinitionValidator(QSharedPointer<ExpressionParser> expressionParser):
expressionParser_(expressionParser)
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
bool BusDefinitionValidator::validate(QSharedPointer<const BusDefinition> busDefinition) const
{
	if (!busDefinition->getVlnv().isValid())
	{
		return false;
	}

	if (!busDefinition->getMaxMasters().isEmpty() &&
        !expressionParser_->isValidExpression(busDefinition->getMaxMasters()))
	{
		return false;
	}

	if (!busDefinition->getMaxSlaves().isEmpty() &&
        !expressionParser_->isValidExpression(busDefinition->getMaxSlaves()))
	{
		return false;
	}

    ParameterValidator2014 parameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >());
	foreach (QSharedPointer<Parameter> currentParameter, *busDefinition->getParameters())
	{
		if (!parameterValidator.validate(currentParameter))
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void BusDefinitionValidator::findErrorsIn(QVector<QString>& errors,
    QSharedPointer<const BusDefinition> busDefinition) const
{
    QString context = QObject::tr("containing bus definition");
	if (busDefinition->getVlnv().isValid(errors, context))
    {
        context = QObject::tr("bus definition %1").arg(busDefinition->getVlnv().toString());
    }

	if (!busDefinition->getMaxMasters().isEmpty() && 
        !expressionParser_->isValidExpression(busDefinition->getMaxMasters()))
	{
		errors.append(QObject::tr("MaxMasters '%1' is not a valid expression within %2.").arg(
            busDefinition->getMaxMasters(), context));
	}

	if (!busDefinition->getMaxSlaves().isEmpty() &&
		!expressionParser_->isValidExpression(busDefinition->getMaxSlaves()))
	{
		errors.append(QObject::tr("MaxSlaves '%1' is not a valid expression within %2.").arg(
            busDefinition->getMaxSlaves(), context));
	}

    ParameterValidator2014 parameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >());
    foreach (QSharedPointer<Parameter> currentParameter, *busDefinition->getParameters())
    {
        parameterValidator.findErrorsIn(errors, currentParameter, context);
    }
}
