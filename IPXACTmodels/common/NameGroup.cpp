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
    name_(name.toStdString()), 
    displayName_(displayName.toStdString()),
    description_(description.toStdString())
{

}

//-----------------------------------------------------------------------------
// Function: NameGroup::NameGroup()
//-----------------------------------------------------------------------------
NameGroup::NameGroup(std::string_view name, std::string_view displayName, std::string_view description) :
    name_(name),
    displayName_(displayName),
    description_(description)
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
    return QString::fromStdString(name_);
}

std::string NameGroup::nameStd() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setName()
//-----------------------------------------------------------------------------
void NameGroup::setName(QString const& name)
{
    name_ = name.toStdString();
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setName()
//-----------------------------------------------------------------------------
void NameGroup::setName(std::string_view name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::displayName()
//-----------------------------------------------------------------------------
QString NameGroup::displayName() const
{
    return QString::fromStdString(displayName_);
}

std::string NameGroup::displayNameStd() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setDisplayName()
//-----------------------------------------------------------------------------
void NameGroup::setDisplayName(QString const& displayName)
{
    displayName_ = displayName.toStdString();
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setDisplayName()
//-----------------------------------------------------------------------------
void NameGroup::setDisplayName(std::string_view displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::shortDescription()
//-----------------------------------------------------------------------------
QString NameGroup::shortDescription() const
{
    return QString::fromStdString(shortDescription_);
}

std::string NameGroup::shortDescriptionStd() const
{
    return shortDescription_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setShortDescription()
//-----------------------------------------------------------------------------
void NameGroup::setShortDescription(std::string_view description)
{
    shortDescription_ = description;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setShortDescription()
//-----------------------------------------------------------------------------
void NameGroup::setShortDescription(QString const& description)
{
    shortDescription_ = description.toStdString();
}

//-----------------------------------------------------------------------------
// Function: NameGroup::description()
//-----------------------------------------------------------------------------
QString NameGroup::description() const
{
    return QString::fromStdString(description_);
}

std::string NameGroup::descriptionStd() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: NameGroup::setDescription()
//-----------------------------------------------------------------------------
void NameGroup::setDescription(QString const& description)
{
    description_ = description.toStdString();
}

void NameGroup::setDescription(std::string_view description)
{
    description_ = description;
}
