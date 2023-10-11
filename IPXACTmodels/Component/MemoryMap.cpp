//-----------------------------------------------------------------------------
// File: MemoryMap.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Describes the ipxact:memoryMap element.
//-----------------------------------------------------------------------------

#include "MemoryMap.h"

#include "MemoryRemap.h"

//-----------------------------------------------------------------------------
// Function: MemoryMap::MemoryMap()
//-----------------------------------------------------------------------------
MemoryMap::MemoryMap(QString const& name):
MemoryMapBase(name),
    memoryRemaps_(new QList<QSharedPointer<MemoryRemap> > ()),
    addressUnitBits_(),
    shared_()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMap::MemoryMap()
//-----------------------------------------------------------------------------
MemoryMap::MemoryMap(const MemoryMap& other):
MemoryMapBase(other),
memoryRemaps_(new QList<QSharedPointer<MemoryRemap> > ()),
addressUnitBits_(other.addressUnitBits_),
shared_(other.shared_),
memoryMapDefinitionReference_(other.memoryMapDefinitionReference_),
typeDefinitionsReference_(other.typeDefinitionsReference_)

{
    copyMemoryRemaps(other);
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::operator=()
//-----------------------------------------------------------------------------
MemoryMap& MemoryMap::operator=(const MemoryMap& other)
{
    if (this != &other)
    {
        MemoryMapBase::operator=(other);

        memoryRemaps_->clear();
        copyMemoryRemaps(other);
        
        addressUnitBits_ = other.addressUnitBits_;
        shared_ = other.shared_;
        memoryMapDefinitionReference_ = other.memoryMapDefinitionReference_;
        typeDefinitionsReference_ = other.typeDefinitionsReference_;

    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: memorymap::~MemoryMap()
//-----------------------------------------------------------------------------
MemoryMap::~MemoryMap()
{

}

//-----------------------------------------------------------------------------
// Function: memorymap::getMemoryRemaps()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MemoryRemap> > > MemoryMap::getMemoryRemaps() const
{
    return memoryRemaps_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::setMemoryRemaps()
//-----------------------------------------------------------------------------
void MemoryMap::setMemoryRemaps(QSharedPointer<QList<QSharedPointer<MemoryRemap> > > newMemoryRemaps)
{
    memoryRemaps_ = newMemoryRemaps;
}

//-----------------------------------------------------------------------------
// Function: memorymap::getAddressUnitBits()
//-----------------------------------------------------------------------------
QString MemoryMap::getAddressUnitBits() const
{
    return addressUnitBits_;
}

//-----------------------------------------------------------------------------
// Function: memorymap::setAddressUnitBits()
//-----------------------------------------------------------------------------
void MemoryMap::setAddressUnitBits(QString const& newAddressUnitBits)
{
	addressUnitBits_ = newAddressUnitBits;
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::getShared()
//-----------------------------------------------------------------------------
QString MemoryMap::getShared() const
{
    return shared_.toString();
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::setShared()
//-----------------------------------------------------------------------------
void MemoryMap::setShared(bool newSharedStatus)
{
    shared_.setValue(newSharedStatus);
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::clearShared()
//-----------------------------------------------------------------------------
void MemoryMap::clearShared()
{
    shared_.setUnspecified();
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::getMemoryMapDefinitionReference()
//-----------------------------------------------------------------------------
QString MemoryMap::getMemoryMapDefinitionReference() const
{
    return memoryMapDefinitionReference_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::setMemoryMapDefinitionReference()
//-----------------------------------------------------------------------------
void MemoryMap::setMemoryMapDefinitionReference(QString const& newDefinitionRef)
{
    memoryMapDefinitionReference_ = newDefinitionRef;
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::getTypeDefinitionsReference()
//-----------------------------------------------------------------------------
QString MemoryMap::getTypeDefinitionsReference() const
{
    return typeDefinitionsReference_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::setTypeDefinitionsReference()
//-----------------------------------------------------------------------------
void MemoryMap::setTypeDefinitionsReference(QString const& newTypeDefinitionsRef)
{
    typeDefinitionsReference_ = newTypeDefinitionsRef;
}

//-----------------------------------------------------------------------------
// Function: MemoryMap::copyMemoryRemaps()
//-----------------------------------------------------------------------------
void MemoryMap::copyMemoryRemaps(const MemoryMap& other)
{
    for (auto memoryRemap : *other.memoryRemaps_)
    {
        if (memoryRemap)
        {
            auto copy = QSharedPointer<MemoryRemap>(new MemoryRemap(*memoryRemap));
            memoryRemaps_->append(copy);
        }
    }
}