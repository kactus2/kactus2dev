//-----------------------------------------------------------------------------
// File: ParameterReferenceCounter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.01.2015
//
// Description:
// Calculates the amount of references made to parameters and gives this data to the parameters.
//-----------------------------------------------------------------------------

#include "ParameterReferenceCounter.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::ParameterReferenceCounter()
//-----------------------------------------------------------------------------
ParameterReferenceCounter::ParameterReferenceCounter(QSharedPointer<ParameterFinder> parameterFinder):
parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::~ParameterReferenceCounter()
//-----------------------------------------------------------------------------
ParameterReferenceCounter::~ParameterReferenceCounter()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::increaseReferenceCount()
//-----------------------------------------------------------------------------
void ParameterReferenceCounter::increaseReferenceCount(QString id)
{
    QSharedPointer<Parameter> targetParameter = parameterFinder_->getParameterWithID(id);

    if (targetParameter)
    {
        targetParameter->increaseUsageCount();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::decreaseReferenceCount()
//-----------------------------------------------------------------------------
void ParameterReferenceCounter::decreaseReferenceCount(QString id)
{
    QSharedPointer<Parameter> targetParameter = parameterFinder_->getParameterWithID(id);

    if (targetParameter)
    {
        targetParameter->decreaseUsageCount();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::recalculateReferencesToParameters()
//-----------------------------------------------------------------------------
void ParameterReferenceCounter::recalculateReferencesToParameters(QVector<QSharedPointer<Parameter> > parameterList)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::countReferencesInParameters()
//-----------------------------------------------------------------------------
int ParameterReferenceCounter::countReferencesInParameters(QString const& parameterID,
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters) const
{
    int referenceCount = 0;

    foreach (QSharedPointer<Parameter> singleParameter, *parameters)
    {
        referenceCount += countReferencesInSingleParameter(parameterID, singleParameter);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::countReferencesInSingleParameter()
//-----------------------------------------------------------------------------
int ParameterReferenceCounter::countReferencesInSingleParameter(QString const& parameterID,
    QSharedPointer<Parameter> parameter) const
{
    int referenceCount = 0;

    referenceCount += countReferencesInExpression(parameterID, parameter->getValue());
    referenceCount += countReferencesInExpression(parameterID, parameter->getVectorLeft());
    referenceCount += countReferencesInExpression(parameterID, parameter->getVectorRight());
    referenceCount +=
        countReferencesInExpression(parameterID, parameter->getAttribute(QLatin1String("kactus2:arrayLeft")));
    referenceCount +=
        countReferencesInExpression(parameterID, parameter->getAttribute(QLatin1String("kactus2:arrayRight")));

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::countReferencesInConfigurableElementValues()
//-----------------------------------------------------------------------------
int ParameterReferenceCounter::countReferencesInConfigurableElementValues(QString const& parameterId,
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > configurableElements) const
{
    int referenceCount = 0;

    foreach (QSharedPointer<ConfigurableElementValue> element, *configurableElements)
    {
        referenceCount += countReferencesInSingleConfigurableElementValue(parameterId, element);
    }

    return referenceCount;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::countReferencesInSingleConfigurableElementValue()
//-----------------------------------------------------------------------------
int ParameterReferenceCounter::countReferencesInSingleConfigurableElementValue(QString const& parameterID,
    QSharedPointer<ConfigurableElementValue> element) const
{
    return countReferencesInExpression(parameterID, element->getConfigurableValue());
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::countReferencesInExpression()
//-----------------------------------------------------------------------------
int ParameterReferenceCounter::countReferencesInExpression(QString const& parameterID,
    QString const& expression) const
{
    return expression.count(parameterID);
}
