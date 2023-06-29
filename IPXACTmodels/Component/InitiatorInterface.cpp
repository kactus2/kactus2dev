//-----------------------------------------------------------------------------
// File: InitiatorInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.6.2023
//
// Description:
// 
//-----------------------------------------------------------------------------
#include "InitiatorInterface.h"

#include <QString>
#include <QMap>
#include <QObject>

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::InitiatorInterface()
//-----------------------------------------------------------------------------
InitiatorInterface::InitiatorInterface() :
    Extendable()
{
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::InitiatorInterface()
//-----------------------------------------------------------------------------
InitiatorInterface::InitiatorInterface(InitiatorInterface const& other):
    Extendable(other),
    addressSpaceRef_(other.addressSpaceRef_),
    isPresent_(other.isPresent_),
    baseAddress_(other.baseAddress_),
    modeRefs_(other.modeRefs_)
{

}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::operator=()
//-----------------------------------------------------------------------------
InitiatorInterface& InitiatorInterface::operator=(InitiatorInterface const& other)
{
    if (this != &other)
    {
        Extendable::operator=(other);

        addressSpaceRef_ = other.addressSpaceRef_;
        isPresent_ = other.isPresent_;
        baseAddress_ = other.baseAddress_;
        modeRefs_ = other.modeRefs_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::~InitiatorInterface()
//-----------------------------------------------------------------------------
InitiatorInterface::~InitiatorInterface()
{
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::setAddressSpaceRef()
//-----------------------------------------------------------------------------
void InitiatorInterface::setAddressSpaceRef(QString const& addressSpaceRef)
{
    addressSpaceRef_ = addressSpaceRef;
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::getAddressSpaceRef()
//-----------------------------------------------------------------------------
QString InitiatorInterface::getAddressSpaceRef() const
{
    return addressSpaceRef_;
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::setIsPresent()
//-----------------------------------------------------------------------------
void InitiatorInterface::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::getIsPresent()
//-----------------------------------------------------------------------------
QString InitiatorInterface::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::setBaseAddress()
//-----------------------------------------------------------------------------
void InitiatorInterface::setBaseAddress(QString const& baseAddress)
{
    baseAddress_ = baseAddress;
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::getBaseAddress()
//-----------------------------------------------------------------------------
QString InitiatorInterface::getBaseAddress() const
{
    return baseAddress_;
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::setModeRefs()
//-----------------------------------------------------------------------------
void InitiatorInterface::setModeRefs(QStringList const& modeRefs)
{
    modeRefs_ = modeRefs;
}

//-----------------------------------------------------------------------------
// Function: InitiatorInterface::getModeRefs()
//-----------------------------------------------------------------------------
QStringList InitiatorInterface::getModeRefs() const
{
    return modeRefs_;
}
