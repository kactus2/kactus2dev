//-----------------------------------------------------------------------------
// File: ParameterComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.04.2015
//
// Description:
// Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------

#include "ParameterComparator.h"

#include <IPXACTmodels/parameter.h>

//-----------------------------------------------------------------------------
// Function: ParameterComparator::ParameterComparator()
//-----------------------------------------------------------------------------
ParameterComparator::ParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterComparator::ParameterComparator()
//-----------------------------------------------------------------------------
ParameterComparator::~ParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterComparator::compare()
//-----------------------------------------------------------------------------
bool ParameterComparator::compare(QSharedPointer<Parameter> first, 
    QSharedPointer<Parameter> second) const
{
    return IPXactElementComparator::compare(first, second);
}

//-----------------------------------------------------------------------------
// Function: ParameterComparator::compareFields()
//-----------------------------------------------------------------------------
bool ParameterComparator::compareFields(QSharedPointer<const Parameter> first, 
    QSharedPointer<const Parameter> second) const
{
    return first->getValue() == second->getValue();
}

//-----------------------------------------------------------------------------
// Function: ParameterComparator::compare()
//-----------------------------------------------------------------------------
bool ParameterComparator::compare(QList<QSharedPointer<Parameter> > const references,
    QList<QSharedPointer<Parameter> > const subjects)
{
    return ListComparator::compare(references, subjects);
}

//-----------------------------------------------------------------------------
// Function: ParameterComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ParameterComparator::diffFields(QSharedPointer<const Parameter> reference, 
    QSharedPointer<const Parameter> subject) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    QSharedPointer<IPXactDiff> modification(new IPXactDiff(elementType(), reference->getName()));
    modification->setChangeType(IPXactDiff::MODIFICATION);

    modification->checkForChange("description", reference->getDescription(), subject->getDescription());
    modification->checkForChange("type", reference->getType(), subject->getType());
    modification->checkForChange("value", reference->getValue(), subject->getValue());
    modification->checkForChange("minimum value", reference->getMinimumValue(), subject->getMinimumValue());
    modification->checkForChange("maximum value", reference->getMaximumValue(), subject->getMaximumValue());
    modification->checkForChange("resolve", reference->getValueResolve(), subject->getValueResolve());

    diffResult.append(modification);
    return diffResult;
}

//-----------------------------------------------------------------------------
// Function: ParameterComparator::elementType()
//-----------------------------------------------------------------------------
QString ParameterComparator::elementType() const
{
    return "parameter";
}
