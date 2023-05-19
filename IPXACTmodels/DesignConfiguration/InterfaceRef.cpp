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
    Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::getIsPresent()
//-----------------------------------------------------------------------------
std::string InterfaceRef::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::setIsPresent()
//-----------------------------------------------------------------------------
void InterfaceRef::setIsPresent(std::string const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::getComponentRef()
//-----------------------------------------------------------------------------
std::string InterfaceRef::getComponentRef() const
{
    return componentRef_;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::setComponentRef()
//-----------------------------------------------------------------------------
void InterfaceRef::setComponentRef(std::string const& newComponentRef)
{
    componentRef_ = newComponentRef;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::getBusRef()
//-----------------------------------------------------------------------------
std::string InterfaceRef::getBusRef() const
{
    return busRef_;
}

//-----------------------------------------------------------------------------
// Function: InterfaceRef::setBusRef()
//-----------------------------------------------------------------------------
void InterfaceRef::setBusRef(std::string const& newBusRef)
{
    busRef_ = newBusRef;
}
