//-----------------------------------------------------------------------------
// File: MasterInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.8.2010
//
// Description:
// Implementation of ipxact:master in bus interface.
//-----------------------------------------------------------------------------

#include "MasterInterface.h"

#include <QString>
#include <QMap>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: MasterInterface::MasterInterface()
//-----------------------------------------------------------------------------
MasterInterface::MasterInterface():
addressSpaceRef_(),
isPresent_(),
baseAddress_(),
baseAttributes_()
{
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::MasterInterface()
//-----------------------------------------------------------------------------
MasterInterface::MasterInterface(MasterInterface const& other):
addressSpaceRef_(other.addressSpaceRef_),
isPresent_(other.isPresent_),
baseAddress_(other.baseAddress_),
baseAttributes_(other.baseAttributes_)
{
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::operator=()
//-----------------------------------------------------------------------------
MasterInterface& MasterInterface::operator=(MasterInterface const& other)
{
    if (this != &other)
    {
        addressSpaceRef_ = other.addressSpaceRef_;
        isPresent_ = other.isPresent_;
        baseAddress_ = other.baseAddress_;
        baseAttributes_ = other.baseAttributes_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::~MasterInterface()
//-----------------------------------------------------------------------------
MasterInterface::~MasterInterface()
{
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::setAddressSpaceRef()
//-----------------------------------------------------------------------------
void MasterInterface::setAddressSpaceRef(QString const& addressSpaceRef)
{
    addressSpaceRef_ = addressSpaceRef;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::getAddressSpaceRef()
//-----------------------------------------------------------------------------
QString MasterInterface::getAddressSpaceRef() const
{
    return addressSpaceRef_;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::setIsPresent()
//-----------------------------------------------------------------------------
void MasterInterface::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::getIsPresent()
//-----------------------------------------------------------------------------
QString MasterInterface::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::setBaseAddress()
//-----------------------------------------------------------------------------
void MasterInterface::setBaseAddress(QString const& baseAddress)
{
    baseAddress_ = baseAddress;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::getBaseAddress()
//-----------------------------------------------------------------------------
QString MasterInterface::getBaseAddress() const
{
    return baseAddress_;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::setBaseAttributes()
//-----------------------------------------------------------------------------
void MasterInterface::setBaseAttributes(QMap<QString, QString> const& baseAttributes)
{
    baseAttributes_ = baseAttributes;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::getBaseAttributes()
//-----------------------------------------------------------------------------
QMap<QString,QString> MasterInterface::getBaseAttributes() const
{
    return baseAttributes_;
}

//-----------------------------------------------------------------------------
// Function: MasterInterface::getPrompt()
//-----------------------------------------------------------------------------
QString MasterInterface::getPrompt() const
{
    return baseAttributes_.value("ipxact:prompt");
}


