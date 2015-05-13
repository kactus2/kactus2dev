//-----------------------------------------------------------------------------
// File: AddressSpaceExpressionsGatherer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from an address space and its memory maps.
//-----------------------------------------------------------------------------

#include "AddressSpaceExpressionsGatherer.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/MemoryMapExpressionsGatherer.h>

#include <IPXACTmodels/memorymap.h>

//-----------------------------------------------------------------------------
// Function: AddressSpaceExpressionsGatherer::AddressSpaceExpressionGatherer()
//-----------------------------------------------------------------------------
AddressSpaceExpressionGatherer::AddressSpaceExpressionGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceExpressionsGatherer::~AddressSpaceExpressionGatherer()
//-----------------------------------------------------------------------------
AddressSpaceExpressionGatherer::~AddressSpaceExpressionGatherer()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceExpressionsGatherer::getExpressions()
//-----------------------------------------------------------------------------
QStringList AddressSpaceExpressionGatherer::getExpressions(QSharedPointer<AddressSpace> addressSpace) const
{
    QStringList expressionList;

    expressionList.append(addressSpace->getRange());
    expressionList.append(addressSpace->getWidthExpression());

    if (addressSpace->hasLocalMemoryMap())
    {
        MemoryMapExpressionGatherer memoryMapGatherer;

        expressionList.append(memoryMapGatherer.getExpressions(addressSpace->getLocalMemoryMap()));
    }

    return expressionList;
}
