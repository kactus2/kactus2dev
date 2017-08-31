//-----------------------------------------------------------------------------
// File: TransparentBridge.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2017
//
// Description:
// Implementation of ipxact:transparentBridge element.
//-----------------------------------------------------------------------------

#include "TransparentBridge.h"

//-----------------------------------------------------------------------------
// Function: TransparentBridge::TransparentBridge()
//-----------------------------------------------------------------------------
TransparentBridge::TransparentBridge(QString const& masterInterfaceRef):
masterRef_(masterInterfaceRef), 
    isPresent_()
{

}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::TransparentBridge()
//-----------------------------------------------------------------------------
TransparentBridge::TransparentBridge(TransparentBridge const& other)
{
    masterRef_ = other.masterRef_;
    isPresent_ = other.isPresent_;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::~TransparentBridge()
//-----------------------------------------------------------------------------
TransparentBridge::~TransparentBridge()
{

}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::setMasterRef()
//-----------------------------------------------------------------------------
void TransparentBridge::setMasterRef(QString const& masterInterfaceName)
{
    masterRef_ = masterInterfaceName;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::getMasterRef()
//-----------------------------------------------------------------------------
QString TransparentBridge::getMasterRef() const
{
    return masterRef_;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::setIsPresent()
//-----------------------------------------------------------------------------
void TransparentBridge::setIsPresent(QString const& presence)
{
    isPresent_ = presence;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::getIsPresent()
//-----------------------------------------------------------------------------
QString TransparentBridge::getIsPresent() const
{
    return isPresent_;
}
