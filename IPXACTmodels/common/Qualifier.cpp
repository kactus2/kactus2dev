//-----------------------------------------------------------------------------
// File: Qualifier.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------

#include "Qualifier.h"

//-----------------------------------------------------------------------------
// Function: Qualifier::Qualifier()
//-----------------------------------------------------------------------------
Qualifier::Qualifier() :
isAddress_(false),
    isData_(false),
    isClock_(false),
    isReset_(false)
{

}

//-----------------------------------------------------------------------------
// Function: Qualifier::Qualifier()
//-----------------------------------------------------------------------------
Qualifier::Qualifier(Qualifier const& other):
isAddress_(other.isAddress_),
    isData_(other.isData_),
    isClock_(other.isClock_),
    isReset_(other.isReset_)
{

}

//-----------------------------------------------------------------------------
// Function: Qualifier::isSet()
//-----------------------------------------------------------------------------
bool Qualifier::isSet()
{
    return isData_ || isAddress_ || isClock_ || isReset_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::isAddress()
//-----------------------------------------------------------------------------
bool Qualifier::isAddress() const
{
    return isAddress_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::isData()
//-----------------------------------------------------------------------------
bool Qualifier::isData() const
{
    return isData_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::isClock()
//-----------------------------------------------------------------------------
bool Qualifier::isClock() const
{
    return isClock_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::isReset()
//-----------------------------------------------------------------------------
bool Qualifier::isReset() const
{
    return isReset_;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::setType()
//-----------------------------------------------------------------------------
void Qualifier::setType(Type qualifierType)
{
    isAddress_ = (qualifierType == Address || qualifierType == Data_Address);
    isData_ = (qualifierType == Data || qualifierType == Data_Address);
    isClock_ = (qualifierType == Clock);
    isReset_ = (qualifierType == Reset);
}

//-----------------------------------------------------------------------------
// Function: Qualifier::operator=()
//-----------------------------------------------------------------------------
Qualifier& Qualifier::operator=(Qualifier const& other)
{
    if (this != &other)
    {
        isAddress_ = other.isAddress_;
        isData_ = other.isData_;
        isClock_ = other.isClock_;
        isReset_ = other.isReset_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: Qualifier::operator!=()
//-----------------------------------------------------------------------------
bool Qualifier::operator!=(Qualifier const& other)
{
    return isAddress_ != other.isAddress_ || isData_ != other.isData_ || isClock_ != other.isClock_ ||
        isReset_ != other.isReset_;
}
