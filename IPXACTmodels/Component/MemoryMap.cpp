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
MemoryMap::MemoryMap(QString const& name /* = QString() */):
MemoryMapBase(name),
Extendable(),
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
Extendable(other),
memoryRemaps_(new QList<QSharedPointer<MemoryRemap> > ()),
addressUnitBits_(other.addressUnitBits_),
shared_(other.shared_)
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
        Extendable::operator=(other);

        memoryRemaps_->clear();
        copyMemoryRemaps(other);
        
        addressUnitBits_ = other.addressUnitBits_;
        shared_ = other.shared_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: memorymap::~MemoryMap()
//-----------------------------------------------------------------------------
MemoryMap::~MemoryMap()
{

}
/*
//-----------------------------------------------------------------------------
// Function: memorymap::isValid()
//-----------------------------------------------------------------------------
bool MemoryMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList remapStateNames, QStringList& errorList,
    const QString& parentIdentifier) const
{
    bool valid = AbstractMemoryMap::isValid(componentChoices, errorList, parentIdentifier);

    const QString thisIdentifier(QObject::tr("memory map %1").arg(name()));

    QStringList memoryRemapNames;
    foreach (QSharedPointer<MemoryRemap> singleMemoryRemap, *memoryRemaps_)
    {
        if (memoryRemapNames.contains(singleMemoryRemap->name()))
        {
            errorList.append(QObject::tr("%1 contains several memory remaps with the name %2").arg(
                thisIdentifier).arg(singleMemoryRemap->name()));
            valid = false;
        }
        else
        {
            memoryRemapNames.append(singleMemoryRemap->name());
        }
        if (!singleMemoryRemap->isValid(componentChoices, remapStateNames, errorList, thisIdentifier))
        {
            valid = false;
        }
    }

    return valid;
}*/
/*
//-----------------------------------------------------------------------------
// Function: memorymap::isValid()
//-----------------------------------------------------------------------------
bool MemoryMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList remapStateNames) const
{
    bool valid = AbstractMemoryMap::isValid(componentChoices);

    QStringList memoryRemapNames;
    foreach (QSharedPointer<MemoryRemap> singleMemoryRemap, *memoryRemaps_)
    {
        if (memoryRemapNames.contains(singleMemoryRemap->name()))
        {
            return false;
        }
        else
        {
            memoryRemapNames.append(singleMemoryRemap->name());
        }

        if (!singleMemoryRemap->isValid(componentChoices, remapStateNames))
        {
            return false;
        }
    }

    return valid;
}*/

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
// Function: MemoryMap::copyMemoryRemaps()
//-----------------------------------------------------------------------------
void MemoryMap::copyMemoryRemaps(const MemoryMap& other)
{
    foreach (QSharedPointer<MemoryRemap> memoryRemap, *other.memoryRemaps_)
    {
        if (memoryRemap)
        {
            QSharedPointer<MemoryRemap> copy = QSharedPointer<MemoryRemap>(new MemoryRemap(*memoryRemap.data()));
            memoryRemaps_->append(copy);
        }
    }
}