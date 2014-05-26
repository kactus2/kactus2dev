//-----------------------------------------------------------------------------
// File: Kactus2Group.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.5.2014
//
// Description:
// Kactus2 composite of vendor extensions.
//-----------------------------------------------------------------------------

#include "Kactus2Group.h"

//-----------------------------------------------------------------------------
// Function: Kactus2Flag::Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Group::Kactus2Group(QString name):
    Kactus2Placeholder(name)
{
 
}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::Kactus2Group()
//-----------------------------------------------------------------------------
Kactus2Group::Kactus2Group(Kactus2Group const& other):
    Kactus2Placeholder(other)
{

}


//-----------------------------------------------------------------------------
// Function: Kactus2Flag::~Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Group::~Kactus2Group()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::clone()
//-----------------------------------------------------------------------------
Kactus2Group* Kactus2Group::clone() const
{
    return new Kactus2Group(*this);
}

//-----------------------------------------------------------------------------
// Function: Kactus2Group::write()
//-----------------------------------------------------------------------------
void Kactus2Group::write(QXmlStreamWriter& writer) const
{
    Kactus2Placeholder::write(writer);


}

