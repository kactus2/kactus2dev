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

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::MemoryBlockBase()
//-----------------------------------------------------------------------------
MemoryBlockBase::MemoryBlockBase(QString const& name, QString const& baseAddress):
NameGroup(name),
Extendable(),
isPresent_(),
baseAddress_(baseAddress),
parameters_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::MemoryBlockBase()
//-----------------------------------------------------------------------------
MemoryBlockBase::MemoryBlockBase(const MemoryBlockBase& other):
NameGroup(other),
Extendable(other),
isPresent_(other.isPresent_),
baseAddress_(other.baseAddress_),
parameters_(new QList<QSharedPointer<Parameter> > ())
{
    copyParameters(other);
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
        
        parameters_->clear();
        copyParameters(other);
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

//-----------------------------------------------------------------------------
// Function: MemoryBlockBase::copyParameters()
//-----------------------------------------------------------------------------
void MemoryBlockBase::copyParameters(const MemoryBlockBase& other)
{
    foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
    {
        if (parameter)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
            parameters_->append(copy);
        }
    }
}