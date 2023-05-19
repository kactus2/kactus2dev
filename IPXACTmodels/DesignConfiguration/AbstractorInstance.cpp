//-----------------------------------------------------------------------------
// File: AbstractorInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.08.2015
//
// Description:
// Matches the ipxact:abstractorInstance element.
//-----------------------------------------------------------------------------

#include "AbstractorInstance.h"

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::AbstractorInstance()
//-----------------------------------------------------------------------------
AbstractorInstance::AbstractorInstance():
    Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::AbstractorInstance()
//-----------------------------------------------------------------------------
AbstractorInstance::AbstractorInstance(const AbstractorInstance& other):
instanceName_(other.instanceName_),
displayName_(other.displayName_),
description_(other.description_),
abstractorRef_(new ConfigurableVLNVReference(*other.abstractorRef_.data())),
viewName_(other.viewName_)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getAbstractorRef()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> AbstractorInstance::getAbstractorRef() const
{
    return abstractorRef_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getShortDescription()
//-----------------------------------------------------------------------------
std::string AbstractorInstance::getShortDescription() const
{
    return shortDescription_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getDescription()
//-----------------------------------------------------------------------------
std::string AbstractorInstance::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getDisplayName()
//-----------------------------------------------------------------------------
std::string AbstractorInstance::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getInstanceName()
//-----------------------------------------------------------------------------
std::string AbstractorInstance::getInstanceName() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getViewName()
//-----------------------------------------------------------------------------
std::string AbstractorInstance::getViewName() const
{
    return viewName_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setAbstractorRef()
//-----------------------------------------------------------------------------
void AbstractorInstance::setAbstractorRef(QSharedPointer<ConfigurableVLNVReference> abstractorRef)
{
    abstractorRef_ = abstractorRef;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setShortDescription()
//-----------------------------------------------------------------------------
void AbstractorInstance::setShortDescription(const std::string& shortDescription)
{
    shortDescription_ = shortDescription;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setDescription()
//-----------------------------------------------------------------------------
void AbstractorInstance::setDescription(const std::string& description)
{
    description_ = description;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setDisplayName()
//-----------------------------------------------------------------------------
void AbstractorInstance::setDisplayName(const std::string& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setInstanceName()
//-----------------------------------------------------------------------------
void AbstractorInstance::setInstanceName(const std::string& instanceName)
{
    instanceName_ = instanceName;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setViewName()
//-----------------------------------------------------------------------------
void AbstractorInstance::setViewName(const std::string& viewName)
{
    viewName_ = viewName;
}