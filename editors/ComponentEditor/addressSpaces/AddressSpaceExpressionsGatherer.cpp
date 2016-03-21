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

#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressSpace.h>

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
    expressionList.append(addressSpace->getWidth());
    expressionList.append(getExpressionsFromSegments(addressSpace->getSegments()));

    if (addressSpace->hasLocalMemoryMap())
    {
        MemoryMapExpressionGatherer memoryMapGatherer;

        QSharedPointer<MemoryMap> transformedMemoryMap (new MemoryMap(addressSpace->getLocalMemoryMap()->name()));
        transformedMemoryMap->setAddressUnitBits(addressSpace->getAddressUnitBits());

        foreach (QSharedPointer<MemoryBlockBase> memoryBlock, *addressSpace->getLocalMemoryMap()->getMemoryBlocks())
        {
            transformedMemoryMap->getMemoryBlocks()->append(memoryBlock);
        }

        expressionList.append(memoryMapGatherer.getExpressions(transformedMemoryMap));
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceExpressionsGatherer::getExpressionsFromSegments()
//-----------------------------------------------------------------------------
QStringList AddressSpaceExpressionGatherer::getExpressionsFromSegments(
    QSharedPointer<QList<QSharedPointer<Segment> > > segmentList) const
{
    QStringList segmentExpressions;

    foreach (QSharedPointer<Segment> segment, *segmentList)
    {
        segmentExpressions.append(segment->getAddressOffset());
        segmentExpressions.append(segment->getRange());
    }

    return segmentExpressions;
}
