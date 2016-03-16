//-----------------------------------------------------------------------------
// File: ApiDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// Validator for kactus2:ApiDefinition.
//-----------------------------------------------------------------------------

#include "ApiDefinitionValidator.h"

#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>

//-----------------------------------------------------------------------------
// Function: ApiDefinitionValidator::ApiDefinitionValidator()
//-----------------------------------------------------------------------------
ApiDefinitionValidator::ApiDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionValidator::~ApiDefinitionValidator()
//-----------------------------------------------------------------------------
ApiDefinitionValidator::~ApiDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionValidator::validate()
//-----------------------------------------------------------------------------
bool ApiDefinitionValidator::validate(QSharedPointer<const ApiDefinition> apiDefinition) const
{
	if (!apiDefinition->getVlnv().isValid() )
	{
		return false;
	}

	// Check that the COM definition reference is valid.
	if (!apiDefinition->getComDefinitionRef().isEmpty() &&
		!apiDefinition->getComDefinitionRef().isValid())
	{
		return false;
	}

	// Check for multiple definitions of same data type.
	QStringList dataTypeNames;

	foreach (QString const& dataType, *apiDefinition->getDataTypes())
	{
		if (dataTypeNames.contains(dataType))
		{
			return false;
		}
		else
		{
			dataTypeNames.push_back(dataType);
		}
	}

	// Validate the functions.
	QStringList funcNames;

	foreach (QSharedPointer<ApiFunction> func, *apiDefinition->getFunctions())
	{
		if (funcNames.contains(func->name()))
		{
			return false;
		}
		else
		{
			funcNames.append(func->name());
		}

		if (!func->validate())
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinitionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void ApiDefinitionValidator::findErrorsIn(QVector<QString>& errorList,
    QSharedPointer<const ApiDefinition> apiDefinition) const
{
	QString thisIdentifier(QObject::tr("the containing API definition"));

	if (apiDefinition->getVlnv().isValid(errorList, thisIdentifier))
	{
		thisIdentifier = QObject::tr("API definition '%1'").arg(apiDefinition->getVlnv().toString());
	}

	// Check that the COM definition reference is valid.
	if (!apiDefinition->getComDefinitionRef().isEmpty())
	{
		apiDefinition->getComDefinitionRef().isValid(errorList, thisIdentifier);
	}

	// Check for multiple definitions of same data type.
	QStringList dataTypeNames;

	foreach (QString const& dataType, *apiDefinition->getDataTypes())
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

	// Validate the functions.
	QStringList funcNames;

	foreach (QSharedPointer<ApiFunction> func, *apiDefinition->getFunctions())
	{
		if (funcNames.contains(func->name()))
		{
			errorList.append(QObject::tr("Function with name '%1' defined multiple times in %2").arg(func->name(),
				thisIdentifier));
		}
		else
		{
			funcNames.append(func->name());
		}

		func->findErrors(errorList, thisIdentifier);
	}
}
