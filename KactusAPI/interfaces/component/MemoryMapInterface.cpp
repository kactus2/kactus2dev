//-----------------------------------------------------------------------------
// File: MemoryMapInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.05.2020
//
// Description:
// Interface for editing memory maps and remaps.
//-----------------------------------------------------------------------------

#include <MemoryMapInterface.h>

#include <MemoryMapExpressionsGatherer.h>
#include <MemoryRemapExpressionGatherer.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/ModeReference.h>

#include <QMimeData>
#include <QApplication>
#include <QClipboard>

using namespace std;

namespace
{
    std::string const MEMORYMAP_TYPE = "memoryMap";
    std::string const MEMORYREMAP_TYPE = "memoryRemap";

    std::string const DEFAULT_REMAP_STATE = "Default";
    std::string const NO_REMAP_STATE = "No remap state selected.";

    std::string const NO_INTERFACE_BINDING = "No binding";
};

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::MemoryMapInterface()
//-----------------------------------------------------------------------------
MemoryMapInterface::MemoryMapInterface(QSharedPointer<MemoryMapValidator> mapValidator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
component_(),
mapData_(),
validator_(mapValidator),
addressBlockInterface_(0),
subspaceInterface_(0)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setAddressBlockInterface()
//-----------------------------------------------------------------------------
void MemoryMapInterface::setAddressBlockInterface(AddressBlockInterface* blockInterface)
{
    addressBlockInterface_ = blockInterface;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setSubspaceMapInterface()
//-----------------------------------------------------------------------------
void MemoryMapInterface::setSubspaceMapInterface(SubspaceMapInterface* submapInterface)
{
    subspaceInterface_ = submapInterface;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryMapInterface::setMemoryMaps(QSharedPointer<Component> component)
{
    component_ = component;
    mapData_ = component->getMemoryMaps();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> MemoryMapInterface::getMemoryMap(std::string const& itemName) const
{
    QString itemNameQ = QString::fromStdString(itemName);

    for (auto currentMap : *mapData_)
    {
        if (currentMap->name() == itemNameQ)
        {
            return currentMap;
        }
    }

    return QSharedPointer<MemoryMap>();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getMemoryRemap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryRemap> MemoryMapInterface::getMemoryRemap(std::string const& mapName,
    std::string const& itemName) const
{
    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapName);
    if (!containingMap)
    {
        return QSharedPointer<MemoryRemap>();
    }

    for (auto remap : *containingMap->getMemoryRemaps())
    {
        if (remap->name().toStdString() == itemName)
        {
            return remap;
        }
    }

    return QSharedPointer<MemoryRemap>();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getItemIndex()
//-----------------------------------------------------------------------------
int MemoryMapInterface::getItemIndex(string const& itemName) const
{
    for (int i = 0; i < mapData_->size(); ++i)
    {
        if (mapData_->at(i)->name().toStdString() == itemName)
        {
            return i;
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getMemoryRemapIndex()
//-----------------------------------------------------------------------------
int MemoryMapInterface::getMemoryRemapIndex(std::string const& mapName, std::string const& remapName) const
{
    QSharedPointer<MemoryMap> parentMap = getMemoryMap(mapName);
    if (parentMap)
    {
        QSharedPointer<QList<QSharedPointer<MemoryRemap> > > remapList = parentMap->getMemoryRemaps();
        for (int i = 0; i < remapList->size(); ++i)
        {
            if (remapList->at(i)->name().toStdString() == remapName)
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
string MemoryMapInterface::getIndexedItemName(int itemIndex) const
{
    string blockName = "";
    if (itemIndex >= 0 && itemIndex < mapData_->size())
    {
        blockName = mapData_->at(itemIndex)->name().toStdString();
    }

    return blockName;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getIndexedRemapName()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getIndexedRemapName(std::string mapName, int const& itemIndex) const
{
    std::string remapName = "";

    QSharedPointer<MemoryMap> parentMap = getMemoryMap(mapName);
    if (parentMap)
    {
        QSharedPointer<QList<QSharedPointer<MemoryRemap> > > remapList = parentMap->getMemoryRemaps();
        if (itemIndex >= 0 && itemIndex < remapList->size())
        {
            remapName = remapList->at(itemIndex)->name().toStdString();
        }
    }

    return remapName;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::itemCount()
//-----------------------------------------------------------------------------
int MemoryMapInterface::itemCount() const
{
    return mapData_->count();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::remapCount()
//-----------------------------------------------------------------------------
int MemoryMapInterface::remapCount(std::string const& mapName) const
{
    std::string remapName = "";

    QSharedPointer<MemoryMap> parentMap = getMemoryMap(mapName);
    if (parentMap)
    {
        return parentMap->getMemoryRemaps()->count();
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getItemNames()
//-----------------------------------------------------------------------------
vector<string> MemoryMapInterface::getItemNames() const
{
    vector<string> names;
    for (auto map : *mapData_)
    {
        names.push_back(map->name().toStdString());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getRemapNames()
//-----------------------------------------------------------------------------
std::vector<std::string> MemoryMapInterface::getRemapNames(std::string const& mapName) const
{
    vector<string> names;
    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapName);
    if (containingMap)
    {
        for (auto remap : *containingMap->getMemoryRemaps())
        {
            names.push_back(remap->name().toStdString());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setName()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::setName(string const& currentName, string const& newName)
{
    QSharedPointer<MemoryMap> editedItem = getMemoryMap(currentName);
    if (editedItem && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName = getUniqueName(newName, MEMORYMAP_TYPE);

        editedItem->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setRemapName()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::setRemapName(std::string const& mapName, std::string const& currentName,
    std::string const& newName)
{
    QSharedPointer<MemoryRemap> editedItem = getMemoryRemap(mapName, currentName);
    if (editedItem && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName = getUniqueRemapName(mapName, newName, MEMORYREMAP_TYPE);

        editedItem->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setRemapDescription()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getRemapDescription(std::string const& mapName, std::string const& remapName) const
{
    QSharedPointer<MemoryRemap> editedItem = getMemoryRemap(mapName, remapName);
    if (editedItem)
    {
        return editedItem->description().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setRemapDescription()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::setRemapDescription(std::string const& mapName, std::string const& itemName,
    std::string const& newDescription)
{
    QSharedPointer<MemoryRemap> editedItem = getMemoryRemap(mapName, itemName);
    if (!editedItem)
    {
        return false;
    }

    editedItem->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getIsPresentValue()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getIsPresentValue(std::string const& mapName, std::string const& remapName,
    int const& baseNumber) const
{
    QSharedPointer<MemoryMapBase> baseMap;
    if (remapName.empty())
    {
        baseMap = getMemoryMap(mapName);
    }
    else
    {
        baseMap = getMemoryRemap(mapName, remapName);
    }

    if (baseMap)
    {
        return parseExpressionToBaseNumber(baseMap->getIsPresent(), baseNumber).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getIsPresentFormattedExpression()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getIsPresentFormattedExpression(std::string const& mapName,
    std::string const& remapName) const
{
    QSharedPointer<MemoryMapBase> baseMap;
    if (remapName.empty())
    {
        baseMap = getMemoryMap(mapName);
    }
    else
    {
        baseMap = getMemoryRemap(mapName, remapName);
    }

    if (baseMap)
    {
        return formattedValueFor(baseMap->getIsPresent()).toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getIsPresentExpression()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getIsPresentExpression(std::string const& mapName, std::string const& remapName)
    const
{
    QSharedPointer<MemoryMapBase> baseMap;
    if (remapName.empty())
    {
        baseMap = getMemoryMap(mapName);
    }
    else
    {
        baseMap = getMemoryRemap(mapName, remapName);
    }

    if (baseMap)
    {
        return baseMap->getIsPresent().toStdString();
    }

    return string("");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setIsPresent()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::setIsPresent(std::string const& mapName, std::string const& newIsPresent,
    std::string const& remapName)
{
    QSharedPointer<MemoryMapBase> baseMap;
    if (remapName.empty())
    {
        baseMap = getMemoryMap(mapName);
    }
    else
    {
        baseMap = getMemoryRemap(mapName, remapName);
    }

    if (!baseMap)
    {
        return false;
    }
    
    baseMap->setIsPresent(QString::fromStdString(newIsPresent));
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getRemapState()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getRemapState(std::string const& mapName, std::string const& remapName) const
{
    if (remapName.empty())
    {
        return DEFAULT_REMAP_STATE;
    }

    QSharedPointer<MemoryRemap> selectedItem = getMemoryRemap(mapName, remapName);
    if (!selectedItem->getRemapState().isEmpty())
    {
        return selectedItem->getRemapState().toStdString();
    }
    else
    {
        return NO_REMAP_STATE;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setRemapState()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::setRemapState(std::string const& mapName, std::string const& remapName,
    std::string const& newRemapState)
{
    QSharedPointer<MemoryRemap> selectedItem = getMemoryRemap(mapName, remapName);
    if (!selectedItem)
    {
        return false;
    }

    selectedItem->setRemapState(QString::fromStdString(newRemapState));
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getAddressUnitBits()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getAddressUnitBits(std::string const& mapName) const
{
    QSharedPointer<MemoryMap> map = getMemoryMap(mapName);
    if (map)
    {
        return map->getAddressUnitBits().toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setAddressUnitBits()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::setAddressUnitBits(std::string const& mapName, std::string const& newAddressUnitBits)
const
{
    QSharedPointer<MemoryMap> map = getMemoryMap(mapName);
    if (!map)
    {
        return false;
    }

    map->setAddressUnitBits(QString::fromStdString(newAddressUnitBits));
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getInterfaceBinding()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getInterfaceBinding(std::string const& mapName) const
{
    QSharedPointer<MemoryMap> map = getMemoryMap(mapName);
    if (!map)
    {
        return "";
    }

    QStringList interfaceNames = component_->getTargetInterfaces(map->name());
    if (interfaceNames.isEmpty())
    {
        return NO_INTERFACE_BINDING;
    }
    else
    {
        return interfaceNames.join(", ").toStdString();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getAssociatedSlaveInterfaces()
//-----------------------------------------------------------------------------
std::vector<std::string> MemoryMapInterface::getAssociatedSlaveInterfaces(std::string const& mapName) const
{
    std::vector<std::string> associatedSlaves;

    for (auto const& slave : component_->getTargetInterfaces(QString::fromStdString(mapName)))
    {
        associatedSlaves.push_back(slave.toStdString());
    }

    return associatedSlaves;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int MemoryMapInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const&  valueID) const
{
    QSharedPointer<MemoryMap> selectedItem = getMemoryMap(itemName);

    QString idString = QString::fromStdString(valueID);

    int referencesInIsPresent = selectedItem->getIsPresent().count(idString);
    int totalReferencesToParameter = referencesInIsPresent;

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getReferencedParameters()
//-----------------------------------------------------------------------------
std::vector<std::string> MemoryMapInterface::getMapExpressions(std::string const& mapName,
    std::string const& remapName) const
{
    QStringList mapExpressions;

    if (remapName.empty())
    {
        QSharedPointer<MemoryMap> selectedMap = getMemoryMap(mapName);
        MemoryMapExpressionGatherer* expressionGatherer = new MemoryMapExpressionGatherer();
        mapExpressions = expressionGatherer->getExpressions(selectedMap);
    }
    else
    {
        QSharedPointer<MemoryRemap> selectedMap = getMemoryRemap(mapName, remapName);
        MemoryRemapExpressionGatherer* expressionGatherer = new MemoryRemapExpressionGatherer();
        mapExpressions = expressionGatherer->getExpressions(selectedMap);
    }

    std::vector<std::string> expressionList;

    for (auto const& expression : mapExpressions)
    {
        expressionList.push_back(expression.toStdString());
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getAllReferencesToIdInRemapItem()
//-----------------------------------------------------------------------------
int MemoryMapInterface::getAllReferencesToIdInRemapItem(std::string const& mapName, std::string const& remapName,
    std::string const& valueID) const
{
    QSharedPointer<MemoryRemap> selectedItem = getMemoryRemap(mapName, remapName);

    QString idString = QString::fromStdString(valueID);

    int referencesInIsPresent = selectedItem->getIsPresent().count(idString);
    int totalReferencesToParameter = referencesInIsPresent;

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::validateItems()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::validateItems() const
{
    for (auto currentItem : *mapData_)
    {
        if (!validator_->validate(currentItem))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::itemHasValidName(std::string const& itemName) const
{
    QSharedPointer<MemoryMap> selectedItem = getMemoryMap(itemName);
    if (selectedItem)
    {
        return validator_->hasValidName(selectedItem);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::remapHasValidName()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::remapHasValidName(std::string const& mapName, std::string const& itemName) const
{
    QSharedPointer<MemoryRemap> selectedItem = getMemoryRemap(mapName, itemName);
    if (selectedItem)
    {
        return validator_->hasValidName(selectedItem);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::remapHasValidRemapState()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::remapHasValidRemapState(std::string const& mapName, std::string const& itemName) const
{
    QSharedPointer<MemoryRemap> remap = getMemoryRemap(mapName, itemName);
    if (remap)
    {
        return !validator_->remapStateIsNotValid(remap);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::itemHasValidIsPresent()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::itemHasValidIsPresent(std::string const& mapName, std::string const& remapName) const
{
    QSharedPointer<MemoryMapBase> selectedItem;
    if (remapName.empty())
    {
        selectedItem = getMemoryMap(mapName);
    }
    else
    {
        selectedItem = getMemoryRemap(mapName, remapName);
    }

    if (selectedItem)
    {
        return validator_->hasValidIsPresent(selectedItem);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::memoryMapHasValidAddressUnitBits()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::memoryMapHasValidAddressUnitBits(std::string const& mapName) const
{
    QSharedPointer<MemoryMap> map = getMemoryMap(mapName);
    if (map)
    {
        return validator_->hasValidAddressUnitBits(map);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::addBlock()
//-----------------------------------------------------------------------------
void MemoryMapInterface::addMemoryMap(int const& row, std::string const& newMapName)
{
    QSharedPointer<MemoryMap> newMap(new MemoryMap());
    newMap->setName(getUniqueName(newMapName, MEMORYMAP_TYPE));

    mapData_->insert(row, newMap);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::addMemoryRemap()
//-----------------------------------------------------------------------------
void MemoryMapInterface::addMemoryRemap(std::string const& mapName, std::string const& newRemapName)
{
    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapName);
    if (!containingMap)
    {
        return;
    }

    QSharedPointer<MemoryRemap> newRemap(new MemoryRemap());
//     newRemap->setName(getUniqueName(newRemapName, MEMORYREMAP_TYPE));
    newRemap->setName(getUniqueRemapName(mapName, newRemapName, MEMORYREMAP_TYPE));
    for (auto mapBlock : *containingMap->getMemoryBlocks())
    {
        QSharedPointer<MemoryBlockBase> remapBlock = mapBlock->clone();
        newRemap->getMemoryBlocks()->append(remapBlock);
    }

    containingMap->getMemoryRemaps()->append(newRemap);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::removeMap()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::removeMap(std::string const& mapName)
{
    QSharedPointer<MemoryMap> removedItem = getMemoryMap(mapName);
    if (!removedItem)
    {
        return false;
    }

    return mapData_->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::removeRemap()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::removeRemap(std::string const& mapName, std::string remapName)
{
    QSharedPointer<MemoryMap> containingMap = getMemoryMap(mapName);
    if (!containingMap)
    {
        return false;
    }

    QSharedPointer<MemoryRemap> removedItem = getMemoryRemap(mapName, remapName);
    if (!removedItem)
    {
        return false;
    }

    return containingMap->getMemoryRemaps()->removeOne(removedItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::copyRows()
//-----------------------------------------------------------------------------
void MemoryMapInterface::copyRows(std::vector<std::string> const& selectedRows)
{
    QList<QSharedPointer<MemoryMapBase> > copiedMemories;
    QList<QSharedPointer<MemoryRemap> > copiedRemaps;
    for (auto const& row : selectedRows)
    {
        QStringList rowData = QString::fromStdString(row).split("_");
        int mapIndex = rowData.first().toInt();

        std::string mapName = getIndexedItemName(mapIndex);

        if (rowData.size() == 2)
        {
            int remapIndex = rowData.last().toInt();
            std::string remapName = getIndexedRemapName(mapName, remapIndex);

            QSharedPointer<MemoryRemap> remap = getMemoryRemap(mapName, remapName);
            if (remap && !copiedRemaps.contains(remap))
            {
                copiedRemaps.append(remap);
            }
        }
        else
        {
            QSharedPointer<MemoryMap> memoryMap = getMemoryMap(mapName);
            if (memoryMap && !copiedMemories.contains(memoryMap))
            {
                copiedMemories.append(memoryMap);
            }
        }
    }

    for (auto remap : copiedRemaps)
    {
        copiedMemories.append(remap);
    }

    QVariant memoryBlockVariant;
    memoryBlockVariant.setValue(copiedMemories);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:memoryMap", QByteArray());
    newMimeData->setImageData(memoryBlockVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::pasteMemoryMaps()
//-----------------------------------------------------------------------------
std::vector<std::string> MemoryMapInterface::pasteMemoryMaps()
{
    std::vector<std::string> pastedItemNames;

    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<MemoryMapBase> > >())
        {
            QList<QSharedPointer<MemoryMapBase> > copiedMapList =
                pasteVariant.value<QList<QSharedPointer<MemoryMapBase>>>();

            foreach(QSharedPointer<MemoryMapBase> copiedMap, copiedMapList)
            {
                QSharedPointer<MemoryMap> copyMap = copiedMap.dynamicCast<MemoryMap>();
                if (copyMap)
                {
                    pastedItemNames.push_back(pasteMemoryMap(copyMap).toStdString());
                }
            }
        }
    }

    return pastedItemNames;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::pasteMemoryRemaps()
//-----------------------------------------------------------------------------
std::vector<std::string> MemoryMapInterface::pasteMemoryRemaps(std::string const& memoryMapName)
{
    std::vector<std::string> pastedItemNames;

    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (!memoryMapName.empty() && pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<MemoryMapBase> > >())
        {
            QList<QSharedPointer<MemoryMapBase> > copiedMapList =
                pasteVariant.value<QList<QSharedPointer<MemoryMapBase>>>();

            for (auto const& copiedMap : copiedMapList)
            {
                QSharedPointer<MemoryRemap> copyRemap = copiedMap.dynamicCast<MemoryRemap>();
                if (copyRemap)
                {
                    QString newRemapName = pasteMemoryRemap(copyRemap, memoryMapName);
                    if (!newRemapName.isEmpty())
                    {
                        pastedItemNames.push_back(newRemapName.toStdString());
                    }
                }
            }
        }
    }

    return pastedItemNames;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::pasteMemoryMap()
//-----------------------------------------------------------------------------
QString MemoryMapInterface::pasteMemoryMap(QSharedPointer<MemoryMap> mapCopy)
{
    QSharedPointer<MemoryMap> newMap(new MemoryMap(*mapCopy.data()));
    newMap->setName(getUniqueName(newMap->name().toStdString(), MEMORYMAP_TYPE));
    mapData_->append(newMap);
    
    return newMap->name();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::pasteMemoryRemap()
//-----------------------------------------------------------------------------
QString MemoryMapInterface::pasteMemoryRemap(QSharedPointer<MemoryRemap> remapCopy,
    std::string const& parentMapName)
{
    if (!parentMapName.empty())
    {
        QSharedPointer<MemoryMap> parentMap = getMemoryMap(parentMapName);
        if (parentMap)
        {
            QSharedPointer<MemoryRemap> newRemap(new MemoryRemap(*remapCopy.data()));
            newRemap->setName(getUniqueRemapName(parentMapName, newRemap->name().toStdString(), MEMORYREMAP_TYPE));
            parentMap->getMemoryRemaps()->append(newRemap);

            return newRemap->name();
        }
    }

    return QString("");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getAddressBlockInterface()
//-----------------------------------------------------------------------------
AddressBlockInterface* MemoryMapInterface::getAddressBlockInterface() const
{
    return addressBlockInterface_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getSubspaceMapInterface()
//-----------------------------------------------------------------------------
SubspaceMapInterface* MemoryMapInterface::getSubspaceMapInterface() const
{
    return subspaceInterface_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getMapPointer()
//-----------------------------------------------------------------------------
MemoryMap* MemoryMapInterface::getMapPointer(std::string const& mapName) const
{
    MemoryMap* mapPointer = nullptr;

    QSharedPointer<MemoryMap> selectedMap = getMemoryMap(mapName);
    if (selectedMap)
    {
        mapPointer = selectedMap.data();
    }

    return mapPointer;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getRemapPointer()
//-----------------------------------------------------------------------------
MemoryRemap* MemoryMapInterface::getRemapPointer(std::string const& mapName, std::string const& remapName) const
{
    MemoryRemap* reMapPointer = nullptr;

    QSharedPointer<MemoryRemap> selectedRemap = getMemoryRemap(mapName, remapName);
    if (selectedRemap)
    {
        reMapPointer = selectedRemap.data();
    }

    return reMapPointer;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getRemapModeReferenceCount()
//-----------------------------------------------------------------------------
int MemoryMapInterface::getRemapModeReferenceCount(std::string const& mapName, std::string const& remapName) const
{
    auto remap = getMemoryRemap(mapName, remapName);
    if (!remap)
    {
        return 0;
    }

    return remap->getModeReferences()->size();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getRemapModeReferences()
//-----------------------------------------------------------------------------
std::vector<std::pair<unsigned int, std::string> > MemoryMapInterface::getRemapModeReferences(std::string const& mapName, std::string const& remapName) const
{
    std::vector<std::pair<unsigned int, std::string> > modeRefs;

    if (auto remap = getMemoryRemap(mapName, remapName))
    {
        auto modeReferences = remap->getModeReferences();

        std::transform(modeReferences->cbegin(), modeReferences->cend(), std::back_inserter(modeRefs),
            [](auto modeReference)
            {
                return std::make_pair(modeReference->getPriority(), modeReference->getReference().toStdString());
            });
    }

    return modeRefs;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setRemapModeReferences()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::setRemapModeReferences(std::string const& mapName, std::string const& remapName, 
    std::vector<std::pair<unsigned int, std::string> > const& newModeRefs)
{
    if (auto remap = getMemoryRemap(mapName, remapName))
    {
        QSharedPointer<QList<QSharedPointer<ModeReference> > > createdModeRefs(
            new QList<QSharedPointer<ModeReference> >());

        for (auto const& [priority, reference] : newModeRefs)
        {
            QSharedPointer<ModeReference> createdModeRef(new ModeReference());
            createdModeRef->setReference(QString::fromStdString(reference));
            createdModeRef->setPriority(priority);
            createdModeRefs->append(createdModeRef);
        }

        remap->setModeReferences(createdModeRefs);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::addRemapModeReference()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::addRemapModeReference(std::string const& mapName, std::string const& remapName)
{
    auto remap = getMemoryRemap(mapName, remapName);
    if (!remap)
    {
        return false;
    }

    QSharedPointer<ModeReference> newModeRef(new ModeReference());
    remap->getModeReferences()->append(newModeRef);

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::removeRemapModeReference()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::removeRemapModeReference(std::string const& mapName, std::string const& remapName, int modeReferenceIndex)
{
    auto remap = getMemoryRemap(mapName, remapName);
    if (!remap || modeReferenceIndex < 0 || modeReferenceIndex >= getRemapModeReferenceCount(mapName, remapName))
    {
        return false;
    }

    remap->getModeReferences()->removeAt(modeReferenceIndex);
    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getModeReferenceInterface()
//-----------------------------------------------------------------------------
ModeReferenceInterface* MemoryMapInterface::getModeReferenceInterface() const
{
    return modeReferenceInterface_;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setModeReferenceInterface()
//-----------------------------------------------------------------------------
void MemoryMapInterface::setModeReferenceInterface(ModeReferenceInterface* modeRefInterface)
{
    modeReferenceInterface_ = modeRefInterface;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getRemapModeReferencesExcludingRemap()
//-----------------------------------------------------------------------------
std::vector<std::pair<unsigned int, std::string> > MemoryMapInterface::getRemapModeReferencesExcludingRemap(
    std::string const& mapName, std::string const& remapName) const
{
    std::vector<std::pair<unsigned int, std::string> > allModeRefs;

    if (auto selectedMap = getMemoryMap(mapName))
    {
        QString remapNameQ = QString::fromStdString(remapName);
        for (auto const& remap : *selectedMap->getMemoryRemaps())
        {
            // Get all other mode references, excluding mode references in given remap.
            if (remap->name() != remapNameQ)
            {
                std::transform(remap->getModeReferences()->cbegin(), remap->getModeReferences()->cend(), 
                    std::back_inserter(allModeRefs),
                    [](auto modeRef)
                    {
                        return std::make_pair(modeRef->getPriority(), modeRef->getReference().toStdString());
                    });
            }
        }
    }

    return allModeRefs;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> MemoryMapInterface::getItem(std::string const& itemName) const
{
    return getMemoryMap(itemName);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getUniqueRemapName()
//-----------------------------------------------------------------------------
QString MemoryMapInterface::getUniqueRemapName(std::string const& mapName, std::string const& newName,
    std::string const& itemTypeName) const
{
    QString referenceRemapName = QString::fromStdString(newName);
    if (referenceRemapName.isEmpty())
    {
        referenceRemapName = QString::fromStdString(itemTypeName);
    }

    QString newRemapName = referenceRemapName;

    QString format = "$itemName$_$itemNumber$";
    int runningNumber = 0;
    while (!remapNameIsUnique(mapName, newRemapName))
    {
        newRemapName = format;
        newRemapName.replace("$itemName$", referenceRemapName);
        newRemapName.replace("$itemNumber$", QString::number(runningNumber));

        runningNumber++;
    }

    return newRemapName;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::remapNameIsUnique()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::remapNameIsUnique(std::string const& mapName, QString const& remapName) const
{
    for (auto const& containedName : getRemapNames(mapName))
    {
        QString convertedName = QString::fromStdString(containedName);
        if (convertedName == remapName)
        {
            return false;
        }
    }

    return true;
}
