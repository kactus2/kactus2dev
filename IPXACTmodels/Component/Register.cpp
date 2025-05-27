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

#include <IPXACTmodels/Component/MemoryArray.h>

#include <QtGlobal>

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register(QString const& name, QString const& addressOffset, QString const& size):
RegisterDefinition(name, addressOffset),
size_(size),
alternateRegisters_(new QList<QSharedPointer<AlternateRegister> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Register::Register()
//-----------------------------------------------------------------------------
Register::Register(const Register& other):
    RegisterDefinition(other),
    size_(other.size_),
    alternateRegisters_(new QList<QSharedPointer<AlternateRegister> > ()),
    registerDefinitionReference_(other.registerDefinitionReference_),
    typeDefinitionsReference_(other.typeDefinitionsReference_)
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
        size_ = other.size_;
        registerDefinitionReference_ = other.registerDefinitionReference_;
        typeDefinitionsReference_ = other.typeDefinitionsReference_;

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
// Function: Register::getRegisterDefinitionReference()
//-----------------------------------------------------------------------------
QString Register::getRegisterDefinitionReference() const
{
    return registerDefinitionReference_;
}

//-----------------------------------------------------------------------------
// Function: Register::setRegisterDefinitionReference()
//-----------------------------------------------------------------------------
void Register::setRegisterDefinitionReference(QString const& registerDefinitionRef)
{
    registerDefinitionReference_ = registerDefinitionRef;
}

//-----------------------------------------------------------------------------
// Function: Register::getTypeDefinitionsReference()
//-----------------------------------------------------------------------------
QString Register::getTypeDefinitionsReference() const
{
    return typeDefinitionsReference_;
}

//-----------------------------------------------------------------------------
// Function: Register::setTypeDefinitionsReference()
//-----------------------------------------------------------------------------
void Register::setTypeDefinitionsReference(QString const& typeDefinitionsRef)
{
    typeDefinitionsReference_ = typeDefinitionsRef;
}

//-----------------------------------------------------------------------------
// Function: Register::calculateDefaultStride()
//-----------------------------------------------------------------------------
quint64 Register::calculateDefaultStride(quint64 addressUnitBits, quint64 regSize)
{
    Q_ASSERT_X(addressUnitBits == 0, "Register::calculateDefaultStride()", "Address unit bits is zero");

    // Default stride is smallest k so that k * AUB >= reg size
    if (addressUnitBits == 0)
    {
        return 0;
    }
    else if (regSize % addressUnitBits == 0)
    {
        return regSize / addressUnitBits;
    }
    else
    {
        return regSize / addressUnitBits + 1;
    }
}

//-----------------------------------------------------------------------------
// Function: Register::copyAlternateRegisters()
//-----------------------------------------------------------------------------
void Register::copyAlternateRegisters(const Register& other)
{
    for (auto const& alternateRegister : *other.alternateRegisters_)
    {
        if (alternateRegister)
        {
            auto copy = QSharedPointer<AlternateRegister>(new AlternateRegister(*alternateRegister));
            alternateRegisters_->append(copy);
        }
    }
}
