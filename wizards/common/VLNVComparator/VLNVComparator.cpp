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
// Function: VLNVComparator::compare()
//-----------------------------------------------------------------------------
bool VLNVComparator::compare(QSharedPointer<const VLNV> first, QSharedPointer<const VLNV> second) const
{
    if (first.isNull() && second.isNull())
    {
        return true;
    }

    if (first.isNull() || second.isNull())
    {
        return false;
    }

    return *first.data() == *second.data();
}

//-----------------------------------------------------------------------------
// Function: VLNVComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > VLNVComparator::diff(QSharedPointer<const VLNV> reference, 
    QSharedPointer<const VLNV> subject) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    QSharedPointer<IPXactDiff> vlnvDiff(new IPXactDiff("vlnv", reference->toString()));

    if (compare(reference, subject))
    {
        vlnvDiff->setChangeType(IPXactDiff::NO_CHANGE);
    }
    else if (reference->isEmpty() && !subject->isEmpty())
    {
        vlnvDiff->setChangeType(IPXactDiff::ADD);
    }
    else if (!reference->isEmpty() && subject->isEmpty())
    {
        vlnvDiff->setChangeType(IPXactDiff::REMOVE);
    }
    else
    {
        vlnvDiff->setChangeType(IPXactDiff::MODIFICATION);

        vlnvDiff->checkForChange("vendor", reference->getVendor(), subject->getVendor());
        vlnvDiff->checkForChange("library", reference->getLibrary(), subject->getLibrary());
        vlnvDiff->checkForChange("name", reference->getName(), subject->getName());
        vlnvDiff->checkForChange("version", reference->getVersion(), subject->getVersion());       
    }

    diffResult.append(vlnvDiff);

    return diffResult;
}
