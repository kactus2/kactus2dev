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

#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/addressblock.h>

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
QStringList MemoryRemapExpressionGatherer::getExpressions(QSharedPointer<AbstractMemoryMap> memoryMap) const
{
    QStringList expressionList;

    AddressBlockExpressionGatherer addressBlockGatherer;

    foreach(QSharedPointer<MemoryMapItem> memoryMapItem, memoryMap->getItems())
    {
        QSharedPointer<AddressBlock> addressBlock = memoryMapItem.dynamicCast<AddressBlock>();
        if (addressBlock)
        {
            expressionList.append(addressBlockGatherer.getExpressions(addressBlock));
        }
    }

    return expressionList;
}
