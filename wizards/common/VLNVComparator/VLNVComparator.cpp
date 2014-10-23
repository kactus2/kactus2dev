//-----------------------------------------------------------------------------
// File: VLNVComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in VLVN elements.
//-----------------------------------------------------------------------------

#include "VLNVComparator.h"

#include <IPXACTmodels/vlnv.h>

//-----------------------------------------------------------------------------
// Function: VLNVComparator::VLNVComparator()
//-----------------------------------------------------------------------------
VLNVComparator::VLNVComparator()
{

}

//-----------------------------------------------------------------------------
// Function: VLNVComparator::~VLNVComparator()
//-----------------------------------------------------------------------------
VLNVComparator::~VLNVComparator()
{

}

//-----------------------------------------------------------------------------
// Function: VLNVComparator::elementType()
//-----------------------------------------------------------------------------
QString VLNVComparator::elementType() const
{
    return "vlnv";
}

//-----------------------------------------------------------------------------
// Function: VLNVComparator::compareFields()
//-----------------------------------------------------------------------------
bool VLNVComparator::compareFields(QSharedPointer<const VLNV> first, QSharedPointer<const VLNV> second) const
{
    return *first == *second;
}

//-----------------------------------------------------------------------------
// Function: VLNVComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > VLNVComparator::diffFields(QSharedPointer<const VLNV> reference, 
    QSharedPointer<const VLNV> subject) const
{   
    if (reference->isEmpty())
    {
        return addDiff();    
    }

    if (subject->isEmpty())
    {
        return removeDiff();
    }

    QList<QSharedPointer<IPXactDiff> > changeDiff;

    QSharedPointer<IPXactDiff> vlnvDiff(new IPXactDiff(elementType()));
    vlnvDiff->setChangeType(IPXactDiff::MODIFICATION);

    vlnvDiff->checkForChange("vlnv", reference->toString(), subject->toString());

    changeDiff.append(vlnvDiff);
    return changeDiff;
}
