//-----------------------------------------------------------------------------
// File: addressspacescene.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.12.2012
//
// Description:
// The graphics scene containing the segments and local memory map of an address space.
//-----------------------------------------------------------------------------

#include "addressspacescene.h"
#include "segmentgraphitem.h"
#include "addressspacevisualizationitem.h"
#include "addressspacegapitem.h"
#include "localaddrblockgraphitem.h"
#include "AddressSpaceConflictedItem.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <common/graphicsItems/visualizeritem.h>

//-----------------------------------------------------------------------------
// Function: AddressSpaceScene()
//-----------------------------------------------------------------------------
AddressSpaceScene::AddressSpaceScene(QSharedPointer<AddressSpace> addrSpace,
                                     QSharedPointer<ExpressionParser> expressionParser,
									 QObject *parent):
MemoryAlignScene(expressionParser, parent),
addrSpace_(addrSpace)
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceScene::getWidth()
//-----------------------------------------------------------------------------
QString AddressSpaceScene::getWidth() const
{
    return addrSpace_->getWidth();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceScene::getRange()
//-----------------------------------------------------------------------------
QString AddressSpaceScene::getRange() const
{
    return addrSpace_->getRange();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceScene::createRegionItems()
//-----------------------------------------------------------------------------
void AddressSpaceScene::createRegionItems()
{
    quint64 addressSpaceEnd = addressSpaceLastAddress();
    QSharedPointer<QList<QSharedPointer<Segment> > > segments = addrSpace_->getSegments();
    auto width = getWidth();
    for (QSharedPointer<Segment> current : *segments)
    {
        if (current->getIsPresent().isEmpty() ||
            expressionParser_->parseExpression(current->getIsPresent()).toInt() == 1)
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
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceScene::createAddressBlockItems()
//-----------------------------------------------------------------------------
void AddressSpaceScene::createAddressBlockItems()
{
    quint64 addressSpaceEnd = addressSpaceLastAddress();

    auto width = getWidth();
    QSharedPointer<MemoryMapBase> localMap = addrSpace_->getLocalMemoryMap();
    if (!localMap.isNull() && !localMap->getMemoryBlocks().isNull())
    {
        for (QSharedPointer<MemoryBlockBase> block : *localMap->getMemoryBlocks())
        {
            QSharedPointer<AddressBlock> addrBlock = block.dynamicCast<AddressBlock>();
            if (addrBlock &&
                (addrBlock->getIsPresent().isEmpty() ||
                    expressionParser_->parseExpression(addrBlock->getIsPresent()).toInt() == 1))
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

