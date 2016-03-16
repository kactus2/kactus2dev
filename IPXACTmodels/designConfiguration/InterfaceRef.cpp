//-----------------------------------------------------------------------------
// File: InterfaceRef.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Matches the ipxact:interfaceRef element.
//-----------------------------------------------------------------------------

#include "InterfaceRef.h"

//-----------------------------------------------------------------------------
// Function: InterfaceRef::InterfaceRef()
//-----------------------------------------------------------------------------
InterfaceRef::InterfaceRef():
componentRef_(),
busRef_(),
isPresent_()
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::InterfaceRef()
//-----------------------------------------------------------------------------
InterfaceRef::InterfaceRef(const InterfaceRef& other):
componentRef_(other.componentRef_),
busRef_(other.busRef_),
isPresent_(other.isPresent_)
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::~InterfaceRef()
//-----------------------------------------------------------------------------
InterfaceRef::~InterfaceRef()
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::getIsPresent()
//-----------------------------------------------------------------------------
QString InterfaceRef::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::setIsPresent()
//-----------------------------------------------------------------------------
void InterfaceRef::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::getComponentRef()
//-----------------------------------------------------------------------------
QString InterfaceRef::getComponentRef() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::setComponentRef()
//-----------------------------------------------------------------------------
void InterfaceRef::setComponentRef(QString const& newComponentRef)
{
    componentRef_ = newComponentRef;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::getBusRef()
//-----------------------------------------------------------------------------
QString InterfaceRef::getBusRef() const
{
    return busRef_;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::setBusRef()
//-----------------------------------------------------------------------------
void InterfaceRef::setBusRef(QString const& newBusRef)
{
    busRef_ = newBusRef;
}
