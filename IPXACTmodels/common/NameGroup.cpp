//-----------------------------------------------------------------------------
// File: NameGroup.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// Implementation for ipxact:nameGroup element.
//-----------------------------------------------------------------------------

#include "NameGroup.h"
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/XmlUtils.h>

//-----------------------------------------------------------------------------
// Function: NameGroup::NameGroup()
//-----------------------------------------------------------------------------
NameGroup::NameGroup(): 
    name_(),
    displayName_(),
    description_() 
{

}

//-----------------------------------------------------------------------------
// Function: NameGroup::NameGroup()
//-----------------------------------------------------------------------------
NameGroup::NameGroup(QString const& name, QString const& displayName, QString const& description):
    name_(name), 
    displayName_(displayName),
    description_(description)
{

}

//-----------------------------------------------------------------------------
// Function: NameGroup::NameGroup()
//-----------------------------------------------------------------------------
NameGroup::NameGroup( const NameGroup& other ):
    name_(other.name_),
    displayName_(other.displayName_),
    description_(other.description_)
{

}

//-----------------------------------------------------------------------------
// Function: NameGroup::~NameGroup()
//-----------------------------------------------------------------------------
NameGroup::~NameGroup()
{

}

//-----------------------------------------------------------------------------
// Function: NameGroup::operator=()
//-----------------------------------------------------------------------------
NameGroup& NameGroup::operator=( const NameGroup& other )
{
    if (this != &other) {
        name_ = other.name_;
        displayName_ = other.displayName_;
        description_ = other.description_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::name()
//-----------------------------------------------------------------------------
QString NameGroup::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setName()
//-----------------------------------------------------------------------------
void NameGroup::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::displayName()
//-----------------------------------------------------------------------------
QString NameGroup::displayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setDisplayName()
//-----------------------------------------------------------------------------
void NameGroup::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::description()
//-----------------------------------------------------------------------------
QString NameGroup::description() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setDescription()
//-----------------------------------------------------------------------------
void NameGroup::setDescription(QString const& description)
{
    description_ = description;
}
