//-----------------------------------------------------------------------------
// File: AccessHandle.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.06.2026
//
// Description:
// Describes the ipxact:accessHandle element.
//-----------------------------------------------------------------------------

#include "AccessHandle.h"

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: AccessHandle::AccessHandle()
//-----------------------------------------------------------------------------
AccessHandle::AccessHandle()
{
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::AccessHandle()
//-----------------------------------------------------------------------------
AccessHandle::AccessHandle(AccessHandle const& other) :
    Extendable(other),
    force_(other.force_),
    indices_(other.indices_),
    viewRefs_(other.viewRefs_)
{
    Copy::copyList(other.slices_, slices_);
    Copy::copyList(other.pathSegments_, pathSegments_);
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::operator=()
//-----------------------------------------------------------------------------
AccessHandle & AccessHandle::operator=(AccessHandle const& other)
{
    if (this != &other)
    {
        Extendable::operator=(other);

        force_ = other.force_;
        indices_ = other.indices_;
        viewRefs_ = other.viewRefs_;

        Copy::copyList(other.slices_, slices_);
        Copy::copyList(other.pathSegments_, pathSegments_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::setForce()
//-----------------------------------------------------------------------------
void AccessHandle::setForce(bool force)
{
    force_.setValue(force);
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::getForce()
//-----------------------------------------------------------------------------
QString AccessHandle::getForce() const
{
    return force_.toString();
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::setIndices()
//-----------------------------------------------------------------------------
void AccessHandle::setIndices(QStringList const& indices)
{
    indices_ = indices;
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::getIndices()
//-----------------------------------------------------------------------------
QStringList AccessHandle::getIndices() const
{
    return indices_;
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::setSlices()
//-----------------------------------------------------------------------------
void AccessHandle::setSlices(QSharedPointer<QList<QSharedPointer<Slice> > > slices)
{
    slices_ = slices;
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::getSlices()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Slice> > > AccessHandle::getSlices() const
{
    return slices_;
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::setViewReferences()
//-----------------------------------------------------------------------------
void AccessHandle::setViewReferences(QStringList const& viewRefs)
{
    viewRefs_ = viewRefs;
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::getViewReferences()
//-----------------------------------------------------------------------------
QStringList AccessHandle::getViewReferences() const
{
    return viewRefs_;
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::setPathSegments()
//-----------------------------------------------------------------------------
void AccessHandle::setPathSegments(QSharedPointer<QList<QSharedPointer<PathSegment> > > pathSegments)
{
    pathSegments_ = pathSegments;
}

//-----------------------------------------------------------------------------
// Function: AccessHandle::getPathSegments()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<PathSegment> > > AccessHandle::getPathSegments() const
{
    return pathSegments_;
}
