//-----------------------------------------------------------------------------
// File: ParameterReferenceCounter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.01.2015
//
// Description:
// Calculates the amount of references made to parameters and gives this data to the parameters.
//-----------------------------------------------------------------------------

#include "ParameterReferenceCounter.h"

#include <IPXACTmodels/parameter.h>

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

    targetParameter->increaseUsageCount();
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceCounter::decreaseReferenceCount()
//-----------------------------------------------------------------------------
void ParameterReferenceCounter::decreaseReferenceCount(QString id)
{
    QSharedPointer<Parameter> targetParameter = parameterFinder_->getParameterWithID(id);

    targetParameter->decreaseUsageCount();
}