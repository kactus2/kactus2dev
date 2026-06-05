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
#include "MemoryArray.h"
#include "AccessPolicy.h"

#include <IPXACTmodels/utilities/Copy.h>

//-----------------------------------------------------------------------------
// Function: AddressBlock::AddressBlock()
//-----------------------------------------------------------------------------
AddressBlock::AddressBlock(QString const& name /* = QString() */, QString const& baseAddress /* = QString() */):
MemoryBlockBase(name, baseAddress),
typeIdentifier_(),
range_(),
rangeAttributes_(),
width_(),
widthAttributes_()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlock::AddressBlock()
//-----------------------------------------------------------------------------
AddressBlock::AddressBlock(const AddressBlock &other):
MemoryBlockBase(other),
misalignmentAllowed_(other.misalignmentAllowed_),
addressBlockDefinitionRef_(other.addressBlockDefinitionRef_),
typeDefinitionsRef_(other.typeDefinitionsRef_),
typeIdentifier_(other.typeIdentifier_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_),
width_(other.width_),
widthAttributes_(other.widthAttributes_)
{
    copyRegisterData(other);

    if (other.memoryArray_)
    {
        memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray(*other.memoryArray_));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::operator=()
//-----------------------------------------------------------------------------
AddressBlock& AddressBlock::operator=( const AddressBlock& other )
{
    if (this != &other)
    {
        MemoryBlockBase::operator=(other);
        misalignmentAllowed_ = other.misalignmentAllowed_;
        addressBlockDefinitionRef_ = other.addressBlockDefinitionRef_;
        typeDefinitionsRef_ = other.typeDefinitionsRef_;
        typeIdentifier_ = other.typeIdentifier_;
        range_ = other.range_;
        rangeAttributes_ = other.rangeAttributes_;
        width_ = other.width_;
        widthAttributes_ = other.widthAttributes_;

        registerData_->clear();
        copyRegisterData(other);


        memoryArray_.clear();
        if (other.memoryArray_)
        {
            memoryArray_ = QSharedPointer<MemoryArray>(new MemoryArray(*other.memoryArray_));
        }
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
    widthAttributes_ = newWidthAttributes;
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
      registerOffsets.append(registerBase->getAddressOffset());
    }

    return registerOffsets;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getMemoryArray()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryArray> AddressBlock::getMemoryArray() const
{
    return memoryArray_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setMemoryArray()
//-----------------------------------------------------------------------------
void AddressBlock::setMemoryArray(QSharedPointer<MemoryArray> newMemoryArray)
{
    memoryArray_ = newMemoryArray;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getMisalignmentAllowed()
//-----------------------------------------------------------------------------
QString AddressBlock::getMisalignmentAllowed() const
{
    return misalignmentAllowed_.toString();
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setMisalignmentAllowed()
//-----------------------------------------------------------------------------
void AddressBlock::setMisalignmentAllowed(bool newMisalignmentAllowed)
{
    misalignmentAllowed_.setValue(newMisalignmentAllowed);
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getAddressBlockDefinitionRef()
//-----------------------------------------------------------------------------
QString AddressBlock::getAddressBlockDefinitionRef() const
{
    return addressBlockDefinitionRef_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setAddressBlockDefinitionRef()
//-----------------------------------------------------------------------------
void AddressBlock::setAddressBlockDefinitionRef(QString const& newAddressBlockDefinitionRef)
{
    addressBlockDefinitionRef_ = newAddressBlockDefinitionRef;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::getTypeDefinitionsRef()
//-----------------------------------------------------------------------------
QString AddressBlock::getTypeDefinitionsRef() const
{
    return typeDefinitionsRef_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::setTypeDefinitionsRef()
//-----------------------------------------------------------------------------
void AddressBlock::setTypeDefinitionsRef(QString const& newTypeDefinitionsRef)
{
    typeDefinitionsRef_ = newTypeDefinitionsRef;
}

//-----------------------------------------------------------------------------
// Function: AddressBlock::copyRegisterData()
//-----------------------------------------------------------------------------
void AddressBlock::copyRegisterData(const AddressBlock& other)
{
    for (auto registerBase : *other.registerData_)
    {
        if (registerBase)
        {
            QSharedPointer<RegisterBase> copy = registerBase->clone();
            registerData_->append(copy);
        }
    }
}
