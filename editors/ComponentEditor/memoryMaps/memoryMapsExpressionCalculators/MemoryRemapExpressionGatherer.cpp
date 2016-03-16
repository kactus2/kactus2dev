//-----------------------------------------------------------------------------
// File: MemoryRemapExpressionsGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a memory remap and its address blocks.
//-----------------------------------------------------------------------------

#include "MemoryRemapExpressionGatherer.h"
#include "AddressBlockExpressionsGatherer.h"

#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>

//-----------------------------------------------------------------------------
// Function: MemoryRemapExpressionGatherer::MemoryRemapExpressionGatherer()
//-----------------------------------------------------------------------------
MemoryRemapExpressionGatherer::MemoryRemapExpressionGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemapExpressionGatherer::~MemoryRemapExpressionGatherer()
//-----------------------------------------------------------------------------
MemoryRemapExpressionGatherer::~MemoryRemapExpressionGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryRemapExpressionsGatherer::getExpressions()
//-----------------------------------------------------------------------------
QStringList MemoryRemapExpressionGatherer::getExpressions(QSharedPointer<MemoryMapBase> memoryMap) const
{
    QStringList expressionList;

    AddressBlockExpressionGatherer addressBlockGatherer;

    foreach(QSharedPointer<MemoryBlockBase> memoryBlock, *memoryMap->getMemoryBlocks())
    {
        QSharedPointer<AddressBlock> addressBlock = memoryBlock.dynamicCast<AddressBlock>();
        if (addressBlock)
        {
            expressionList.append(addressBlockGatherer.getExpressions(addressBlock));
        }
    }

    return expressionList;
}
