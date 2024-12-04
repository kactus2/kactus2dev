//-----------------------------------------------------------------------------
// File: MemoryDesignConstructor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.01.2017
//
// Description:
// Constructs the items for the memory design diagram.
//-----------------------------------------------------------------------------

#include "MemoryDesignConstructor.h"

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <editors/MemoryDesigner/MemoryColumnHandler.h>
#include <editors/MemoryDesigner/MemoryGraphicsItemHandler.h>
#include <editors/MemoryDesigner/MemoryConnectionHandler.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryColumn.h>

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::MemoryDesignConstructor()
//-----------------------------------------------------------------------------
MemoryDesignConstructor::MemoryDesignConstructor(QSharedPointer<GraphicsColumnLayout> layout):
QObject(),
columnHandler_(new MemoryColumnHandler(layout)),
itemHandler_(new MemoryGraphicsItemHandler()),
connectionHandler_(new MemoryConnectionHandler(columnHandler_)),
widthBoundary_(0)
{
    connect(itemHandler_.data(), SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)),
        this, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::~MemoryDesignConstructor()
//-----------------------------------------------------------------------------
MemoryDesignConstructor::~MemoryDesignConstructor()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::setCondenseMemoryItems()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::setCondenseMemoryItems(bool condenseMemoryItems)
{
    connectionHandler_->setCondenseMemoryItems(condenseMemoryItems);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::memoryItemsAreCondensed()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::memoryItemsAreCondensed() const
{
    return connectionHandler_->memoryItemsAreCondensed();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::setFilterAddressSpaceChains()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::setFilterAddressSpaceChains(bool filterChains)
{
    connectionHandler_->setFilterAddressSpaceChains(filterChains);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::addressSpaceChainsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::addressSpaceChainsAreFiltered() const
{
    return connectionHandler_->addressSpaceChainsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::condenseFieldItems()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::condenseFieldItems()
{
    if (!addressBlockRegistersAreFiltered() && !fieldsAreFiltered()  && widthBoundary_ > 0)
    {
        changeMemoryMapWidths(-widthBoundary_);
        widthBoundary_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::extendFieldItems()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::extendFieldItems()
{
    if (!addressBlockRegistersAreFiltered() && !fieldsAreFiltered())
    {
        qreal widthChange = 0;

        foreach (MemoryColumn* memoryColumn, columnHandler_->getMemoryMapColumns())
        {
            widthChange = qMax(widthChange, memoryColumn->getMaximumNeededChangeInWidth());
        }

        if (widthChange > widthBoundary_)
        {
            int futureColumnWidth = columnHandler_->getMemoryMapColumnWidth() + widthChange;
            int widthRemainder = futureColumnWidth % 10;
            if (widthRemainder != 9)
            {
                widthRemainder = 9 - widthRemainder;
                widthChange += widthRemainder;
            }

            changeMemoryMapWidths(widthChange);
            widthBoundary_ += widthChange;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::setFilterAddressSpaceSegments()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::setFilterAddressSpaceSegments(bool filterSegments)
{
    itemHandler_->setFilterAddressSpaceSegments(filterSegments);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::addressSpaceSegmentsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::addressSpaceSegmentsAreFiltered() const
{
    return itemHandler_->addressSpaceSegmentsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::setFilterAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::setFilterAddressBlocks(bool filterBlocks)
{
    itemHandler_->setFilterAddressBlocks(filterBlocks);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::addressBlocksAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::addressBlocksAreFiltered() const
{
    return itemHandler_->addressBlocksAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::setFilterAddressBlockRegisters()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::setFilterAddressBlockRegisters(bool filterRegisters)
{
    itemHandler_->setFilterAddressBlockRegisters(filterRegisters);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::addressBlockRegistersAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::addressBlockRegistersAreFiltered() const
{
    return itemHandler_->addressBlockRegistersAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::setFilterFields()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::setFilterFields(bool filterFields)
{
    itemHandler_->setFilterFields(filterFields);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::fieldsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::fieldsAreFiltered() const
{
    return itemHandler_->fieldsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::filterUnconnectedMemoryItems()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::filterUnconnectedMemoryItems(bool filterUnconnected)
{
    itemHandler_->filterUnconnectedMemoryItems(filterUnconnected);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::unconnectedMemoryItemsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::unconnectedMemoryItemsAreFiltered() const
{
    return itemHandler_->unconnectedMemoryItemsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::filterMemoryOverlapItems()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::filterMemoryOverlapItems(bool filterOverlap)
{
    itemHandler_->filterMemoryOverlapItems(filterOverlap);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::memoryOverlapItemsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::memoryOverlapItemsAreFiltered() const
{
    return itemHandler_->memoryOverlapItemsAreFiltered();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::constructMemoryDesignItems()
//-----------------------------------------------------------------------------
bool MemoryDesignConstructor::constructMemoryDesignItems(QSharedPointer<ConnectivityGraph> connectionGraph)
{
    connectionHandler_->clearConnectionLists();
    widthBoundary_ = 0;

    if (connectionGraph)
    {
        if (itemHandler_->addressBlockRegistersAreFiltered() ||
            (itemHandler_->addressBlocksAreFiltered() && itemHandler_->fieldsAreFiltered()))
        {
            columnHandler_->setNewMemoryMapColumnWidth(MemoryDesignerConstants::SPACECOLUMNWIDTH);
        }
        else if (itemHandler_->fieldsAreFiltered())
        {
            columnHandler_->setNewMemoryMapColumnWidth(MemoryDesignerConstants::SPACECOLUMNWIDTH +
                (MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 - 5));
        }
        else if (itemHandler_->addressBlocksAreFiltered())
        {
            columnHandler_->setNewMemoryMapColumnWidth(MemoryDesignerConstants::MEMORYMAPCOLUMNWIDTH -
                (MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 - 5));
        }
        else
        {
            columnHandler_->setNewMemoryMapColumnWidth(MemoryDesignerConstants::MEMORYMAPCOLUMNWIDTH);
        }

        columnHandler_->createInitialColumns();

        MemoryColumn* addressSpaceColumn =
            columnHandler_->findColumnByName(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
        MemoryColumn* memoryMapColumn =
            columnHandler_->findColumnByName(MemoryDesignerConstants::MEMORYMAPCOLUM_NAME);
        if (addressSpaceColumn && memoryMapColumn)
        {
            itemHandler_->createMemoryItems(connectionGraph, addressSpaceColumn, memoryMapColumn);
            connectionHandler_->createConnectedItems(connectionGraph, addressSpaceColumn, memoryMapColumn);
            itemHandler_->createFieldOverlapItems();

            filterUnconnectedMemoryItems(unconnectedMemoryItemsAreFiltered());

            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::setNewLayout()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::setNewLayout(QSharedPointer<GraphicsColumnLayout> newLayout)
{
    columnHandler_->setNewLayout(newLayout);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::getWidthBoundary()
//-----------------------------------------------------------------------------
int MemoryDesignConstructor::getWidthBoundary() const
{
    return widthBoundary_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::setNewWidthBoundary()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::setNewWidthBoundary(int newWidthBoundary)
{
    widthBoundary_ = newWidthBoundary;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignConstructor::changeWidths()
//-----------------------------------------------------------------------------
void MemoryDesignConstructor::changeMemoryMapWidths(qreal deltaWidth)
{
    columnHandler_->changeColumnWidths(deltaWidth);
    connectionHandler_->reDrawConnectionsAndCollisions();
}
