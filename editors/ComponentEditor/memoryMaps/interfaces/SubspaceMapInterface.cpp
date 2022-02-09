//-----------------------------------------------------------------------------
// File: SubspaceMapInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.01.2022
//
// Description:
// Interface for editing subspace maps.
//-----------------------------------------------------------------------------

#include "SubspaceMapInterface.h"

#include <IPXACTmodels/Component/SubSpaceMap.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Segment.h>

#include <editors/ComponentEditor/busInterfaces/interfaces/BusInterfaceInterface.h>

using namespace std;

namespace
{
    std::string const SUBSPACEMAP_TYPE = "subspaceMap";
};

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::SubspaceMapInterface()
//-----------------------------------------------------------------------------
SubspaceMapInterface::SubspaceMapInterface(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter, BusInterfaceInterface* busInterface,
    ParametersInterface* parameterInterface):
MemoryBlockInterface(expressionParser, expressionFormatter, parameterInterface),
addressSpaces_(),
busInterfaceInterface_(busInterface)
{

}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getDefaultName()
//-----------------------------------------------------------------------------
std::string SubspaceMapInterface::getDefaultName() const
{
    return SUBSPACEMAP_TYPE;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::setAddressSpaces()
//-----------------------------------------------------------------------------
void SubspaceMapInterface::setAddressSpaces(QSharedPointer<QList<QSharedPointer<AddressSpace> > > newAddressSpaces)
{
    addressSpaces_ = newAddressSpaces;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getSubspaceMap()
//-----------------------------------------------------------------------------
QSharedPointer<SubSpaceMap> SubspaceMapInterface::getSubspaceMap(std::string const& itemName) const
{
    QSharedPointer<MemoryBlockBase> memoryBlock = getBlock(itemName);
    if (memoryBlock)
    {
        QSharedPointer<SubSpaceMap> subspaceBlock = memoryBlock.dynamicCast<SubSpaceMap>();
        if (subspaceBlock)
        {
            return subspaceBlock;
        }
    }

    return QSharedPointer<SubSpaceMap>();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getMasterReference()
//-----------------------------------------------------------------------------
std::string SubspaceMapInterface::getMasterReference(std::string const& itemName) const
{
    QSharedPointer<SubSpaceMap> editedItem = getSubspaceMap(itemName);
    if (editedItem)
    {
        return editedItem->getMasterReference().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::setMasterReference()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::setMasterReference(std::string const& itemName, std::string const& newMasterReference)
{
    QSharedPointer<SubSpaceMap> editedItem = getSubspaceMap(itemName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setMasterReference(QString::fromStdString(newMasterReference));
    return true;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getSegmentReference()
//-----------------------------------------------------------------------------
std::string SubspaceMapInterface::getSegmentReference(std::string const& itemName) const
{
    QSharedPointer<SubSpaceMap> editedItem = getSubspaceMap(itemName);
    if (editedItem)
    {
        return editedItem->getSegmentReference().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::setSegmentReference()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::setSegmentReference(std::string const& itemName, std::string const& newSegmentReference)
{
    QSharedPointer<SubSpaceMap> editedItem = getSubspaceMap(itemName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setSegmentReference(QString::fromStdString(newSegmentReference));
    return true;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getRangeValue()
//-----------------------------------------------------------------------------
std::string SubspaceMapInterface::getRangeValue(std::string const& blockName, int const& baseNumber) const
{
    std::string rangeValue = "0";

    QSharedPointer<SubSpaceMap> subMap = getSubspaceMap(blockName);
    if (subMap)
    {
        QSharedPointer<AddressSpace> referencedSpace = getReferencedAddressSpace(blockName);
        if (referencedSpace)
        {
            QSharedPointer<Segment> referencedSegment = getReferencedSegment(referencedSpace, blockName);

            quint64 memoryRange = 0;

            if (referencedSegment)
            {
                memoryRange = parseExpressionToDecimal(referencedSegment->getRange()).toULongLong();
            }
            else
            {
                memoryRange = parseExpressionToDecimal(referencedSpace->getRange()).toULongLong();
            }

            QString rangeQ = QString::number(memoryRange);

            rangeValue = parseExpressionToBaseNumber(rangeQ, baseNumber).toStdString();
        }
    }

    return rangeValue;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getReferencedAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> SubspaceMapInterface::getReferencedAddressSpace(std::string const& subspaceMapName)
const
{
    std::string masterBusReference = getMasterReference(subspaceMapName);

    QString spaceReference =
        QString::fromStdString(busInterfaceInterface_->getAddressSpaceReference(masterBusReference));
    if (!spaceReference.isEmpty())
    {
        for (auto space : *addressSpaces_)
        {
            if (space->name() == spaceReference)
            {
                return space;
            }
        }
    }

    return QSharedPointer<AddressSpace>();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getReferencedSegment()
//-----------------------------------------------------------------------------
QSharedPointer<Segment> SubspaceMapInterface::getReferencedSegment(QSharedPointer<AddressSpace> referencedSpace,
    std::string const& subspaceMapName) const
{
    QString segmentReference = QString::fromStdString(getSegmentReference(subspaceMapName));
    if (!segmentReference.isEmpty())
    {
        for (auto segment : *referencedSpace->getSegments())
        {
            if (segment->name() == segmentReference)
            {
                return segment;
            }
        }
    }

    return QSharedPointer<Segment>();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::validateItems()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::validateItems() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::itemHasValidName(string const& itemName) const
{
    QSharedPointer<SubSpaceMap> subMap = getSubspaceMap(itemName);
    if (subMap)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::hasValidBaseAddress()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::hasValidBaseAddress(std::string const& itemName) const
{
    QSharedPointer<SubSpaceMap> subMap = getSubspaceMap(itemName);
    if (subMap)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::hasValidIsPresent(std::string const& itemName) const
{
    QSharedPointer<SubSpaceMap> subMap = getSubspaceMap(itemName);
    if (subMap)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::addBlock()
//-----------------------------------------------------------------------------
void SubspaceMapInterface::addBlock(int const& row, std::string const& newBlockName)
{
    QString newBase = getNewBlockBaseAddress();

    QSharedPointer<SubSpaceMap> newSubMap(new SubSpaceMap());
    newSubMap->setBaseAddress(newBase);
    newSubMap->setName(getUniqueName(newBlockName, SUBSPACEMAP_TYPE));

    getBlocksData()->insert(row, newSubMap);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getExpressionsInSelectedItems()
//-----------------------------------------------------------------------------
std::vector<std::string> SubspaceMapInterface::getExpressionsInSelectedItems(std::vector<std::string> itemNames)
    const
{
    std::vector<std::string> expressionList;

    for (auto name : itemNames)
    {
        QSharedPointer<SubSpaceMap> currentSubMap = getSubspaceMap(name);
        if (currentSubMap)
        {
            expressionList.push_back(currentSubMap->getBaseAddress().toStdString());
            expressionList.push_back(currentSubMap->getIsPresent().toStdString());
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getCopiedBlocks()
//-----------------------------------------------------------------------------
QList<QSharedPointer<MemoryBlockBase> > SubspaceMapInterface::getCopiedBlocks(std::vector<int> selectedRows) const
{
    QList<QSharedPointer<MemoryBlockBase> > copiedBlocks;
    for (auto index : selectedRows)
    {
        QSharedPointer<SubSpaceMap> block = getSubspaceMap(getIndexedItemName(index));
        if (block)
        {
            copiedBlocks.append(block);
        }
    }

    return copiedBlocks;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getMimeType()
//-----------------------------------------------------------------------------
QString SubspaceMapInterface::getMimeType() const
{
    return QString("text/xml/ipxact:subspaceMap");
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::createCopyBlock()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryBlockBase> SubspaceMapInterface::createCopyBlock(QSharedPointer<MemoryBlockBase> copiedItem)
const
{
    QSharedPointer<SubSpaceMap> copiedBlock = copiedItem.dynamicCast<SubSpaceMap>();
    if (copiedBlock)
    {
        QSharedPointer<SubSpaceMap> newBlock(new SubSpaceMap(*copiedBlock.data()));
        newBlock->setName(getUniqueName(newBlock->name().toStdString(), SUBSPACEMAP_TYPE));

        return newBlock;
    }

    return QSharedPointer<MemoryBlockBase>();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::()
//-----------------------------------------------------------------------------
int SubspaceMapInterface::countItems(QList<QSharedPointer<MemoryBlockBase> > itemList) const
{
    int itemCount = 0;
    for (auto blockItem : itemList)
    {
        QSharedPointer<SubSpaceMap> subspaceMapItem = blockItem.dynamicCast<SubSpaceMap>();
        if (subspaceMapItem)
        {
            itemCount++;
        }
    }

    return itemCount;
}
