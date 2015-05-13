//-----------------------------------------------------------------------------
// File: MemoryMapExpressionsGatherer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a memory map and its memory remaps.
//-----------------------------------------------------------------------------

#include "MemoryMapExpressionsGatherer.h"
#include "MemoryRemapExpressionGatherer.h"

#include <IPXACTmodels/MemoryRemap.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapExpressionsGatherer::MemoryMapExpressionGatherer()
//-----------------------------------------------------------------------------
MemoryMapExpressionGatherer::MemoryMapExpressionGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapExpressionsGatherer::~MemoryMapExpressionGatherer()
//-----------------------------------------------------------------------------
MemoryMapExpressionGatherer::~MemoryMapExpressionGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapExpressionsGatherer::getExpressions()
//-----------------------------------------------------------------------------
QStringList MemoryMapExpressionGatherer::getExpressions(QSharedPointer<MemoryMap> memoryMap) const
{
    QStringList expressionList;

    MemoryRemapExpressionGatherer memoryRemapGatherer;

    expressionList.append(memoryRemapGatherer.getExpressions(memoryMap));

    foreach (QSharedPointer<MemoryRemap> memoryRemap, *memoryMap->getMemoryRemaps())
    {
        expressionList.append(memoryRemapGatherer.getExpressions(memoryRemap));
    }

    return expressionList;
}
