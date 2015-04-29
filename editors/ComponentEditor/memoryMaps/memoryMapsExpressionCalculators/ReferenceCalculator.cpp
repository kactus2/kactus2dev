//-----------------------------------------------------------------------------
// File: ReferenceCalculator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Calculates the usage of parameters in a given list of expressions.
//-----------------------------------------------------------------------------

#include "ReferenceCalculator.h"

//-----------------------------------------------------------------------------
// Function: ReferenceCalculator::ReferenceCalculator()
//-----------------------------------------------------------------------------
ReferenceCalculator::ReferenceCalculator(QSharedPointer<ParameterFinder> parameterFinder):
parameterFinder_(parameterFinder)
{

}

//-----------------------------------------------------------------------------
// Function: ReferenceCalculator::~ReferenceCalculator()
//-----------------------------------------------------------------------------
ReferenceCalculator::~ReferenceCalculator()
{

}

//-----------------------------------------------------------------------------
// Function: ReferenceCalculator::getReferencedParameters()
//-----------------------------------------------------------------------------
QMap<QString, int> ReferenceCalculator::getReferencedParameters(QStringList listOfPossibleReferences) const
{
    QMap<QString, int> referencedParameters;

    QStringList componentParameterIds = parameterFinder_->getAllParameterIds();
    QString joinedPossibleReferences = listOfPossibleReferences.join(" ");

    foreach (QString parameterId, componentParameterIds)
    {
        int numberOfFound = joinedPossibleReferences.count(parameterId);
        if (numberOfFound > 0)
        {
            referencedParameters.insert(parameterId, numberOfFound);
        }
    }

    return referencedParameters;
}
