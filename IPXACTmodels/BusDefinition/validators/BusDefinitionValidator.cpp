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
    
    bool validInitiators = false;
    expressionParser_->parseExpression(QString::fromStdString(busDefinition->getMaxInitiators()), &validInitiators);
    if (validInitiators == false)
	{
		return false;
	}
    
    bool validTargets = false;
    expressionParser_->parseExpression(QString::fromStdString(busDefinition->getMaxSlaves()), &validTargets);
    if (validTargets == false)
	{
		return false;
	}

    ParameterValidator parameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >(), busDefinition->getRevision());

 	for (auto const& currentParameter : *busDefinition->getParameters())
	{
        if (parameterValidator.validate(currentParameter) == false)
		{
			return false;
		}
	}

    if (busDefinition->getRevision() != Document::Revision::Std22)
    {
        if (!busDefinition->getChoices()->isEmpty())
        {
            return false;
        }

        if (!busDefinition->getShortDescription().isEmpty())
        {
            return false;
        }

        if (!busDefinition->getDisplayName().isEmpty())
        {
            return false;
        }
    }
    else
    {
        for (auto const& currentChoice : *busDefinition->getChoices())
        {
            if (!choiceValidator_->validate(currentChoice))
            {
                return false;
            }
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
    expressionParser_->parseExpression(QString::fromStdString(busDefinition->getMaxInitiators()),
        &initiatorsValid);

	if (initiatorsValid == false)
	{
		errors.append(QObject::tr("MaxInitiators '%1' is not a valid expression within %2.").arg(
            QString::fromStdString(busDefinition->getMaxInitiators()), context));
	}
    
    bool targetsValid = false;
    expressionParser_->parseExpression(QString::fromStdString(busDefinition->getMaxTargets()), &targetsValid);
    if (targetsValid == false)
	{
		errors.append(QObject::tr("MaxTargets '%1' is not a valid expression within %2.").arg(
            QString::fromStdString(busDefinition->getMaxTargets()), context));
	}

    ParameterValidator parameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >(), busDefinition->getRevision());

    for (auto const& currentParameter : *busDefinition->getParameters())
    {
        parameterValidator.findErrorsIn(errors, currentParameter, context);
    }

    if (!busDefinition->getChoices()->isEmpty()
        && busDefinition->getRevision() != Document::Revision::Std22)
    {
        errors.append(QObject::tr("Choices within %1 not supported by standard revision.").arg(context));
    }

    for (auto const& currentChoice : *busDefinition->getChoices())
    {
        choiceValidator_->findErrorsIn(errors, currentChoice, context);
    }
}
