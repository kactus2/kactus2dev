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
abstractorRef_(new ConfigurableVLNVReference(*other.abstractorRef_)),
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
QString AbstractorInstance::getShortDescription() const
{
    return shortDescription_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getDescription()
//-----------------------------------------------------------------------------
QString AbstractorInstance::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getDisplayName()
//-----------------------------------------------------------------------------
QString AbstractorInstance::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getInstanceName()
//-----------------------------------------------------------------------------
QString AbstractorInstance::getInstanceName() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::getViewName()
//-----------------------------------------------------------------------------
QString AbstractorInstance::getViewName() const
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
void AbstractorInstance::setShortDescription(const QString& shortDescription)
{
    shortDescription_ = shortDescription;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setDescription()
//-----------------------------------------------------------------------------
void AbstractorInstance::setDescription(const QString& description)
{
    description_ = description;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setDisplayName()
//-----------------------------------------------------------------------------
void AbstractorInstance::setDisplayName(const QString& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setInstanceName()
//-----------------------------------------------------------------------------
void AbstractorInstance::setInstanceName(const QString& instanceName)
{
    instanceName_ = instanceName;
}

//-----------------------------------------------------------------------------
// Function: AbstractorInstance::setViewName()
//-----------------------------------------------------------------------------
void AbstractorInstance::setViewName(const QString& viewName)
{
    viewName_ = viewName;
}