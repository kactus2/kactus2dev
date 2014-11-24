//-----------------------------------------------------------------------------
// File: ModelParameterValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.11.2014
//
// Description:
// Validator for the IP-XACT Model Parameter element.
//-----------------------------------------------------------------------------

#include "ModelParameterValidator.h"

#include <IPXACTmodels/modelparameter.h>

//-----------------------------------------------------------------------------
// Function: ModelParameterValidator::ModelParameterValidator()
//-----------------------------------------------------------------------------
ModelParameterValidator::ModelParameterValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterValidator::~ModelParameterValidator()
//-----------------------------------------------------------------------------
ModelParameterValidator::~ModelParameterValidator()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterValidator::validate()
//-----------------------------------------------------------------------------
bool ModelParameterValidator::validate(ModelParameter const* modelParameter,
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    return ParameterValidator::validate(modelParameter, availableChoices) && hasValidUsageType(modelParameter);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterValidator::hasValidUsageType()
//-----------------------------------------------------------------------------
bool ModelParameterValidator::hasValidUsageType(ModelParameter const* modelParameter) const
{
    QString usageType = modelParameter->getAttribute("spirit:usageType");

    return usageType.isEmpty() || usageType == "nontyped" || usageType =="typed";
}

//-----------------------------------------------------------------------------
// Function: ModelParameterValidator::findErrorsIn()
//-----------------------------------------------------------------------------
QStringList ModelParameterValidator::findErrorsIn(ModelParameter const* modelParameter, 
    QString const& context, QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList errors = ParameterValidator::findErrorsIn(modelParameter, context, availableChoices);

    errors.append(findErrorsInUsageType(modelParameter, context));

    return errors;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterValidator::findErrorsInUsageType()
//-----------------------------------------------------------------------------
QStringList ModelParameterValidator::findErrorsInUsageType(ModelParameter const* modelParameter, 
    QString const& context) const
{
    QStringList usageErrors;
    if (!hasValidUsageType(modelParameter)) 
    {
        usageErrors.append(QObject::tr("Invalid usage type set for %1 %2 within %3"
            ).arg(modelParameter->elementName(), modelParameter->getName(), context));
    }

    return usageErrors;
}
