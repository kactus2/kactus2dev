//-----------------------------------------------------------------------------
// File: RegionScene.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.10.2023
//
// Description:
// The graphics scene containing the regions and referenced memory map of a CPU.
//-----------------------------------------------------------------------------

#include "RegionScene.h"
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/segmentgraphitem.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacegapitem.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/localaddrblockgraphitem.h>
#include <editors/ComponentEditor/addressSpaces/addressSpaceVisualizer/AddressSpaceConflictedItem.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <IPXACTmodels/utilities/Search.h>

#include <common/graphicsItems/visualizeritem.h>

//-----------------------------------------------------------------------------
// Function: RegionScene()
//-----------------------------------------------------------------------------
RegionScene::RegionScene(QSharedPointer<Cpu> cpu,
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
    QSharedPointer<ExpressionParser> expressionParser,
    QObject* parent) :
MemoryAlignScene(expressionParser, parent),
    cpu_(cpu),
    memoryMaps_(memoryMaps)
{

}

//-----------------------------------------------------------------------------
// Function: RegionScene::getWidth()
//-----------------------------------------------------------------------------
QString RegionScene::getWidth() const
{
    return cpu_->getWidth();
}

//-----------------------------------------------------------------------------
// Function: RegionScene::getRange()
//-----------------------------------------------------------------------------
QString RegionScene::getRange() const
{
    return cpu_->getRange();
}

//-----------------------------------------------------------------------------
// Function: RegionScene::createRegionItems()
//-----------------------------------------------------------------------------
void RegionScene::createRegionItems()
{
    quint64 addressSpaceEnd = addressSpaceLastAddress();

    QSharedPointer<QList<QSharedPointer<Region> > > segments = cpu_->getRegions();
    auto width = getWidth();
    for (QSharedPointer<Region> current : *segments)
    {
        auto segItem = new SegmentGraphItem(current, width, expressionParser_);
        addItem(segItem);

        if (segItem->getOffset() > addressSpaceEnd)
        {
            exceedingSegments_.insert(segItem->getOffset(), segItem);
        }
        else
        {
            segmentItems_.insert(segItem->getOffset(), segItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegionScene::createAddressBlockItems()
//-----------------------------------------------------------------------------
void RegionScene::createAddressBlockItems()
{
    quint64 addressSpaceEnd = addressSpaceLastAddress();

    auto width = getWidth();
    auto memoryMapName = cpu_->getMemoryMapReference();

    auto memoryMap = Search::findByName(memoryMapName, *memoryMaps_);

    if (!memoryMap.isNull() && !memoryMap->getMemoryBlocks().isNull())
    {
        for (QSharedPointer<MemoryBlockBase> block : *memoryMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addrBlock = block.dynamicCast<AddressBlock>();
            if (addrBlock)
            {
                auto blockItem = new LocalAddrBlockGraphItem(addrBlock, width, expressionParser_);
                addItem(blockItem);

                if (blockItem->getOffset() > addressSpaceEnd)
                {
                    exceedingAddrBlocks_.insert(blockItem->getOffset(), blockItem);
                }
                else
                {
                    addrBlockItems_.insert(blockItem->getOffset(), blockItem);
                }
            }
        }
    }
}

