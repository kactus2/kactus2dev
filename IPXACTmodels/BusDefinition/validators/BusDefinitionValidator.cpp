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

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/Component/validators/ChoiceValidator.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionValidator::BusDefinitionValidator()
//-----------------------------------------------------------------------------
BusDefinitionValidator::BusDefinitionValidator(LibraryInterface* library,
    QSharedPointer<ExpressionParser> expressionParser):
library_(library),
expressionParser_(expressionParser),
parameterValidator_(new ParameterValidator(expressionParser, QSharedPointer<QList<QSharedPointer<Choice> > >())),
choiceValidator_(new ChoiceValidator(expressionParser))
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
    if (busDefinition->getVlnv().isValid() == false)
	{
		return false;
	}

    if (!extendIsValid(busDefinition))
    {
        return false;
    }
    
    bool validMasters = false;
    expressionParser_->parseExpression(busDefinition->getMaxMasters(), &validMasters);
    if (validMasters == false)
	{
		return false;
	}
    
    bool validSlaves = false;
    expressionParser_->parseExpression(busDefinition->getMaxSlaves(), &validSlaves);
    if (validSlaves == false)
	{
		return false;
	}

 	for (auto const& currentParameter : *busDefinition->getParameters())
	{
        if (parameterValidator_->validate(currentParameter) == false)
		{
			return false;
		}
	}

    for (auto const& currentChoice : *busDefinition->getChoices())
    {
        if (!choiceValidator_->validate(currentChoice))
        {
            return false;
        }
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: BusDefinitionValidator::extendIsValid()
//-----------------------------------------------------------------------------
bool BusDefinitionValidator::extendIsValid(QSharedPointer<const BusDefinition> busDefinition) const
{
    VLNV extendVLNV = busDefinition->getExtends();
    return extendVLNV.isEmpty() ||
        (library_->getModelReadOnly(extendVLNV) && extendVLNV.getType() == VLNV::BUSDEFINITION);
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

    if (!extendIsValid(busDefinition))
    {
        errors.append(QObject::tr("Extend VLNV '%1' is not valid within %2.").arg(
            busDefinition->getExtends().toString(), context));
    }

    bool initiatorsValid = false;
    expressionParser_->parseExpression(busDefinition->getMaxInitiators(), &initiatorsValid);
	if (initiatorsValid == false)
	{
		errors.append(QObject::tr("MaxInitiators '%1' is not a valid expression within %2.").arg(
            busDefinition->getMaxInitiators(), context));
	}
    
    bool targetsValid = false;
	expressionParser_->parseExpression(busDefinition->getMaxTargets(), &targetsValid);
    if (targetsValid == false)
	{
		errors.append(QObject::tr("MaxTargets '%1' is not a valid expression within %2.").arg(
            busDefinition->getMaxTargets(), context));
	}

    for (auto const& currentParameter : *busDefinition->getParameters())
    {
        parameterValidator_->findErrorsIn(errors, currentParameter, context);
    }

    for (auto const& currentChoice : *busDefinition->getChoices())
    {
        choiceValidator_->findErrorsIn(errors, currentChoice, context);
    }
}
