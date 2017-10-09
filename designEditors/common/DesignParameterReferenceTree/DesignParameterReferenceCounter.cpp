//-----------------------------------------------------------------------------
// File: DesignParameterReferenceCounter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.10.2017
//
// Description:
// Calculates the amount of references made to design parameters.
//-----------------------------------------------------------------------------

#include "DesignParameterReferenceCounter.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::DesignParameterReferenceCounter()
//-----------------------------------------------------------------------------
DesignParameterReferenceCounter::DesignParameterReferenceCounter(QSharedPointer<ParameterFinder> parameterFinder):
ParameterReferenceCounter(parameterFinder),
design_(0)
{

}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::~DesignParameterReferenceCounter()
//-----------------------------------------------------------------------------
DesignParameterReferenceCounter::~DesignParameterReferenceCounter()
{

}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::setDesign()
//-----------------------------------------------------------------------------
void DesignParameterReferenceCounter::setDesign(QSharedPointer<Design> newDesign)
{
    design_ = newDesign;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::recalculateReferencesToParameters()
//-----------------------------------------------------------------------------
void DesignParameterReferenceCounter::recalculateReferencesToParameters(
    QVector<QSharedPointer<Parameter> > parameterList)
{
    foreach (QSharedPointer<Parameter> parameter, parameterList)
    {
        int referenceCount = 0;
        QString parameterID = parameter->getValueId();

        referenceCount += countReferencesInParameters(parameterID, design_->getParameters());
        referenceCount += countReferencesInComponentInstances(parameterID);

        parameter->setUsageCount(referenceCount);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::countReferencesInComponentInstances()
//-----------------------------------------------------------------------------
int DesignParameterReferenceCounter::countReferencesInComponentInstances(QString const& parameterID) const
{
    int referenceCounter = 0;

    foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        referenceCounter += countReferencesInSingleComponentInstance(parameterID, instance);
    }

    return referenceCounter;
}

//-----------------------------------------------------------------------------
// Function: DesignParameterReferenceCounter::countReferencesInSingleComponentInstance()
//-----------------------------------------------------------------------------
int DesignParameterReferenceCounter::countReferencesInSingleComponentInstance(QString const& parameterID,
    QSharedPointer<ComponentInstance> instance) const
{
    return countReferencesInConfigurableElementValues(parameterID, instance->getConfigurableElementValues());
}
