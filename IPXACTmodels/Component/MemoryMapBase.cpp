//-----------------------------------------------------------------------------
// File: MemoryMapBase.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Contains common memory-map elements.
//-----------------------------------------------------------------------------

#include "MemoryMapBase.h"

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::MemoryMapBase()
//-----------------------------------------------------------------------------
MemoryMapBase::MemoryMapBase(QString const& name /* = QString() */):
NameGroup(name),
isPresent_(),
memoryBlocks_(new QList<QSharedPointer<MemoryBlockBase> > ())
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::MemoryMapBase()
//-----------------------------------------------------------------------------
MemoryMapBase::MemoryMapBase(const MemoryMapBase& other):
NameGroup(other),
isPresent_(other.isPresent_),
memoryBlocks_(new QList<QSharedPointer<MemoryBlockBase> > ())
{
    copyMemoryBlocks(other);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::operator=()
//-----------------------------------------------------------------------------
MemoryMapBase& MemoryMapBase::operator=( const MemoryMapBase& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        isPresent_ = other.isPresent_;

        memoryBlocks_->clear();
        copyMemoryBlocks(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::~MemoryMapBase()
//-----------------------------------------------------------------------------
MemoryMapBase::~MemoryMapBase()
{
    memoryBlocks_.clear();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::elementName()
//-----------------------------------------------------------------------------
QString MemoryMapBase::elementName() const
{
    return QLatin1String("memory map");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::getIsPresent()
//-----------------------------------------------------------------------------
QString MemoryMapBase::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::setIsPresent()
//-----------------------------------------------------------------------------
void MemoryMapBase::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::getMemoryBlocks()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > MemoryMapBase::getMemoryBlocks() const
{
    return memoryBlocks_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::setMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBase::setMemoryBlocks(QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > newMemoryBlocks)
{
    memoryBlocks_->clear();
    memoryBlocks_ = newMemoryBlocks;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::hasMemoryBlocks()
//-----------------------------------------------------------------------------
bool MemoryMapBase::hasMemoryBlocks() const
{
    return !memoryBlocks_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapBase::copyMemoryBlocks()
//-----------------------------------------------------------------------------
void MemoryMapBase::copyMemoryBlocks(const MemoryMapBase& other)
{
    foreach (QSharedPointer<MemoryBlockBase> memoryBlockBase, *other.memoryBlocks_)
    {
        if (memoryBlockBase)
        {
            QSharedPointer<MemoryBlockBase> copy = memoryBlockBase->clone();
            memoryBlocks_->append(copy);
        }
    }
}