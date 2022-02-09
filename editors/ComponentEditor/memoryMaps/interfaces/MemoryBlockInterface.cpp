//-----------------------------------------------------------------------------
// File: MemoryBlockInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 31.01.2022
//
// Description:
// Parent interface for editing memory blocks.
//-----------------------------------------------------------------------------

#include "MemoryBlockInterface.h"

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/SubSpaceMap.h>

#include <editors/ComponentEditor/parameters/ParametersInterface.h>

#include <QMimeData>
#include <QApplication>
#include <QClipboard>

using namespace std;

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::MemoryBlockInterface()
//-----------------------------------------------------------------------------
MemoryBlockInterface::MemoryBlockInterface(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter, ParametersInterface* parameterInterface):
ParameterizableInterface(expressionParser, expressionFormatter),
NameGroupInterface(),
blockData_(),
addressUnitBits_(""),
parameterInterface_(parameterInterface)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::setAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryBlockInterface::setMemoryBlocks(QSharedPointer<QList<QSharedPointer<MemoryBlockBase>>> newMemoryBlocks)
{
    blockData_ = newMemoryBlocks;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getBlocksData()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MemoryBlockBase> > > MemoryBlockInterface::getBlocksData()
{
    return blockData_;
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::MemoryBlockBase()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryBlockBase> MemoryBlockInterface::getBlock(std::string const& itemName) const
{
    for (auto currentBlock : *blockData_)
    {
        if (currentBlock->name().toStdString() == itemName)
        {
            return currentBlock;
        }
    }

    return QSharedPointer<MemoryBlockBase>();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getAddressUnitBits()
//-----------------------------------------------------------------------------
QString MemoryBlockInterface::getAddressUnitBits() const
{
    return addressUnitBits_;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getItemIndex()
//-----------------------------------------------------------------------------
int MemoryBlockInterface::getItemIndex(string const& itemName) const
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
// Function: MemoryBlockInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
string MemoryBlockInterface::getIndexedItemName(int const& itemIndex) const
{
    string blockName = "";
    if (itemIndex >= 0 && itemIndex < blockData_->size())
    {
        blockName = blockData_->at(itemIndex)->name().toStdString();
    }

    return blockName;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::itemCount()
//-----------------------------------------------------------------------------
int MemoryBlockInterface::itemCount() const
{
    return blockData_->count();
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getItemNames()
//-----------------------------------------------------------------------------
vector<string> MemoryBlockInterface::getItemNames() const
{
    vector<string> names;
    for (auto block : *blockData_)
    {
        names.push_back(block->name().toStdString());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::setName()
//-----------------------------------------------------------------------------
bool MemoryBlockInterface::setName(string const& currentName, string const& newName)
{
    QSharedPointer<MemoryBlockBase> editedItem = getBlock(currentName);
    if (!editedItem)
    {
        return false;
    }

    QString uniqueNewName = getUniqueName(newName, getDefaultName());
    editedItem->setName(uniqueNewName);

    return true;
}


//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getDescription()
//-----------------------------------------------------------------------------
string MemoryBlockInterface::getDescription(string const& itemName) const
{
    QSharedPointer<MemoryBlockBase> editedItem = getBlock(itemName);
    if (editedItem)
    {
        return editedItem->description().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::setDescription()
//-----------------------------------------------------------------------------
bool MemoryBlockInterface::setDescription(string const& itemName, string const& newDescription)
{
    QSharedPointer<MemoryBlockBase> editedItem = getBlock(itemName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getBaseAddressValue()
//-----------------------------------------------------------------------------
std::string MemoryBlockInterface::getBaseAddressValue(std::string const& blockName, int const& baseNumber) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return parseExpressionToBaseNumber(selectedBlock->getBaseAddress(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getBaseAddressFormattedExpression()
//-----------------------------------------------------------------------------
std::string MemoryBlockInterface::getBaseAddressFormattedExpression(std::string const& blockName) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return formattedValueFor(selectedBlock->getBaseAddress()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getBaseAddressExpression()
//-----------------------------------------------------------------------------
std::string MemoryBlockInterface::getBaseAddressExpression(std::string const& blockName) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getBaseAddress().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::setBaseAddress()
//-----------------------------------------------------------------------------
bool MemoryBlockInterface::setBaseAddress(std::string const& blockName, std::string const& newBaseAdress) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }
    
    selectedBlock->setBaseAddress(QString::fromStdString(newBaseAdress));
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getIsPresentValue()
//-----------------------------------------------------------------------------
std::string MemoryBlockInterface::getIsPresentValue(std::string const& blockName, int const& baseNumber) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return parseExpressionToBaseNumber(selectedBlock->getIsPresent(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: AddressBlockInterface::getIsPresentFormattedExpression()
//-----------------------------------------------------------------------------
std::string MemoryBlockInterface::getIsPresentFormattedExpression(std::string const& blockName) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return formattedValueFor(selectedBlock->getIsPresent()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getIsPresentExpression()
//-----------------------------------------------------------------------------
std::string MemoryBlockInterface::getIsPresentExpression(std::string const& blockName) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(blockName);
    if (selectedBlock)
    {
        return selectedBlock->getIsPresent().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::setIsPresent()
//-----------------------------------------------------------------------------
bool MemoryBlockInterface::setIsPresent(std::string const& blockName, std::string const& newIsPresent) const
{
    QSharedPointer<MemoryBlockBase> selectedBlock = getBlock(blockName);
    if (!selectedBlock)
    {
        return false;
    }

    selectedBlock->setIsPresent(QString::fromStdString(newIsPresent));
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int MemoryBlockInterface::getAllReferencesToIdInItem(const string& itemName, string const&  valueID) const
{
    QSharedPointer<MemoryBlockBase> selectedItem = getBlock(itemName);

    QString idString = QString::fromStdString(valueID);

    int referencesInBaseAddress = selectedItem->getBaseAddress().count(idString);
    int referencesInIsPresent = selectedItem->getIsPresent().count(idString);

    int totalReferencesToParameter = referencesInBaseAddress + referencesInIsPresent;

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::setAddressUnitBits()
//-----------------------------------------------------------------------------
void MemoryBlockInterface::setAddressUnitBits(std::string const& newAddressUnitbits)
{
    addressUnitBits_ = QString::fromStdString(newAddressUnitbits);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getNewBlockBaseAddress()
//-----------------------------------------------------------------------------
QString MemoryBlockInterface::getNewBlockBaseAddress() const
{
    quint64 lastBaseAddress = 0;
    quint64 lastRange = 0;

    for (int itemIndex = 0; itemIndex < itemCount(); ++itemIndex)
    {
        std::string itemName = getIndexedItemName(itemIndex);
        QString calculatedExpression = QString::fromStdString(getBaseAddressValue(itemName, 10));
        quint64 blockBaseAddress = calculatedExpression.toULongLong();

        if (blockBaseAddress > lastBaseAddress || (itemIndex == itemCount() - 1 && lastBaseAddress == 0))
        {
            QSharedPointer<AddressBlock> addressBlock = blockData_->at(itemIndex).dynamicCast<AddressBlock>();
            QSharedPointer<SubSpaceMap> subspaceMap = blockData_->at(itemIndex).dynamicCast<SubSpaceMap>();
            if (addressBlock || subspaceMap)
            {
                lastBaseAddress = blockBaseAddress;
                lastRange = QString::fromStdString(getRangeValue(itemName, 10)).toULongLong();
            }
        }
    }


    // convert the address to hexadecimal form
    QString newBase = QString::number(lastBaseAddress + lastRange, 16);
    newBase = newBase.toUpper();
    newBase.prepend("'h");

    return newBase;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::removeBlock()
//-----------------------------------------------------------------------------
bool MemoryBlockInterface::removeBlock(std::string const& blockName)
{
    QSharedPointer<MemoryBlockBase> removedItem = getBlock(blockName);
    if (!removedItem)
    {
        return false;
    }

    return blockData_->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::copyRows()
//-----------------------------------------------------------------------------
void MemoryBlockInterface::copyRows(std::vector<int> selectedRows)
{
    QList<QSharedPointer<MemoryBlockBase> > copiedBlocks = getCopiedBlocks(selectedRows);

    QVariant memoryBlockVariant;
    memoryBlockVariant.setValue(copiedBlocks);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData(getMimeType(), QByteArray());
    newMimeData->setImageData(memoryBlockVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::pasteRows()
//-----------------------------------------------------------------------------
std::vector<std::string> MemoryBlockInterface::pasteRows()
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
                QSharedPointer<MemoryBlockBase> newBlock = createCopyBlock(copiedItem);
                if (newBlock)
                {
                    blockData_->append(newBlock);
                    pastedItemNames.push_back(newBlock->name().toStdString());
                }
            }
        }
    }

    return pastedItemNames;
}

//-----------------------------------------------------------------------------
// Function: MemoryBlockInterface::getPasteRowCount()
//-----------------------------------------------------------------------------
int MemoryBlockInterface::getPasteRowCount() const
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();
    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<MemoryBlockBase> > >())
        {
            QList<QSharedPointer<MemoryBlockBase> > newBlocks =
                pasteVariant.value<QList<QSharedPointer<MemoryBlockBase> > >();

            return countItems(newBlocks);
        }
    }
    
    return 0;
}
