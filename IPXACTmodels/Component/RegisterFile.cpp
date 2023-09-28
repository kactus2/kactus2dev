//-----------------------------------------------------------------------------
// File: RegisterFile.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:registerFile element.
//-----------------------------------------------------------------------------

#include "Register.h"
#include "RegisterFile.h"

#include <IPXACTmodels/Component/MemoryArray.h>

#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: RegisterFile::RegisterFile()
//-----------------------------------------------------------------------------
RegisterFile::RegisterFile(QString const& name, QString const& addressOffset, QString const& range):
RegisterBase(name, addressOffset),
range_(range),
registerData_(new QList<QSharedPointer<RegisterBase> > ())
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFile::RegisterFile()
//-----------------------------------------------------------------------------
RegisterFile::RegisterFile(const RegisterFile& other):
RegisterBase(other),
range_(other.range_),
registerData_(new QList<QSharedPointer<RegisterBase> > ()),
registerFileDefinitionReference_(other.registerFileDefinitionReference_),
typeDefinitionsReference_(other.typeDefinitionsReference_)
{
    copyRegisterData(other);
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::operator=()
//-----------------------------------------------------------------------------
RegisterFile& RegisterFile::operator=(const RegisterFile& other)
{
    if (this != &other)
    {
        RegisterBase::operator=(other);
        range_ = other.range_;
        registerData_->clear();
        copyRegisterData(other);

        registerFileDefinitionReference_ = other.registerFileDefinitionReference_;
        typeDefinitionsReference_ = other.typeDefinitionsReference_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::~RegisterFile()
//-----------------------------------------------------------------------------
RegisterFile::~RegisterFile()
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFile::clone()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterBase> RegisterFile::clone()
{
    return QSharedPointer<RegisterBase>(new RegisterFile(*this));
}


//-----------------------------------------------------------------------------
// Function: RegisterFile::getRange()
//-----------------------------------------------------------------------------
QString RegisterFile::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setRange()
//-----------------------------------------------------------------------------
void RegisterFile::setRange(QString const& newRange)
{
    range_ = newRange;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::getRegisterData()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<RegisterBase> > > RegisterFile::getRegisterData() const
{
    return registerData_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setRegisterData()
//-----------------------------------------------------------------------------
void RegisterFile::setRegisterData(QSharedPointer<QList<QSharedPointer<RegisterBase> > > newRegisterData)
{
    registerData_ = newRegisterData;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::getRegisterFileDefinitionReference()
//-----------------------------------------------------------------------------
QString RegisterFile::getRegisterFileDefinitionReference() const
{
    return registerFileDefinitionReference_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setRegisterFileDefinitionReference()
//-----------------------------------------------------------------------------
void RegisterFile::setRegisterFileDefinitionReference(QString const& newRegisterFileDefinitionRef)
{
    registerFileDefinitionReference_ = newRegisterFileDefinitionRef;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::getTypeDefinitionsReference()
//-----------------------------------------------------------------------------
QString RegisterFile::getTypeDefinitionsReference() const
{
    return typeDefinitionsReference_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setTypeDefinitionsReference()
//-----------------------------------------------------------------------------
void RegisterFile::setTypeDefinitionsReference(QString const& newTypeDefinitionsRef)
{
    typeDefinitionsReference_ = newTypeDefinitionsRef;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::getAllRegisterOffsets()
//-----------------------------------------------------------------------------
QStringList RegisterFile::getAllRegisterOffsets() const
{
    QStringList registerOffsets;

    foreach (QSharedPointer<RegisterBase> registerBase, *registerData_)
    {
      registerOffsets.append(registerBase->getAddressOffset());
    }

    return registerOffsets;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::copyRegisterData()
//-----------------------------------------------------------------------------
void RegisterFile::copyRegisterData(const RegisterFile& other)
{
    foreach (QSharedPointer<RegisterBase> registerBase, *other.registerData_)
    {
        if (registerBase)
        {
            QSharedPointer<RegisterBase> copy = registerBase->clone();

            registerData_->append(copy);
        }
    }
}
