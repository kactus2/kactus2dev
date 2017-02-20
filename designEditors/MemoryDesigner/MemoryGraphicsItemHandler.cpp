//-----------------------------------------------------------------------------
// File: MemoryGraphicsItemHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.01.2017
//
// Description:
// Constructs the memory graphics items for the memory design diagram.
//-----------------------------------------------------------------------------

#include "MemoryGraphicsItemHandler.h"

#include <designEditors/MemoryDesigner/ConnectivityGraph.h>
#include <designEditors/MemoryDesigner/ConnectivityComponent.h>
#include <designEditors/MemoryDesigner/MemoryItem.h>

#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/AddressSpaceGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryColumn.h>

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::MemoryGraphicsItemHandler()
//-----------------------------------------------------------------------------
MemoryGraphicsItemHandler::MemoryGraphicsItemHandler():
filterAddressSpaceSegments_(true),
filterAddressBlocks_(true),
filterRegisters_(true),
filterFields_(true),
memoryMapItems_()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::~MemoryGraphicsItemHandler()
//-----------------------------------------------------------------------------
MemoryGraphicsItemHandler::~MemoryGraphicsItemHandler()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::setFilterAddressSpaceSegments()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::setFilterAddressSpaceSegments(bool filterSegments)
{
    filterAddressSpaceSegments_ = filterSegments;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::addressSpaceSegmentsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::addressSpaceSegmentsAreFiltered() const
{
    return filterAddressSpaceSegments_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::setFilterAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::setFilterAddressBlocks(bool filterBlocks)
{
    filterAddressBlocks_ = filterBlocks;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::addressBlocksAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::addressBlocksAreFiltered() const
{
    return filterAddressBlocks_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::setFilterAddressBlockRegisters()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::setFilterAddressBlockRegisters(bool filterRegisters)
{
    filterRegisters_ = filterRegisters;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::addressBlockRegistersAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::addressBlockRegistersAreFiltered() const
{
    return filterRegisters_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::setFilterFields()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::setFilterFields(bool filterFields)
{
    filterFields_ = filterFields;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::fieldsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryGraphicsItemHandler::fieldsAreFiltered() const
{
    return filterFields_;
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::createMemoryItems()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::createMemoryItems(QSharedPointer<ConnectivityGraph> connectionGraph,
    MemoryColumn* spaceColumn, MemoryColumn* memoryMapColumn)
{
    memoryMapItems_.clear();

    foreach (QSharedPointer<ConnectivityComponent> componentInstance, connectionGraph->getInstances())
    {
        foreach (QSharedPointer<MemoryItem> memoryItem, componentInstance->getMemories())
        {
            if (memoryItem->getType().compare(
                MemoryDesignerConstants::ADDRESSSPACE_TYPE, Qt::CaseInsensitive) == 0)
            {
                createAddressSpaceItem(memoryItem, componentInstance, spaceColumn);
            }
            else if (memoryItem->getType().compare(
                MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
            {
                createMemoryMapItem(memoryItem, componentInstance, memoryMapColumn);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::createAddressSpaceItem()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::createAddressSpaceItem(QSharedPointer<MemoryItem> spaceItem,
    QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn)
{
    if (containingColumn)
    {
        AddressSpaceGraphicsItem* spaceGraphicsItem = new AddressSpaceGraphicsItem(
            spaceItem, containingInstance, filterAddressSpaceSegments_, containingColumn);
        containingColumn->addItem(spaceGraphicsItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::createMemoryMapItem()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::createMemoryMapItem(QSharedPointer<MemoryItem> mapItem,
    QSharedPointer<ConnectivityComponent> containingInstance, MemoryColumn* containingColumn)
{
    if (containingColumn)
    {
        MemoryMapGraphicsItem* mapGraphicsItem = new MemoryMapGraphicsItem(
            mapItem, filterAddressBlocks_, filterRegisters_, filterFields_, containingInstance, containingColumn);
        containingColumn->addItem(mapGraphicsItem);

        memoryMapItems_.append(mapGraphicsItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryGraphicsItemHandler::createFieldOverlapItems()
//-----------------------------------------------------------------------------
void MemoryGraphicsItemHandler::createFieldOverlapItems()
{
    foreach (MemoryMapGraphicsItem* mapItem, memoryMapItems_)
    {
        mapItem->createFieldOverlapItems();
    }
}
