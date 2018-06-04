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

#include <QString>
#include <QMap>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: RegisterFile::RegisterFile()
//-----------------------------------------------------------------------------
RegisterFile::RegisterFile(QString const& name, QString const& addressOffset, QString const& range):
RegisterBase(name),
dimension_(),
addressOffset_(addressOffset),
range_(range),
registerData_(new QList<QSharedPointer<RegisterBase> > ())
{

}

//-----------------------------------------------------------------------------
// Function: RegisterFile::RegisterFile()
//-----------------------------------------------------------------------------
RegisterFile::RegisterFile(const RegisterFile& other):
RegisterBase(other),
dimension_(other.dimension_),
addressOffset_(other.addressOffset_),
range_(other.range_),
registerData_(new QList<QSharedPointer<RegisterBase> > ())
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
        dimension_ = other.dimension_;
        addressOffset_ = other.addressOffset_;
        range_ = other.range_;
        
        registerData_->clear();
        copyRegisterData(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::clone()
//-----------------------------------------------------------------------------
QSharedPointer<RegisterBase> RegisterFile::clone()
{
    return QSharedPointer<RegisterBase>(new RegisterFile(*this));
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::()
//-----------------------------------------------------------------------------
QString RegisterFile::getDimension() const
{
    return dimension_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setDimension()
//-----------------------------------------------------------------------------
void RegisterFile::setDimension(QString const& newDimension)
{
    dimension_ = newDimension;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::getAddressOffset()
//-----------------------------------------------------------------------------
QString RegisterFile::getAddressOffset() const
{
    return addressOffset_;
}

//-----------------------------------------------------------------------------
// Function: RegisterFile::setAddressOffset()
//-----------------------------------------------------------------------------
void RegisterFile::setAddressOffset(QString const& newAddressOffset)
{
    addressOffset_ = newAddressOffset;
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
