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
    for (auto const& modeRef : *other.modeReferences_)
    {
        if (modeRef)
        {
            auto copy = QSharedPointer<ModeReference>(new ModeReference(*modeRef));
            modeReferences_->append(copy);
        }
    }
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

