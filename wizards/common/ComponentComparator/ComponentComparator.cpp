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
#include "../VLNVComparator/VLNVComparator.h"
#include "../ModelParameterComparator/ModelParameterComparator.h"

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
// Function: ComponentComparator::compare()
//-----------------------------------------------------------------------------
bool ComponentComparator::compare(QSharedPointer<const Component> first,
    QSharedPointer<const Component> second) const
{
    if (first == second)
    {
        return true;
    }

    if (first.isNull() || second.isNull())
    {
        return false;
    }

    return compareVLNVs(first, second) &&
        compareModelParameters(first, second);
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ComponentComparator::diff(QSharedPointer<const Component> reference, 
    QSharedPointer<const Component> subject) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    if (compare(reference, subject))
    {
        QSharedPointer<IPXactDiff> noChanges(new IPXactDiff("component"));
        noChanges->setChangeType(IPXactDiff::NO_CHANGE);

        QList<QSharedPointer<IPXactDiff> > diffResult;
        diffResult.append(noChanges);
        return diffResult;
    } 
   
    if (subject.isNull())
    {
        QSharedPointer<IPXactDiff> remove(new IPXactDiff("component"));
        remove->setChangeType(IPXactDiff::REMOVE);

        QList<QSharedPointer<IPXactDiff> > diffResult;
        diffResult.append(remove);
        return diffResult;
    }    

    if (reference.isNull())
    { 
        QSharedPointer<IPXactDiff> add(new IPXactDiff("component"));
        add->setChangeType(IPXactDiff::ADD);
        diffResult.append(add);
    }

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

    return diffResult;
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareVLNVs()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareVLNVs(QSharedPointer<const Component> referenceComponent,
    QSharedPointer<const Component> subjectComponent) const
{   
    QSharedPointer<VLNV> referenceVLNV(new VLNV());
    
    if (!referenceComponent.isNull())
    {
        referenceVLNV = QSharedPointer<VLNV>(new VLNV(*referenceComponent->getVlnv()));
    }

    QSharedPointer<VLNV> otherVLNV(new VLNV(*subjectComponent->getVlnv()));

    VLNVComparator vlnvComparator;
    return vlnvComparator.compare(referenceVLNV, otherVLNV);
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareModelParameters()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareModelParameters(QSharedPointer<const Component> referenceComponent, 
    QSharedPointer<const Component> subjectComponent) const
{
    QList<QSharedPointer<ModelParameter> > referenceModelParameters;
    if (!referenceComponent.isNull())
    {
        referenceModelParameters = referenceComponent->getModelParameters();
    }
    QList<QSharedPointer<ModelParameter> > otherModelParameters = subjectComponent->getModelParameters();

    ModelParameterComparator modelParameterComparator;

    return modelParameterComparator.compare(referenceModelParameters, otherModelParameters);
}
