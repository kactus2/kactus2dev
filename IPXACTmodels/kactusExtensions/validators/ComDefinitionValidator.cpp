//-----------------------------------------------------------------------------
// File: ComDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// Validator for kactus2:ComDefinition.
//-----------------------------------------------------------------------------

#include "ComDefinitionValidator.h"

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>

//-----------------------------------------------------------------------------
// Function: ComDefinitionValidator::ComDefinitionValidator()
//-----------------------------------------------------------------------------
ComDefinitionValidator::ComDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ComDefinitionValidator::~ComDefinitionValidator()
//-----------------------------------------------------------------------------
ComDefinitionValidator::~ComDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ComDefinitionValidator::validate()
//-----------------------------------------------------------------------------
bool ComDefinitionValidator::validate(QSharedPointer<const ComDefinition> comDefinition) const
{
	if (!comDefinition->getVlnv().isValid())
	{
		return false;
	}

	// Check for multiple definitions of same transfer type.
	QStringList transferTypeNames;

	foreach (QString const& transferType, *comDefinition->getTransferTypes())
	{
		if (transferTypeNames.contains(transferType))
		{
			return false;
		}
		else
		{
			transferTypeNames.push_back(transferType);
		}
	}

	// Validate the properties.
	QStringList propertyNames;

	foreach (QSharedPointer<ComProperty> prop, *comDefinition->getProperties())
	{
		if (propertyNames.contains(prop->name()))
		{
			return false;
		}
		else
		{
			propertyNames.append(prop->name());
		}

		if (!prop->validate())
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComDefinitionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ComDefinitionValidator::findErrorsIn(QVector<QString>& errorList,
    QSharedPointer<const ComDefinition> comDefinition) const
{
	QString thisIdentifier(QObject::tr("the containing COM definition"));

	if (comDefinition->getVlnv().isValid(errorList, thisIdentifier))
	{
		thisIdentifier = QObject::tr("COM definition '%1'").arg(comDefinition->getVlnv().toString());
	}

	// Check for multiple definitions of same data type.
	QStringList dataTypeNames;
	foreach (QString const& dataType, *comDefinition->getTransferTypes())
	{
		if (dataTypeNames.contains(dataType))
		{
			errorList.append(QObject::tr("Data type '%1' defined multiple times in '%2'").arg(dataType, 
				thisIdentifier));
		}
		else
		{
			dataTypeNames.push_back(dataType);
		}
	}

	// Validate the properties.
	QStringList propertyNames;
	foreach (QSharedPointer<ComProperty> comProperty, *comDefinition->getProperties())
	{
		if (propertyNames.contains(comProperty->name()))
		{
			errorList.append(QObject::tr("Property '%1' defined multiple times in %2").arg(comProperty->name(),
				thisIdentifier));
		}
		else
		{
			propertyNames.append(comProperty->name());
		}

		comProperty->findErrors(errorList, thisIdentifier);
	}
}
