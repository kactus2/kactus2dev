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
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/SubspaceMapValidator.h>

#include <BusInterfaceInterface.h>

using namespace std;

namespace
{
    std::string const SUBSPACEMAP_TYPE = "subspaceMap";
};

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::SubspaceMapInterface()
//-----------------------------------------------------------------------------
SubspaceMapInterface::SubspaceMapInterface(QSharedPointer<SubspaceMapValidator> subspaceValidator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    BusInterfaceInterface* busInterface, ParametersInterface* parameterInterface):
MemoryBlockInterface(expressionParser, expressionFormatter, busInterface, parameterInterface),
validator_(subspaceValidator)
{

}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> SubspaceMapInterface::getItem(std::string const& itemName) const
{
    return getSubspaceMap(itemName);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getValidator()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryBlockValidator> SubspaceMapInterface::getValidator() const
{
    return validator_;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::getDefaultName()
//-----------------------------------------------------------------------------
std::string SubspaceMapInterface::getDefaultName() const
{
    return SUBSPACEMAP_TYPE;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::acceptBlock()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::acceptBlock(std::string const& blockName) const
{
    QSharedPointer<SubSpaceMap> block = getSubspaceMap(blockName);
    if (block)
    {
        return true;
    }

    return false;
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
std::string SubspaceMapInterface::getInitiatorReference(std::string const& itemName) const
{
    QSharedPointer<SubSpaceMap> editedItem = getSubspaceMap(itemName);
    if (editedItem)
    {
        return editedItem->getInitiatorReference().toStdString();
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

    editedItem->setInitiatorReference(QString::fromStdString(newMasterReference));
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
// Function: SubspaceMapInterface::getWidthValue()
//-----------------------------------------------------------------------------
std::string SubspaceMapInterface::getWidthValue(std::string const& subspaceName, int const& baseNumber) const
{
    QSharedPointer<AddressSpace> space =
        getReferencedAddressSpace(QString::fromStdString(getInitiatorReference(subspaceName)));
    if (space)
    {
        return parseExpressionToBaseNumber(space->getWidth(), baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::validateItems()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::validateItems() const
{
    for (auto currentItemName : getItemNames())
    {
        QSharedPointer<SubSpaceMap> block = getSubspaceMap(currentItemName);
        if (block)
        {
            if (!validator_->validate(block))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::hasValidMasterReference()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::hasValidMasterReference(std::string const& itemName) const
{
    QSharedPointer<SubSpaceMap> subMap = getSubspaceMap(itemName);
    if (subMap)
    {
        return validator_->hasValidMasterReference(subMap);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapInterface::hasValidSegmentReference()
//-----------------------------------------------------------------------------
bool SubspaceMapInterface::hasValidSegmentReference(std::string const& itemName) const
{
    QSharedPointer<SubSpaceMap> subMap = getSubspaceMap(itemName);
    if (subMap)
    {
        return validator_->hasValidSegmentReference(subMap);
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
