//-----------------------------------------------------------------------------
// File: Associable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.6.2014
//
// Description:
// Interface for items that can be associated with others.
//-----------------------------------------------------------------------------

#include "Associable.h"
#include "Association.h"

//-----------------------------------------------------------------------------
// Function: Associable::Associable()
//-----------------------------------------------------------------------------
Associable::Associable():
    associations_()
{

}

//-----------------------------------------------------------------------------
// Function: Associable::~Associable()
//-----------------------------------------------------------------------------
Associable::~Associable()
{

}

//-----------------------------------------------------------------------------
// Function: Associable::addAssociation()
//-----------------------------------------------------------------------------
void Associable::addAssociation(Association* association)
{
    if (!associations_.contains(association))
    {
        associations_.append(association);
    }
}

//-----------------------------------------------------------------------------
// Function: Associable::removeAssociation()
//-----------------------------------------------------------------------------
void Associable::removeAssociation(Association* association)
{
    associations_.removeAll(association);
}
