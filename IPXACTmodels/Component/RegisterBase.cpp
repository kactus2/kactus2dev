//-----------------------------------------------------------------------------
// File: RegisterBase.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the base class for ipxact:register, RegisterBase and alternateRegister.
//-----------------------------------------------------------------------------

#include "RegisterBase.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/Component/AccessPolicy.h>
#include <IPXACTmodels/Component/MemoryArray.h>

//-----------------------------------------------------------------------------
// Function: RegisterBase::RegisterBase()
//-----------------------------------------------------------------------------
RegisterBase::RegisterBase(QString const& name /* = QString() */) :
NameGroup(name),
Extendable()
{

}
RegisterBase::RegisterBase(QString const& name,QString const& addressOffset) :
NameGroup(name),
Extendable(),
addressOffset_(addressOffset)
{

}
//-----------------------------------------------------------------------------
// Function: RegisterBase::RegisterBase()
//-----------------------------------------------------------------------------
RegisterBase::RegisterBase (const RegisterBase& other):
NameGroup(other),
Extendable(other),
addressOffset_(other.addressOffset_),
isPresent_(other.isPresent_),
typeIdentifier_(other.typeIdentifier_)
{
    copyParameters(other);
    copyAccessPolicies(other);

    if (other.memoryArray_)
    {
        memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray(*other.memoryArray_));
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::operator=()
//-----------------------------------------------------------------------------
RegisterBase& RegisterBase::operator=(const RegisterBase& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        addressOffset_ = other.addressOffset_;
        isPresent_ = other.isPresent_;
        typeIdentifier_ = other.typeIdentifier_;

        parameters_->clear();
        copyParameters(other);
        accessPolicies_->clear();
        copyAccessPolicies(other);

        if (other.memoryArray_)
        {
            memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray(*other.memoryArray_));
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::~RegisterBase()
//-----------------------------------------------------------------------------
RegisterBase::~RegisterBase()
{
    parameters_.clear();
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::getIsPresent()
//-----------------------------------------------------------------------------
QString RegisterBase::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::setIsPresent()
//-----------------------------------------------------------------------------
void RegisterBase::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::getTypeIdentifier()
//-----------------------------------------------------------------------------
QString RegisterBase::getTypeIdentifier() const
{
    return typeIdentifier_;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::setTypeIdentifier()
//-----------------------------------------------------------------------------
void RegisterBase::setTypeIdentifier(QString const& newTypeIdentifier)
{
    typeIdentifier_ = newTypeIdentifier;
}
//-----------------------------------------------------------------------------
// Function: RegisterBase::()
//-----------------------------------------------------------------------------
QString RegisterBase::getDimension() const
{   
    if (memoryArray_)
    {
        return memoryArray_->getDimensions()->first()->value_;
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::setDimension()
//-----------------------------------------------------------------------------
void RegisterBase::setDimension(QString const& newDimension)
{
    if (!memoryArray_)
    {
        memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray());
    }
    
    if (newDimension.isEmpty())
    {
        memoryArray_.clear();
        return;
    }

    // Create dimension if missing, otherwise set the new value to the first dim.
    if (memoryArray_->getDimensions()->isEmpty())
    {
        QSharedPointer<MemoryArray::Dimension> newDim(new MemoryArray::Dimension());
        newDim->value_ = newDimension;
        memoryArray_->getDimensions()->append(newDim);
    }
    else
    {
        memoryArray_->getDimensions()->first()->value_ = newDimension;
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::getMemoryArray()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryArray> RegisterBase::getMemoryArray() const
{
    return memoryArray_;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::setMemoryArray()
//-----------------------------------------------------------------------------
void RegisterBase::setMemoryArray(QSharedPointer<MemoryArray> newMemArray)
{
    memoryArray_ = newMemArray;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::getAddressOffset()
//-----------------------------------------------------------------------------
QString RegisterBase::getAddressOffset() const
{
    return addressOffset_;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::setAddressOffset()
//-----------------------------------------------------------------------------
void RegisterBase::setAddressOffset(QString const& newAddressOffset)
{
    addressOffset_ = newAddressOffset;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > RegisterBase::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::setParameters()
//-----------------------------------------------------------------------------
void RegisterBase::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_->clear();
    parameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::getAccessPolicies()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AccessPolicy> > > RegisterBase::getAccessPolicies() const
{
    return accessPolicies_;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::setAccessPolicies()
//-----------------------------------------------------------------------------
void RegisterBase::setAccessPolicies(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > newAccessPolicies)
{
    accessPolicies_ = newAccessPolicies;
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::copyParameters()
//-----------------------------------------------------------------------------
void RegisterBase::copyParameters(const RegisterBase& other)
{
    for (auto const& parameter : *other.parameters_)
    {
        if (parameter)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
            parameters_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterBase::copyAccessPolicies()
//-----------------------------------------------------------------------------
void RegisterBase::copyAccessPolicies(const RegisterBase& other)
{
    for (auto const& accessPolicy : *other.accessPolicies_)
    {
        if (accessPolicy)
        {
            auto copy = QSharedPointer<AccessPolicy>(new AccessPolicy(*accessPolicy));
            accessPolicies_->append(copy);
        }
    }
}
