//-----------------------------------------------------------------------------
// File: MemoryTreeFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 31.8.2023
//
// Description:
// Creates a tree representation of component memory items.
//-----------------------------------------------------------------------------

#include "MemoryTreeFactory.h"

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: MemoryTreeFactory::MemoryTreeFactory()
//-----------------------------------------------------------------------------
MemoryTreeFactory::MemoryTreeFactory(QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces) :
    memoryMaps_(memoryMaps),
    addressSpaces_(addressSpaces)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryTreeFactory::create()
//-----------------------------------------------------------------------------
TreeItem* MemoryTreeFactory::create() const
{
    auto root = new TreeItem("Memory", QString("Root"));

    for (auto const& memoryMap : *memoryMaps_)
    {
        auto memoryMapItem = new TreeItem(memoryMap->name(), QObject::tr("Memory map"));

        for (auto const& memoryBlock : *memoryMap->getMemoryBlocks())
        {
            createAddressBlockItem(memoryBlock, memoryMapItem);
        }

        root->addChild(memoryMapItem);
    }

    for (auto const& addressSpace : *addressSpaces_)
    {
        auto addressSpaceItem = new TreeItem(addressSpace->name(), QObject::tr("Address space"));

        if (addressSpace->hasLocalMemoryMap())
        {
            for (auto const& memoryBlock : *addressSpace->getLocalMemoryMap()->getMemoryBlocks())
            {
                createAddressBlockItem(memoryBlock, addressSpaceItem);
            }
        }

        root->addChild(addressSpaceItem);
    }

    return root;
}

//-----------------------------------------------------------------------------
// Function: MemoryTreeFactory::createAddressBlockItem()
//-----------------------------------------------------------------------------
void MemoryTreeFactory::createAddressBlockItem(QSharedPointer<MemoryBlockBase> memoryBlock, TreeItem* parentItem) const
{
    auto const& addressBlock = memoryBlock.dynamicCast<AddressBlock>();
    if (addressBlock)
    {
        auto addressBlockItem = new TreeItem(addressBlock->name(), QObject::tr("Address block"));

        for (auto const& registerBase : *addressBlock->getRegisterData())
        {
            createRegisterItem(registerBase, addressBlockItem);
        }

        parentItem->addChild(addressBlockItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryTreeFactory::createRegisterItem()
//-----------------------------------------------------------------------------
void MemoryTreeFactory::createRegisterItem(QSharedPointer<RegisterBase> registerBase, TreeItem* parentItem) const
{
    auto const& reg = registerBase.dynamicCast<Register>();
    auto const& registerFile = registerBase.dynamicCast<RegisterFile>();
    if (reg)
    {
        auto registerItem = new TreeItem(reg->name(), QObject::tr("Register"));

        for (auto const& field : *reg->getFields())
        {
            auto fieldItem = new TreeItem(field->name(), QObject::tr("Field"));
            registerItem->addChild(fieldItem);
        }

        parentItem->addChild(registerItem);
    }
    else if (registerFile)
    {
        auto registerFileItem = new TreeItem(registerFile->name(), QObject::tr("Register file"));

        for (auto const& registerData : *registerFile->getRegisterData())
        {
            createRegisterItem(registerData, registerFileItem);
        }

        parentItem->addChild(registerFileItem);
    }
}
