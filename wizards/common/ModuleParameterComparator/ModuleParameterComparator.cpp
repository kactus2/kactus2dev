//-----------------------------------------------------------------------------
// File: ModuleParameterComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in lists of module parameters.
//-----------------------------------------------------------------------------

#include "ModuleParameterComparator.h"

#include <IPXACTmodels/common/ModuleParameter.h>

//-----------------------------------------------------------------------------
// Function: ModuleParameterComparator::ModuleParameterComparator()
//-----------------------------------------------------------------------------
ModuleParameterComparator::ModuleParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterComparator::()
//-----------------------------------------------------------------------------
ModuleParameterComparator::~ModuleParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ModuleParameterComparator::compare()
//-----------------------------------------------------------------------------
bool ModuleParameterComparator::compare(QSharedPointer<ModuleParameter> first, 
    QSharedPointer<ModuleParameter> second) const
{
    return IPXactElementComparator::compare(first, second);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterComparator::compareFields()
//-----------------------------------------------------------------------------
bool ModuleParameterComparator::compareFields(QSharedPointer<const ModuleParameter> first, 
    QSharedPointer<const ModuleParameter> second) const
{
    return first->getValue() == second->getValue() &&
        first->getDataType() == second->getDataType() &&
        first->getUsageType() == second->getUsageType();
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterComparator::compare()
//-----------------------------------------------------------------------------
bool ModuleParameterComparator::compare(QList<QSharedPointer<ModuleParameter> > const references,
    QList<QSharedPointer<ModuleParameter> > const subjects)
{
    return ListComparator::compare(references, subjects);
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ModuleParameterComparator::diffFields(
    QSharedPointer<const ModuleParameter> reference, QSharedPointer<const ModuleParameter> subject) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    QSharedPointer<IPXactDiff> modification(new IPXactDiff(elementType(), reference->name()));
    modification->setChangeType(IPXactDiff::MODIFICATION);

    modification->checkForChange("description", reference->description(), subject->description());
    modification->checkForChange("type", reference->getType(), subject->getType());
    modification->checkForChange("value", reference->getValue(), subject->getValue());
    modification->checkForChange("data type", reference->getDataType(), subject->getDataType());
    modification->checkForChange("minimum value", reference->getMinimumValue(), subject->getMinimumValue());
    modification->checkForChange("maximum value", reference->getMaximumValue(), subject->getMaximumValue());
    modification->checkForChange("usage type", reference->getUsageType(), subject->getUsageType());
    modification->checkForChange("resolve", reference->getValueResolve(), subject->getValueResolve());

    diffResult.append(modification);
    return diffResult;
}

//-----------------------------------------------------------------------------
// Function: ModuleParameterComparator::elementType()
//-----------------------------------------------------------------------------
QString ModuleParameterComparator::elementType() const
{
    return "model parameter";
}
