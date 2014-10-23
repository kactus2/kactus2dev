//-----------------------------------------------------------------------------
// File: ComponentComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.10.2014
//
// Description:
// Comparator for finding differences in components.
//-----------------------------------------------------------------------------

#include "ComponentComparator.h"
#include <wizards/common/VLNVComparator/VLNVComparator.h>
#include <wizards/common/ModelParameterComparator/ModelParameterComparator.h>
#include <wizards/common/ViewComparator/ViewComparator.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/vlnv.h>

//-----------------------------------------------------------------------------
// Function: ComponentComparator::ComponentComparator()
//-----------------------------------------------------------------------------
ComponentComparator::ComponentComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::~ComponentComparator()
//-----------------------------------------------------------------------------
ComponentComparator::~ComponentComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ComponentComparator::diff(QSharedPointer<const Component> reference, 
    QSharedPointer<const Component> subject) const
{
    return IPXactElementComparator::diff(reference, subject);
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareVLNVs()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareVLNVs(QSharedPointer<const Component> referenceComponent,
    QSharedPointer<const Component> subjectComponent) const
{   
    QSharedPointer<VLNV> referenceVLNV(new VLNV(*referenceComponent->getVlnv()));
    QSharedPointer<VLNV> subjectVLNV(new VLNV(*subjectComponent->getVlnv()));

    VLNVComparator vlnvComparator;
    return vlnvComparator.compare(referenceVLNV, subjectVLNV);
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareModelParameters()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareModelParameters(QSharedPointer<const Component> referenceComponent, 
    QSharedPointer<const Component> subjectComponent) const
{
    ModelParameterComparator modelParameterComparator;
    return modelParameterComparator.compare(referenceComponent->getModelParameters(),
        subjectComponent->getModelParameters());
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareViews()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareViews(QSharedPointer<const Component> first,
    QSharedPointer<const Component> second) const
{
    ViewComparator viewComparator;
    return viewComparator.compare(first->getViews(), second->getViews());
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::elementType()
//-----------------------------------------------------------------------------
QString ComponentComparator::elementType() const
{
    return "component";
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareFields()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareFields(QSharedPointer<const Component> first,
    QSharedPointer<const Component> second) const
{
    return compareVLNVs(first, second) &&
        compareModelParameters(first, second) &&
        compareViews(first, second);
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ComponentComparator::diffFields(QSharedPointer<const Component> reference, 
    QSharedPointer<const Component> subject) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    if (!compareVLNVs(reference, subject))
    {
        VLNVComparator vlnvComparator;
        QSharedPointer<VLNV> referenceVLNV(new VLNV(*reference->getVlnv()));
        QSharedPointer<VLNV> otherVLNV(new VLNV(*subject->getVlnv()));
        diffResult.append(vlnvComparator.diff(referenceVLNV, otherVLNV));
    }

    if (!compareModelParameters(reference, subject))
    {
        ModelParameterComparator modelParameterComparator;
        diffResult.append(modelParameterComparator.diff(reference->getModelParameters(), 
            subject->getModelParameters()));
    }

    if (!compareViews(reference, subject))
    {
        ViewComparator viewComparator;
        diffResult.append(viewComparator.diff(reference->getViews(), subject->getViews()));
    }

    return diffResult;
}
