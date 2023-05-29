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
NameGroup::NameGroup(std::string const& name, std::string const& displayName, std::string const& description) :
    name_(QString::fromStdString(name)),
    displayName_(QString::fromStdString(displayName)),
    description_(QString::fromStdString(description))
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
        shortDescription_ = other.shortDescription_;
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
// Function: NameGroup::setName()
//-----------------------------------------------------------------------------
void NameGroup::setName(std::string const& name)
{
    name_ = QString::fromStdString(name);
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
// Function: NameGroup::sortDescription()
//-----------------------------------------------------------------------------
QString NameGroup::shortDescription() const
{
    return shortDescription_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::sortDescription()
//-----------------------------------------------------------------------------
void NameGroup::setShortDescription(QString const& description)
{
    shortDescription_ = description;
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
