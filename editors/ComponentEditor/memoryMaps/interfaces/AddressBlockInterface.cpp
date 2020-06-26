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

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/AddressBlockExpressionsGatherer.h>

#include <IPXACTmodels/common/AccessTypes.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>

#include <QMimeData>
#include <QApplication>
#include <QClipboard>

using namespace std;

namespace
{
    std::string const BLOCK_TYPE = "addressBlock";
};

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::AddressBlockInterface()
//-----------------------------------------------------------------------------
AddressBlockInterface::AddressBlockInterface(QSharedPointer<AddressBlockValidator> blockValidator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    RegisterInterface* subInterface):
ParameterizableInterface(expressionParser, expressionFormatter),
blockData_(),
validator_(blockValidator),
subInterface_(subInterface),
addressUnitBits_("")
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setAddressBlocks()
//-----------------------------------------------------------------------------
void AddressBlockInterface::setAddressBlocks(
    QSharedPointer<QList<QSharedPointer<MemoryBlockBase>>> newAddressBlocks)
{
    blockData_ = newAddressBlocks;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getItemIndex()
//-----------------------------------------------------------------------------
int AddressBlockInterface::getItemIndex(string const& itemName) const
{
    for (int i = 0; i < blockData_->size(); ++i)
    {
        if (blockData_->at(i)->name().toStdString() == itemName)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
string AddressBlockInterface::getIndexedItemName(int const& itemIndex) const
{
    string blockName = "";
    if (itemIndex >= 0 && itemIndex < blockData_->size())
    {
        blockName = blockData_->at(itemIndex)->name().toStdString();
    }

    return blockName;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::itemCount()
//-----------------------------------------------------------------------------
int AddressBlockInterface::itemCount() const
{
    return blockData_->count();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getItemNames()
//-----------------------------------------------------------------------------
vector<string> AddressBlockInterface::getItemNames() const
{
    vector<string> names;
    for (auto block : *blockData_)
    {
        names.push_back(block->name().toStdString());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setName()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setName(string const& currentName, string const& newName)
{
    QSharedPointer<AddressBlock> editedItem = getBlock(currentName);
    if (!editedItem)
    {
        return false;
    }

    QString uniqueNewName = getUniqueName(newName, BLOCK_TYPE);

    editedItem->setName(uniqueNewName);
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getDescription()
//-----------------------------------------------------------------------------
string AddressBlockInterface::getDescription(string const& itemName) const
{
    QSharedPointer<AddressBlock> editedItem = getBlock(itemName);
    if (editedItem)
    {
        return editedItem->description().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setDescription()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setDescription(string const& itemName, string const& newDescription)
{
    QSharedPointer<AddressBlock> editedItem = getBlock(itemName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getBaseAddressValue()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getBaseAddressValue(std::string const& blockName, int const& baseNumber) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return parseExpressionToBaseNumber(selectedBlock->getBaseAddress(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getBaseAddressFormattedExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getBaseAddressFormattedExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return formattedValueFor(selectedBlock->getBaseAddress()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getBaseAddressExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getBaseAddressExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getBaseAddress().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setBaseAddress()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setBaseAddress(std::string const& blockName, std::string const& newBaseAdress) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }
    
    selectedBlock->setBaseAddress(QString::fromStdString(newBaseAdress));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getIsPresentValue()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getIsPresentValue(std::string const& blockName, int const& baseNumber) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return parseExpressionToBaseNumber(selectedBlock->getIsPresent(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getIsPresentFormattedExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getIsPresentFormattedExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return formattedValueFor(selectedBlock->getIsPresent()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getIsPresentExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getIsPresentExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getIsPresent().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setIsPresent()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setIsPresent(std::string const& blockName, std::string const& newIsPresent) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    selectedBlock->setIsPresent(QString::fromStdString(newIsPresent));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getRangeValue()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getRangeValue(std::string const& blockName, int const& baseNumber) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return parseExpressionToBaseNumber(selectedBlock->getRange(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getRangeFormattedExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getRangeFormattedExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return formattedValueFor(selectedBlock->getRange()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getRangeExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getRangeExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getRange().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setRange()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setRange(std::string const& blockName, std::string const& newRange) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
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
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return parseExpressionToBaseNumber(selectedBlock->getWidth(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getWidthFormattedExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getWidthFormattedExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return formattedValueFor(selectedBlock->getWidth()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getWidthExpression()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getWidthExpression(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getWidth().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setWidth()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setWidth(std::string const& blockName, std::string const& newWidth) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
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
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
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
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
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
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
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
std::string AddressBlockInterface::getAccessString(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return AccessTypes::access2Str(selectedBlock->getAccess()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getAccess()
//-----------------------------------------------------------------------------
AccessTypes::Access AddressBlockInterface::getAccess(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getAccess();
    }

    return AccessTypes::ACCESS_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setAccess()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::setAccess(std::string const& blockName, std::string const& newAccess) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    selectedBlock->setAccess(AccessTypes::str2Access(QString::fromStdString(newAccess), AccessTypes::ACCESS_COUNT));
    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getVolatile()
//-----------------------------------------------------------------------------
std::string AddressBlockInterface::getVolatile(std::string const& blockName) const
{
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
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
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
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
    QSharedPointer<AddressBlock> selectedBlock = getBlock(blockName);
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
int AddressBlockInterface::getAllReferencesToIdInItem(const string& itemName, string const&  valueID) const
{
    QSharedPointer<AddressBlock> selectedItem = getBlock(itemName);

    QString idString = QString::fromStdString(valueID);

    int referencesInBaseAddress = selectedItem->getBaseAddress().count(idString);
    int referencesInIsPresent = selectedItem->getIsPresent().count(idString);
    int referencesInRange = selectedItem->getRange().count(idString);
    int referencesInWidth = selectedItem->getWidth().count(idString);

    int totalReferencesToParameter =
        referencesInBaseAddress + referencesInIsPresent + referencesInRange + referencesInWidth;

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::validateItems()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::validateItems() const
{
    for (auto currentItem : *blockData_)
    {
        QSharedPointer<AddressBlock> block = currentItem.dynamicCast<AddressBlock>();
        if (block)
        {
            if (!validator_->validate(block, addressUnitBits_))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::itemHasValidName(string const& itemName) const
{
    QSharedPointer<AddressBlock> block = getBlock(itemName);
    if (block)
    {
        return validator_->hasValidName(block);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::hasValidBaseAddress()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::hasValidBaseAddress(std::string const& itemName) const
{
    QSharedPointer<AddressBlock> block = getBlock(itemName);
    if (block)
    {
        return validator_->hasValidBaseAddress(block);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::hasValidRange()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::hasValidRange(std::string const& itemName) const
{
    QSharedPointer<AddressBlock> block = getBlock(itemName);
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
    QSharedPointer<AddressBlock> block = getBlock(itemName);
    if (block)
    {
        return validator_->hasValidWidth(block);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::hasValidIsPresent(std::string const& itemName) const
{
    QSharedPointer<AddressBlock> block = getBlock(itemName);
    if (block)
    {
        return validator_->hasValidIsPresent(block);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::hasValidUsage()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::hasValidUsage(std::string const& itemName) const
{
    QSharedPointer<AddressBlock> block = getBlock(itemName);
    if (block)
    {
        return validator_->hasValidUsage(block);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> AddressBlockInterface::getBlock(std::string const& itemName) const
{
    for (auto currentBlock : *blockData_)
    {
        if (currentBlock->name().toStdString() == itemName)
        {
            QSharedPointer<AddressBlock> addressBlock = currentBlock.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                return addressBlock;
            }
        }
    }

    return QSharedPointer<AddressBlock>();
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::setAddressUnitBits()
//-----------------------------------------------------------------------------
void AddressBlockInterface::setAddressUnitBits(std::string const& newAddressUnitbits)
{
    addressUnitBits_ = QString::fromStdString(newAddressUnitbits);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::addBlock()
//-----------------------------------------------------------------------------
void AddressBlockInterface::addBlock(int const& row, std::string const& newBlockName)
{
    quint64 lastBaseAddress = 0;
    quint64 lastRange = 0;

    for (int itemIndex = 0; itemIndex < itemCount(); ++itemIndex)
    {
        std::string itemName = getIndexedItemName(itemIndex);
        QString calculatedExpression = QString::fromStdString(getBaseAddressValue(itemName, 10));
        quint64 addressBlockBaseAddress = calculatedExpression.toULongLong();

        if (addressBlockBaseAddress > lastBaseAddress || (itemIndex == itemCount() - 1 && lastBaseAddress == 0))
        {
            QSharedPointer<AddressBlock> addressBlock = blockData_->at(itemIndex).dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                lastBaseAddress = addressBlockBaseAddress;
                lastRange = QString::fromStdString(getRangeValue(itemName, 10)).toULongLong();
            }
        }
    }


    // convert the address to hexadecimal form
    QString newBase = QString::number(lastBaseAddress + lastRange, 16);
    newBase = newBase.toUpper();
    newBase.prepend("'h");

    QSharedPointer<AddressBlock> newBlock(new AddressBlock());
    newBlock->setWidth("32");
    newBlock->setRange("4");
    newBlock->setBaseAddress(newBase);
    newBlock->setName(getUniqueName(newBlockName, BLOCK_TYPE));

    blockData_->insert(row, newBlock);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::removeBlock()
//-----------------------------------------------------------------------------
bool AddressBlockInterface::removeBlock(std::string const& blockName)
{
    QSharedPointer<AddressBlock> removedItem = getBlock(blockName);
    if (!removedItem)
    {
        return false;
    }

    return blockData_->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::copyRows()
//-----------------------------------------------------------------------------
void AddressBlockInterface::copyRows(std::vector<int> selectedRows)
{
    QList<QSharedPointer<MemoryBlockBase> > copiedBlocks;
    for(auto index : selectedRows)
    {
        QSharedPointer<MemoryBlockBase> memoryBlock = blockData_->at(index);
        copiedBlocks.append(memoryBlock);
    }

    QVariant memoryBlockVariant;
    memoryBlockVariant.setValue(copiedBlocks);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:addressBlock", QByteArray());
    newMimeData->setImageData(memoryBlockVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::pasteRows()
//-----------------------------------------------------------------------------
std::vector<std::string> AddressBlockInterface::pasteRows()
{
    std::vector<std::string> pastedItemNames;

    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<MemoryBlockBase> > >())
        {
            QList<QSharedPointer<MemoryBlockBase> > copiedItemList =
                pasteVariant.value<QList<QSharedPointer<MemoryBlockBase>>>();

            foreach(QSharedPointer<MemoryBlockBase> copiedItem, copiedItemList)
            {
                QSharedPointer<AddressBlock> copiedBlock = copiedItem.dynamicCast<AddressBlock>();
                if (copiedBlock)
                {
                    QSharedPointer<AddressBlock> newBlock(new AddressBlock(*copiedBlock.data()));
                    newBlock->setName(getUniqueName(newBlock->name().toStdString(), BLOCK_TYPE));
                    blockData_->append(newBlock);

                    pastedItemNames.push_back(newBlock->name().toStdString());
                }
            }
        }
    }

    return pastedItemNames;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getPasteRowCount()
//-----------------------------------------------------------------------------
int AddressBlockInterface::getPasteRowCount() const
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();
    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<AddressBlock> > >())
        {
            QList<QSharedPointer<AddressBlock> > newRegisters =
                pasteVariant.value<QList<QSharedPointer<AddressBlock> > >();
            return newRegisters.count();
        }
    }
    
    return 0;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getExpressionsInSelectedItems()
//-----------------------------------------------------------------------------
std::vector<std::string> AddressBlockInterface::getExpressionsInSelectedItems(std::vector<std::string> itemNames)
    const
{
    std::vector<std::string> expressionList;

    AddressBlockExpressionGatherer gatherer;

    for (auto name : itemNames)
    {
        QSharedPointer<AddressBlock> currentBlock = getBlock(name);
        QStringList itemExpressions = gatherer.getExpressions(currentBlock);
        for (auto expression : itemExpressions)
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
    QSharedPointer<AddressBlock> block = getBlock(blockName);
    if (block && block->getRegisterData() && !block->getRegisterData()->isEmpty())
    {
        return true;
    }

    return false;
}
