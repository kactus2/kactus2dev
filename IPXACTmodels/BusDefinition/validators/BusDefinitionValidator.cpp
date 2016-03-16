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
// Function: BusDefinitionValidator::BusDefinitionValidator()
//-----------------------------------------------------------------------------
BusDefinitionValidator::BusDefinitionValidator(QSharedPointer<ExpressionParser> expressionParser):
expressionParser_(expressionParser), 
    parameterValidator_(new ParameterValidator2014(expressionParser, 
    QSharedPointer<QList<QSharedPointer<Choice> > >()))
{

}

//-----------------------------------------------------------------------------
// Function: BusDefinitionValidator::~BusDefinitionValidator()
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

 	foreach (QSharedPointer<Parameter> currentParameter, *busDefinition->getParameters())
	{
		if (!parameterValidator_->validate(currentParameter))
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

    foreach (QSharedPointer<Parameter> currentParameter, *busDefinition->getParameters())
    {
        parameterValidator_->findErrorsIn(errors, currentParameter, context);
    }
}
