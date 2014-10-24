//-----------------------------------------------------------------------------
// File: ModelParameterComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------

#include "ModelParameterComparator.h"

#include <IPXACTmodels/modelparameter.h>

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::()
//-----------------------------------------------------------------------------
ModelParameterComparator::ModelParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::()
//-----------------------------------------------------------------------------
ModelParameterComparator::~ModelParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::compare()
//-----------------------------------------------------------------------------
bool ModelParameterComparator::compare(QSharedPointer<ModelParameter> first, 
    QSharedPointer<ModelParameter> second) const
{
    return IPXactElementComparator::compare(first, second);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::compareFields()
//-----------------------------------------------------------------------------
bool ModelParameterComparator::compareFields(QSharedPointer<const ModelParameter> first, 
    QSharedPointer<const ModelParameter> second) const
{
    return first->getValue() == second->getValue() &&
        first->getDataType() == second->getDataType() &&
        first->getUsageType() == second->getUsageType();
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::compare()
//-----------------------------------------------------------------------------
bool ModelParameterComparator::compare(QList<QSharedPointer<ModelParameter> > const references,
    QList<QSharedPointer<ModelParameter> > const subjects)
{
    return ListComparator::compare(references, subjects);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ModelParameterComparator::diffFields(QSharedPointer<const ModelParameter> reference, 
    QSharedPointer<const ModelParameter> subject) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    QSharedPointer<IPXactDiff> modification(new IPXactDiff(elementType(), reference->getName()));
    modification->setChangeType(IPXactDiff::MODIFICATION);

    modification->checkForChange("value", reference->getValue(), subject->getValue());
    modification->checkForChange("data type", reference->getDataType(), subject->getDataType());
    modification->checkForChange("usage type", reference->getUsageType(), subject->getUsageType());

    diffResult.append(modification);
    return diffResult;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::elementType()
//-----------------------------------------------------------------------------
QString ModelParameterComparator::elementType() const
{
    return "model parameter";
}
