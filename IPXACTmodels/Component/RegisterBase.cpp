//-----------------------------------------------------------------------------
// File: RegisterBase.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the base class for ipxact:register, registerFile and alternateRegister.
//-----------------------------------------------------------------------------

#include "RegisterBase.h"

#include <IPXACTmodels/common/Parameter.h>

//-----------------------------------------------------------------------------
// Function: RegisterBase::RegisterBase()
//-----------------------------------------------------------------------------
RegisterBase::RegisterBase(QString const& name /* = QString() */) :
NameGroup(name),
Extendable(),
isPresent_(),
typeIdentifier_(),
parameters_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: RegisterBase::RegisterBase()
//-----------------------------------------------------------------------------
RegisterBase::RegisterBase (const RegisterBase& other):
NameGroup(other),
Extendable(other),
isPresent_(other.isPresent_),
typeIdentifier_(other.typeIdentifier_),
parameters_(new QList<QSharedPointer<Parameter> > ())
{
    copyParameters(other);
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
        isPresent_ = other.isPresent_;
        typeIdentifier_ = other.typeIdentifier_;

        parameters_->clear();
        copyParameters(other);
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
// Function: RegisterBase::copyParameters()
//-----------------------------------------------------------------------------
void RegisterBase::copyParameters(const RegisterBase& other)
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