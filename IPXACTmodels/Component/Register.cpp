//-----------------------------------------------------------------------------
// File: Register.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:register element.
//-----------------------------------------------------------------------------

#include "Register.h"
#include "AlternateRegister.h"

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register(QString const& name, QString const& addressOffset, QString const& size):
RegisterDefinition(name),
dimension_(),
addressOffset_(addressOffset),
size_(size),
alternateRegisters_(new QList<QSharedPointer<AlternateRegister> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register(const Register& other):
RegisterDefinition(other),
dimension_(other.dimension_),
addressOffset_(other.addressOffset_),
size_(other.size_),
alternateRegisters_(new QList<QSharedPointer<AlternateRegister> > ())
{
    copyAlternateRegisters(other);
}

//-----------------------------------------------------------------------------
// Function: Register::operator=()
//-----------------------------------------------------------------------------
Register& Register::operator=(const Register& other)
{
    if (this != &other)
    {
        RegisterDefinition::operator=(other);
        dimension_ = other.dimension_;
        addressOffset_ = other.addressOffset_;
        size_ = other.size_;
        
        alternateRegisters_->clear();
        copyAlternateRegisters(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: Register::~Register()
//-----------------------------------------------------------------------------
Register::~Register()
{
    alternateRegisters_.clear();
}

//-----------------------------------------------------------------------------
// Function: Register::clone()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterBase> Register::clone()
{
    return QSharedPointer<RegisterBase>(new Register(*this));
}

//-----------------------------------------------------------------------------
// Function: Register::getDim()
//-----------------------------------------------------------------------------
QString Register::getDimension() const
{
    return dimension_;
}

//-----------------------------------------------------------------------------
// Function: Register::setDim()
//-----------------------------------------------------------------------------
void Register::setDimension(QString const& newDimension)
{
    dimension_ = newDimension;
}

//-----------------------------------------------------------------------------
// Function: Register::getAddressOffset()
//-----------------------------------------------------------------------------
QString Register::getAddressOffset() const
{
    return addressOffset_;
}

//-----------------------------------------------------------------------------
// Function: Register::setAddressOffset()
//-----------------------------------------------------------------------------
void Register::setAddressOffset(QString const& addressOffset)
{
    addressOffset_ = addressOffset;
}

//-----------------------------------------------------------------------------
// Function: Register::getAlternateRegisters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<AlternateRegister> > > Register::getAlternateRegisters() const
{
    return alternateRegisters_;
}

//-----------------------------------------------------------------------------
// Function: Register::setAlternateRegisters()
//-----------------------------------------------------------------------------
void Register::setAlternateRegisters
    (QSharedPointer<QList<QSharedPointer<AlternateRegister> > > newAlternateRegisters)
{
    alternateRegisters_->clear();
    alternateRegisters_ = newAlternateRegisters;
}

//-----------------------------------------------------------------------------
// Function: Register::getSize()
//-----------------------------------------------------------------------------
QString Register::getSize() const
{
    return size_;
}

//-----------------------------------------------------------------------------
// Function: Register::setSize()
//-----------------------------------------------------------------------------
void Register::setSize(QString const& newSize)
{
    size_ = newSize;
}

//-----------------------------------------------------------------------------
// Function: Register::copyAlternateRegisters()
//-----------------------------------------------------------------------------
void Register::copyAlternateRegisters(const Register& other)
{
    foreach (QSharedPointer<AlternateRegister> alternateRegister, *other.alternateRegisters_)
    {
        if (alternateRegister)
        {
            QSharedPointer<AlternateRegister> copy =
                QSharedPointer<AlternateRegister>(new AlternateRegister(*alternateRegister.data()));
            alternateRegisters_->append(copy);
        }
    }
}