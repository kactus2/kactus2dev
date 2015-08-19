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
// Function: Qualifier::~Qualifier()
//-----------------------------------------------------------------------------
Qualifier::~Qualifier()
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
    if (qualifierType == Data)
    {
        isAddress_ = false;
        isData_ = true;
        isClock_ = false;
        isReset_ = false;
    }
    else if (qualifierType == Address)
    {
        isAddress_ = true;
        isData_ = false;
        isClock_ = false;
        isReset_ = false;
    }
    else if (qualifierType == Data_Address)
    {
        isAddress_ = true;
        isData_ = true;
        isClock_ = false;
        isReset_ = false;
    }
    else if (qualifierType == Clock)
    {
        isData_ = false;
        isAddress_ = false;
        isClock_ = true;
        isReset_ = false;
    }
    else if (qualifierType == Reset)
    {
        isData_ = false;
        isAddress_ = false;
        isClock_ = false;
        isReset_ = true;
    }
    else
    {
        isData_ = false;
        isAddress_ = false;
        isClock_ = false;
        isReset_ = false;
    }
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
