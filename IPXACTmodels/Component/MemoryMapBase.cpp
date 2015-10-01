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

/*
//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::AbstractMemoryMap()
//-----------------------------------------------------------------------------
AbstractMemoryMap::AbstractMemoryMap(QDomNode &abstractMemoryMapNode):
NameGroup(),
id_(),
items_()
{
    for (int i = 0; i < abstractMemoryMapNode.childNodes().count(); ++i)
    {
        QDomNamedNodeMap attributeMap = abstractMemoryMapNode.attributes();
        id_ = attributeMap.namedItem("spirit:id").nodeValue();
        id_ = XmlUtils::removeWhiteSpace(id_);

        QDomNode tempNode = abstractMemoryMapNode.childNodes().at(i);

        if (tempNode.nodeName() == QString("spirit:addressBlock"))
        {
            AddressBlock* temp = new AddressBlock(tempNode);
            items_.append(QSharedPointer<AddressBlock>(temp));
        }
    }
}*/

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

/*
//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::isValid()
//-----------------------------------------------------------------------------
bool AbstractMemoryMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList, const QString& parentIdentifier) const
{
    bool valid = true;
    const QString thisIdentifier(QObject::tr("memory map %1").arg(name()));

    if (name().isEmpty())
    {
        errorList.append(QObject::tr("No name specified for memory map within %1").arg(parentIdentifier));
        valid = false;
    }

    QStringList memItemNames;
    foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
        if (memItemNames.contains(memItem->name()))
        {
            errorList.append(QObject::tr("%1 contains several memory map items with name %2").arg(
                memItem->name()));
            valid = false;
        }
        else
        {
            memItemNames.append(memItem->name());
        }

        if (!memItem->isValid(componentChoices, errorList, thisIdentifier))
        {
            valid = false;
        }
    }

    return valid;
}*/
/*
//-----------------------------------------------------------------------------
// Function: AbstractMemoryMap::isValid()
//-----------------------------------------------------------------------------
bool AbstractMemoryMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
    if (name().isEmpty())
    {
        return false;
    }

    QStringList memItemNames;
    foreach (QSharedPointer<MemoryMapItem> memItem, items_)
    {
        if (memItemNames.contains(memItem->name()))
        {
            return false;
        }
        else
        {
            memItemNames.append(memItem->name());
        }

        if (!memItem->isValid(componentChoices))
        {
            return false;
        }
    }

    return true;
}*/

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
// Function: MemoryMapBase::()
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