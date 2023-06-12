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
    Extendable(),
    initiatorRef_(masterInterfaceRef)
{

}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::TransparentBridge()
//-----------------------------------------------------------------------------
TransparentBridge::TransparentBridge(TransparentBridge const& other) :
    initiatorRef_(other.initiatorRef_),
    isPresent_(other.isPresent_)
{

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
void TransparentBridge::setMasterRef(QString const& interfaceName)
{
    setInitiatorRef(interfaceName);
}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::getMasterRef()
//-----------------------------------------------------------------------------
QString TransparentBridge::getMasterRef() const
{
    return getInitiatorRef();
}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::getMasterRef()
//-----------------------------------------------------------------------------
void TransparentBridge::setInitiatorRef(QString const& interfaceName)
{
    initiatorRef_ = interfaceName;
}

//-----------------------------------------------------------------------------
// Function: TransparentBridge::getMasterRef()
//-----------------------------------------------------------------------------
QString TransparentBridge::getInitiatorRef() const
{
    return initiatorRef_;
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
