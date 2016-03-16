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
#include <wizards/common/ParameterComparator/ParameterComparator.h>
#include <wizards/common/PortComparator/PortComparator.h>
#include <wizards/common/ViewComparator/ViewComparator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>

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
    QSharedPointer<VLNV> referenceVLNV(new VLNV(referenceComponent->getVlnv()));
    QSharedPointer<VLNV> subjectVLNV(new VLNV(subjectComponent->getVlnv()));

    VLNVComparator vlnvComparator;
    return vlnvComparator.compare(referenceVLNV, subjectVLNV);
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareParameters()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareParameters(QSharedPointer<const Component> referenceComponent, 
    QSharedPointer<const Component> subjectComponent) const
{
    ParameterComparator parameterComparator;
    return parameterComparator.compare(*referenceComponent->getParameters(), *subjectComponent->getParameters());
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::comparePorts()
//-----------------------------------------------------------------------------
bool ComponentComparator::comparePorts(QSharedPointer<const Component> first, 
    QSharedPointer<const Component> second) const
{
    PortComparator portComparator;
    return portComparator.compare(*first->getPorts().data(), *second->getPorts().data());
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareViews()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareViews(QSharedPointer<const Component> first,
    QSharedPointer<const Component> second) const
{
    ViewComparator viewComparator;
    return viewComparator.compare(*first->getViews().data(), *second->getViews().data());
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::elementType()
//-----------------------------------------------------------------------------
QString ComponentComparator::elementType() const
{
    return QObject::tr("component");
}

//-----------------------------------------------------------------------------
// Function: ComponentComparator::compareFields()
//-----------------------------------------------------------------------------
bool ComponentComparator::compareFields(QSharedPointer<const Component> first,
    QSharedPointer<const Component> second) const
{
    return compareVLNVs(first, second) &&
        comparePorts(first, second) &&
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
        QSharedPointer<VLNV> referenceVLNV(new VLNV(reference->getVlnv()));
        QSharedPointer<VLNV> otherVLNV(new VLNV(subject->getVlnv()));
        diffResult.append(vlnvComparator.diff(referenceVLNV, otherVLNV));
    }

    if (!compareParameters(reference, subject))
    {
        ParameterComparator parameterComparator;
        diffResult.append(parameterComparator.diff(*reference->getParameters(), *subject->getParameters()));
    }

    if (!comparePorts(reference, subject))
    {
        PortComparator portComparator;
        diffResult.append(portComparator.diff(*reference->getPorts().data(), *subject->getPorts().data()));
    }

    if (!compareViews(reference, subject))
    {
        ViewComparator viewComparator;
        diffResult.append(viewComparator.diff(*reference->getViews().data(), *subject->getViews().data()));
    }

    return diffResult;
}
