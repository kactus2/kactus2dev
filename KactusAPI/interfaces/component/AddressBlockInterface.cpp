//-----------------------------------------------------------------------------
// File: AddressBlockInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.05.2020
//
// Description:
// Interface for editing address blocks.
//-----------------------------------------------------------------------------

#include "AddressBlockInterface.h"

#include <AddressBlockExpressionsGatherer.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/AccessPolicy.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>

namespace
{
    std::string const BLOCK_TYPE = "addressBlock";
};

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::AddressBlockInterface()
//-----------------------------------------------------------------------------
AddressBlockInterface::AddressBlockInterface(QSharedPointer<AddressBlockValidator> blockValidator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    BusInterfaceInterface* busInterface, RegisterInterface* subInterface, ParametersInterface* parameterInterface):
MemoryBlockInterface(expressionParser, expressionFormatter, busInterface, parameterInterface),
validator_(blockValidator),
subInterface_(subInterface)
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getValidator()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryBlockValidator> AddressBlockInterface::getValidator() const
{
    return validator_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getDefaultName()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getDefaultName() const
{
    return BLOCK_TYPE;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::acceptBlock()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::acceptBlock(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> block = getAddressBlock(blockName);
    if (block)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getRangeFormattedExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getRangeFormattedExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (selectedBlock)
    {
        return formattedValueFor(selectedBlock->getRange()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getRangeExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getRangeExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getRange().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setRange()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setRange(std::string const& blockName, std::string const& newRange) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    selectedBlock->setRange(QString::fromStdString(newRange));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getWidthValue()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getWidthValue(std::string const& blockName, int const& baseNumber) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (selectedBlock)
    {
        return parseExpressionToBaseNumber(selectedBlock->getWidth(), baseNumber).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getWidthFormattedExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getWidthFormattedExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (selectedBlock)
    {
        return formattedValueFor(selectedBlock->getWidth()).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getWidthExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getWidthExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getWidth().toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setWidth()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setWidth(std::string const& blockName, std::string const& newWidth) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    selectedBlock->setWidth(QString::fromStdString(newWidth));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getUsageString()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getUsageString(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (selectedBlock)
    {
        return General::usage2Str(selectedBlock->getUsage()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getUsage()
//-----------------------------------------------------------------------------
General::Usage AddressBlockInterface::getUsage(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getUsage();
    }

    return General::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setUsage()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setUsage(std::string const& blockName, std::string const& newUsage) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    selectedBlock->setUsage(General::str2Usage(QString::fromStdString(newUsage), General::USAGE_COUNT));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getAccessString()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getAccessString(std::string const& blockName, bool getAccessPolicyAccess /*= false*/) const
{
    if (auto selectedBlock = getAddressBlock(blockName))
    {
        if (!getAccessPolicyAccess)
        {
            return AccessTypes::access2Str(selectedBlock->getAccess()).toStdString();
        }
        else
        {
            auto accessPolicies = selectedBlock->getAccessPolicies();

            if (!accessPolicies->isEmpty())
            {
                return AccessTypes::access2Str(accessPolicies->first()->getAccess()).toStdString();
            }
        }
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access AddressBlockInterface::getAccess(std::string const& blockName) const
{
    if (auto selectedBlock = getAddressBlock(blockName))
    {
        return selectedBlock->getAccess();
    }

    return AccessTypes::ACCESS_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setAccess()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setAccess(std::string const& blockName, std::string const& newAccess, bool setAccessPolicyAccess /*= false*/) const
{
    auto selectedBlock = getAddressBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    if (!setAccessPolicyAccess)
    {
        selectedBlock->setAccess(AccessTypes::str2Access(QString::fromStdString(newAccess), AccessTypes::ACCESS_COUNT));
        return true;
    }
    else
    {
        if (auto accessPolicies = selectedBlock->getAccessPolicies(); accessPolicies->isEmpty() == false)
        {
            auto accessPolicy = selectedBlock->getAccessPolicies()->first();

            // Remove access policy, if new access is empty.
            if (accessPolicy && newAccess.empty())
            {
                selectedBlock->getAccessPolicies()->clear();
                return true;
            }

            accessPolicy->setAccess(AccessTypes::str2Access(QString::fromStdString(newAccess), 
                AccessTypes::ACCESS_COUNT));
            return true;
        }
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getVolatile()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getVolatile(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getVolatile().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setVolatile()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setVolatile(std::string const& blockName, bool newVolatile) const
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    selectedBlock->setVolatile(newVolatile);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setVolatile()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setVolatile(std::string const& blockName, std::string const& newVolatileValue)
{
    QSharedPointer<AddressBlock> selectedBlock = getAddressBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    if (newVolatileValue == ("true"))
    {
        selectedBlock->setVolatile(true);
    }
    else if (newVolatileValue == ("false"))
    {
        selectedBlock->setVolatile(false);
    }
    else
    {
        selectedBlock->clearVolatile();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int AddressBlockInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const
{
    int referencesInMemoryBlock = MemoryBlockInterface::getAllReferencesToIdInItem(itemName, valueID);
    int totalReferencesToParameter = 0;

    QSharedPointer<AddressBlock> selectedItem = getAddressBlock(itemName);
    if (selectedItem)
    {
        QString idString = QString::fromStdString(valueID);

        int referencesInRange = selectedItem->getRange().count(idString);
        int referencesInWidth = selectedItem->getWidth().count(idString);

        totalReferencesToParameter = referencesInMemoryBlock + referencesInRange + referencesInWidth;
    }

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::validateItems()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::validateItems() const
{
    for (auto const& currentItemName : getItemNames())
    {
        QSharedPointer<AddressBlock> block = getAddressBlock(currentItemName);
        if (block)
        {
            if (!validator_->validate(block, getAddressUnitBits()))
            {
                return false;
            }
        }
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::hasValidRange()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::hasValidRange(std::string const& itemName) const
{
    QSharedPointer<AddressBlock> block = getAddressBlock(itemName);
    if (block)
    {
        return validator_->hasValidRange(block);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::hasValidWidth()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::hasValidWidth(std::string const& itemName) const
{
    QSharedPointer<AddressBlock> block = getAddressBlock(itemName);
    if (block)
    {
        return validator_->hasValidWidth(block);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::hasValidUsage()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::hasValidUsage(std::string const& itemName) const
{
    QSharedPointer<AddressBlock> block = getAddressBlock(itemName);
    if (block)
    {
        return validator_->hasValidUsage(block);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> AddressBlockInterface::getAddressBlock(std::string const& itemName) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(itemName);
    if (selectedBlock)
    {
        QSharedPointer<AddressBlock> selectedAddressBlock = selectedBlock.dynamicCast<AddressBlock>();
        if (selectedAddressBlock)
        {
            return selectedAddressBlock;
        }
    }

    return QSharedPointer<AddressBlock>();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::addBlock()
//-----------------------------------------------------------------------------
void AddressBlockInterface::addBlock(int const& row, std::string const& newBlockName)
{
    QString newBase = getNewBlockBaseAddress();

    QSharedPointer<AddressBlock> newBlock(new AddressBlock());
    newBlock->setWidth("32");
    newBlock->setRange("4");
    newBlock->setBaseAddress(newBase);
    newBlock->setName(getUniqueName(newBlockName, BLOCK_TYPE));

    getBlocksData()->insert(row, newBlock);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getCopiedBlocks()
//-----------------------------------------------------------------------------
QList<QSharedPointer<MemoryBlockBase> > AddressBlockInterface::getCopiedBlocks(std::vector<int> const& selectedRows) const
{
    QList<QSharedPointer<MemoryBlockBase> > copiedBlocks;
    for (auto index : selectedRows)
    {
        QSharedPointer<AddressBlock> block = getAddressBlock(getIndexedItemName(index));
        if (block)
        {
            copiedBlocks.append(block);
        }
    }

    return copiedBlocks;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getMimeType()
//-----------------------------------------------------------------------------
QString AddressBlockInterface::getMimeType() const
{
    return QString("text/xml/ipxact:addressBlock");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::createCopyBlock()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryBlockBase> AddressBlockInterface::createCopyBlock(QSharedPointer<MemoryBlockBase> copiedItem)
const
{
    QSharedPointer<AddressBlock> copiedBlock = copiedItem.dynamicCast<AddressBlock>();
    if (copiedBlock)
    {
        QSharedPointer<AddressBlock> newBlock(new AddressBlock(*copiedBlock.data()));
        newBlock->setName(getUniqueName(newBlock->name().toStdString(), BLOCK_TYPE));

        return newBlock;
    }

    return QSharedPointer<MemoryBlockBase>();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::countItems()
//-----------------------------------------------------------------------------
int AddressBlockInterface::countItems(QList<QSharedPointer<MemoryBlockBase> > itemList) const
{
    int itemCount = 0;
    for (auto blockItem : itemList)
    {
        QSharedPointer<AddressBlock> addressBlockItem = blockItem.dynamicCast<AddressBlock>();
        if (addressBlockItem)
        {
            itemCount++;
        }
    }

    return itemCount;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getExpressionsInSelectedItems()
//-----------------------------------------------------------------------------
std::vector<std::string> AddressBlockInterface::getExpressionsInSelectedItems(std::vector<std::string> const& itemNames) const
{
    std::vector<std::string> expressionList;

    AddressBlockExpressionGatherer gatherer;

    for (auto const& name : itemNames)
    {
        QSharedPointer<AddressBlock> currentBlock = getAddressBlock(name);
        QStringList itemExpressions = gatherer.getExpressions(currentBlock);
        for (auto const& expression : itemExpressions)
        {
            expressionList.push_back(expression.toStdString());
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getSubInterface()
//-----------------------------------------------------------------------------
RegisterInterface* AddressBlockInterface::getSubInterface() const
{
    return subInterface_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::hasRegisters()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::hasRegisters(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> block = getAddressBlock(blockName);
    if (block && block->getRegisterData() && !block->getRegisterData()->isEmpty())
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getAccessPolicyCount()
//-----------------------------------------------------------------------------
int AddressBlockInterface::getAccessPolicyCount(std::string const& blockName) const
{
    if (auto block = getAddressBlock(blockName))
    {
        return block->getAccessPolicies()->size();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::addAccessPolicy()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::addAccessPolicy(std::string const& blockName) const
{
    if (auto block = getAddressBlock(blockName))
    {
        QSharedPointer<AccessPolicy> newAccessPolicy(new AccessPolicy());
        block->getAccessPolicies()->append(newAccessPolicy);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> AddressBlockInterface::getItem(std::string const& itemName) const
{
    return getAddressBlock(itemName);
}
