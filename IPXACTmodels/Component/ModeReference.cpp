//-----------------------------------------------------------------------------
// File: ModeReference.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.7.2023
//
// Description:
// Describes the ipxact:modeRef element.
//-----------------------------------------------------------------------------

#include "ModeReference.h"

//-----------------------------------------------------------------------------
// Function: ModeReference::ModeReference()
//-----------------------------------------------------------------------------
ModeReference::ModeReference()
{

}

//-----------------------------------------------------------------------------
// Function: ModeReference::ModeReference()
//-----------------------------------------------------------------------------
ModeReference::ModeReference(ModeReference const& other):
    reference_(other.reference_),
    priority_(other.priority_)
{

}

//-----------------------------------------------------------------------------
// Function: ModeReference::Reference()
//-----------------------------------------------------------------------------
QString ModeReference::getReference() const
{
    return reference_;
}

//-----------------------------------------------------------------------------
// Function: ModeReference::Reference()
//-----------------------------------------------------------------------------
void ModeReference::setReference(QString const& val)
{
    reference_ = val;
}

//-----------------------------------------------------------------------------
// Function: ModeReference::getPriority()
//-----------------------------------------------------------------------------
QString ModeReference::getPriority() const
{
    return priority_;
}

//-----------------------------------------------------------------------------
// Function: ModeReference::setPriority()
//-----------------------------------------------------------------------------
void ModeReference::setPriority(QString const& val)
{
    priority_ = val;
}
