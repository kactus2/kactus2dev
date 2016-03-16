//-----------------------------------------------------------------------------
// File: AddressBlock.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Describes the ipxact:addressBlock element.
//-----------------------------------------------------------------------------

#include "AddressBlock.h"
#include "RegisterBase.h"
#include "Register.h"

//-----------------------------------------------------------------------------
// Function: AddressBlock::AddressBlock()
//-----------------------------------------------------------------------------
AddressBlock::AddressBlock(QString const& name /* = QString() */, QString const& baseAddress /* = QString() */):
MemoryBlockBase(name, baseAddress),
typeIdentifier_(),
range_(),
rangeAttributes_(),
width_(),
widthAttributes_(),
usage_(General::USAGE_COUNT),
volatile_(),
access_(AccessTypes::ACCESS_COUNT),
registerData_(new QList<QSharedPointer<RegisterBase> > ())
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlock::AddressBlock()
//-----------------------------------------------------------------------------
AddressBlock::AddressBlock(const AddressBlock &other):
MemoryBlockBase(other),
typeIdentifier_(other.typeIdentifier_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_),
width_(other.width_),
widthAttributes_(other.widthAttributes_),
usage_(other.usage_),
volatile_(other.volatile_),
access_(other.access_),
registerData_(new QList<QSharedPointer<RegisterBase> > ())
{
    copyRegisterData(other);
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::operator=()
//-----------------------------------------------------------------------------
AddressBlock& AddressBlock::operator=( const AddressBlock& other )
{
    if (this != &other)
    {
        MemoryBlockBase::operator=(other);
        typeIdentifier_ = other.typeIdentifier_;
        range_ = other.range_;
        rangeAttributes_ = other.rangeAttributes_;
        width_ = other.width_;
        widthAttributes_ = other.widthAttributes_;
        usage_ = other.usage_;
        volatile_ = other.volatile_;
        access_ = other.access_;

        registerData_->clear();
        copyRegisterData(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::~AddressBlock()
//-----------------------------------------------------------------------------
AddressBlock::~AddressBlock()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlock::clone()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryBlockBase> AddressBlock::clone() const
{
    return QSharedPointer<MemoryBlockBase>(new AddressBlock(*this));
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getTypeIdentifier()
//-----------------------------------------------------------------------------
QString AddressBlock::getTypeIdentifier() const
{
    return typeIdentifier_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setTypeIdentifier()
//-----------------------------------------------------------------------------
void AddressBlock::setTypeIdentifier(QString const& newTypeIdentifier)
{
    typeIdentifier_ = newTypeIdentifier;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getRange()
//-----------------------------------------------------------------------------
QString AddressBlock::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setRange()
//-----------------------------------------------------------------------------
void AddressBlock::setRange(QString const& newRange)
{
    range_ = newRange;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getRangeAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> AddressBlock::getRangeAttributes() const
{
    return rangeAttributes_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setRangeAttributes()
//-----------------------------------------------------------------------------
void AddressBlock::setRangeAttributes(QMap<QString, QString> const& newRangeAttributes)
{
    rangeAttributes_.clear();
    rangeAttributes_ = newRangeAttributes;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getWidth()
//-----------------------------------------------------------------------------
QString AddressBlock::getWidth() const
{
    return width_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setWidth()
//-----------------------------------------------------------------------------
void AddressBlock::setWidth(QString const& newWidth)
{
    width_ = newWidth;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getWidthAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> AddressBlock::getWidthAttributes() const
{
    return widthAttributes_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setWidthAttributes()
//-----------------------------------------------------------------------------
void AddressBlock::setWidthAttributes(QMap<QString, QString> const& newWidthAttributes)
{
    widthAttributes_.clear();
    widthAttributes_ = newWidthAttributes;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getUsage()
//-----------------------------------------------------------------------------
General::Usage AddressBlock::getUsage() const
{
    return usage_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setUsage()
//-----------------------------------------------------------------------------
void AddressBlock::setUsage(General::Usage newUsage)
{
    usage_ = newUsage;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getVolatile()
//-----------------------------------------------------------------------------
QString AddressBlock::getVolatile() const
{
    return volatile_.toString();
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setVolatile()
//-----------------------------------------------------------------------------
void AddressBlock::setVolatile(bool newVolatileValue)
{
    volatile_.setValue(newVolatileValue);
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::clearVolatile()
//-----------------------------------------------------------------------------
void AddressBlock::clearVolatile()
{
    volatile_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access AddressBlock::getAccess() const
{
    return access_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setAccess()
//-----------------------------------------------------------------------------
void AddressBlock::setAccess(AccessTypes::Access newAccess)
{
    access_ = newAccess;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getRegisterData()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<RegisterBase> > > AddressBlock::getRegisterData() const
{
    return registerData_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setRegisterData()
//-----------------------------------------------------------------------------
void AddressBlock::setRegisterData(QSharedPointer<QList<QSharedPointer<RegisterBase> > > newRegisterData)
{
    registerData_ = newRegisterData;
}

//-----------------------------------------------------------------------------
// Function: addressblock::getAllRegisterOffsets()
//-----------------------------------------------------------------------------
QStringList AddressBlock::getAllRegisterOffsets() const
{
    QStringList registerOffsets;

    foreach (QSharedPointer<RegisterBase> registerBase, *registerData_)
    {
        QSharedPointer<Register> reg = registerBase.dynamicCast<Register>();
        if (reg)
        {
            registerOffsets.append(reg->getAddressOffset());
        }
    }

    return registerOffsets;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::copyRegisterData()
//-----------------------------------------------------------------------------
void AddressBlock::copyRegisterData(const AddressBlock& other)
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