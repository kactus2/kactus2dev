//-----------------------------------------------------------------------------
// File: AccessPolicy.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Describes the ipxact:accessPolicy element.
//-----------------------------------------------------------------------------

#include "AccessPolicy.h"

//-----------------------------------------------------------------------------
// Function: AccessPolicy::AccessPolicy()
//-----------------------------------------------------------------------------
AccessPolicy::AccessPolicy():
    Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: AccessPolicy::AccessPolicy()
//-----------------------------------------------------------------------------
AccessPolicy::AccessPolicy(AccessPolicy const& other) :
    Extendable(other),
    modeReferences_(new QList<QSharedPointer<ModeReference> >()),
    access_(other.access_)
{
    Utilities::copyList(modeReferences_, other.modeReferences_);
}

//-----------------------------------------------------------------------------
// Function: AccessPolicy::operator=()
//-----------------------------------------------------------------------------
AccessPolicy& AccessPolicy::operator=(AccessPolicy const& other)
{
    if (this != &other)
    {
        Extendable::operator=(other);
        access_ = other.access_;
        Utilities::copyList(modeReferences_, other.modeReferences_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicy::getModeReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ModeReference> > > AccessPolicy::getModeReferences() const
{
    return modeReferences_;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicy::setModeReferences()
//-----------------------------------------------------------------------------
void AccessPolicy::setModeReferences(QSharedPointer<QList<QSharedPointer<ModeReference> > > modeRefs)
{
    modeReferences_ = modeRefs;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicy::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access AccessPolicy::getAccess() const
{
    return access_;
}

//-----------------------------------------------------------------------------
// Function: AccessPolicy::setAccess()
//-----------------------------------------------------------------------------
void AccessPolicy::setAccess(AccessTypes::Access newAccess)
{
    access_ = newAccess;
}
