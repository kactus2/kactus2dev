//-----------------------------------------------------------------------------
// File: MemoryBlockBase.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Describes the base class for ipxact:addressBlock, ipxact:bank and ipxact:subspaceMap elements.
//-----------------------------------------------------------------------------

#include "MemoryBlockBase.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/AccessHandle.h>
#include <IPXACTmodels/Component/AccessPolicy.h>
#include <utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::MemoryBlockBase()
//-----------------------------------------------------------------------------
MemoryBlockBase::MemoryBlockBase(QString const& name, QString const& baseAddress):
NameGroup(name),
Extendable(),
isPresent_(),
usage_(General::USAGE_COUNT),
volatile_(),
access_(AccessTypes::ACCESS_COUNT),
baseAddress_(baseAddress)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::MemoryBlockBase()
//-----------------------------------------------------------------------------
MemoryBlockBase::MemoryBlockBase(const MemoryBlockBase& other):
NameGroup(other),
Extendable(other),
isPresent_(other.isPresent_),
usage_(other.usage_),
volatile_(other.volatile_),
access_(other.access_),
baseAddress_(other.baseAddress_)
{
    Copy::copyList(other.accessHandles_, accessHandles_);
    Copy::copyList(other.accessPolicies_, accessPolicies_);
    Copy::copyList(other.parameters_, parameters_);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::operator=()
//-----------------------------------------------------------------------------
MemoryBlockBase& MemoryBlockBase::operator=(const MemoryBlockBase& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        isPresent_ = other.isPresent_;
        baseAddress_ = other.baseAddress_;
        usage_ = other.usage_;
        volatile_ = other.volatile_;
        access_ = other.access_;

        accessHandles_->clear();
        Copy::copyList(other.accessHandles_, accessHandles_);

        accessPolicies_->clear();
        Copy::copyList(other.accessPolicies_, accessPolicies_);
        
        parameters_->clear();
        Copy::copyList(other.parameters_, parameters_);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::~MemoryBlockBase()
//-----------------------------------------------------------------------------
MemoryBlockBase::~MemoryBlockBase()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::getIsPresent()
//-----------------------------------------------------------------------------
QString MemoryBlockBase::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::setIsPresent()
//-----------------------------------------------------------------------------
void MemoryBlockBase::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::getAccessHandles()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AccessHandle> > > MemoryBlockBase::getAccessHandles() const
{
    return accessHandles_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::setAccessHandles()
//-----------------------------------------------------------------------------
void MemoryBlockBase::setAccessHandles(QSharedPointer<QList<QSharedPointer<AccessHandle> > > accessHandles)
{
    accessHandles_ = accessHandles;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::getBaseAddress()
//-----------------------------------------------------------------------------
QString MemoryBlockBase::getBaseAddress() const
{
    return baseAddress_;
}


//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::setBaseAddress()
//-----------------------------------------------------------------------------
void MemoryBlockBase::setBaseAddress(QString const& baseAddress)
{
    baseAddress_ = baseAddress;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::getUsage()
//-----------------------------------------------------------------------------
General::Usage MemoryBlockBase::getUsage() const
{
    return usage_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::setUsage()
//-----------------------------------------------------------------------------
void MemoryBlockBase::setUsage(General::Usage newUsage)
{
    usage_ = newUsage;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::getVolatile()
//-----------------------------------------------------------------------------
QString MemoryBlockBase::getVolatile() const
{
    return volatile_.toString();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::setVolatile()
//-----------------------------------------------------------------------------
void MemoryBlockBase::setVolatile(bool newVolatileValue)
{
    volatile_.setValue(newVolatileValue);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::clearVolatile()
//-----------------------------------------------------------------------------
void MemoryBlockBase::clearVolatile()
{
    volatile_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access MemoryBlockBase::getAccess() const
{
    return access_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::setAccess()
//-----------------------------------------------------------------------------
void MemoryBlockBase::setAccess(AccessTypes::Access newAccess)
{
    access_ = newAccess;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::getAccessPolicies()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AccessPolicy> > > MemoryBlockBase::getAccessPolicies() const
{
    return accessPolicies_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::setAccessPolicies()
//-----------------------------------------------------------------------------
void MemoryBlockBase::setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > newAccessPolicies)
{
    accessPolicies_ = newAccessPolicies;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::getParametesrs()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > MemoryBlockBase::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::setParameters()
//-----------------------------------------------------------------------------
void MemoryBlockBase::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_ = newParameters;
}
