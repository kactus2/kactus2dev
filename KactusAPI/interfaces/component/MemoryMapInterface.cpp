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
#include <MemoryMapBaseInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/ModeReference.h>

#include <QMimeData>
#include <QApplication>
#include <QClipboard>
#include "ListHelper.h"

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
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter) :
    MemoryMapBaseInterface(expressionParser, expressionFormatter),
    component_(),
    mapData_(),
    validator_(mapValidator),
    addressBlockInterface_(0),
    subspaceInterface_(0)
{

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
// Function: MemoryMapInterface::getItemIndex()
//-----------------------------------------------------------------------------
int MemoryMapInterface::getItemIndex(std::string const& itemName) const
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
// Function: MemoryMapInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getIndexedItemName(int itemIndex) const
{
    std::string blockName = "";
    if (itemIndex >= 0 && itemIndex < mapData_->size())
    {
        blockName = mapData_->at(itemIndex)->name().toStdString();
    }

    return blockName;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::itemCount()
//-----------------------------------------------------------------------------
int MemoryMapInterface::itemCount() const
{
    return mapData_->count();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> MemoryMapInterface::getItemNames() const
{
    std::vector<std::string> names;
    for (auto map : *mapData_)
    {
        names.push_back(map->name().toStdString());
    }


    return ListHelper::listNames(mapData_);
    //return names;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::setName()
//-----------------------------------------------------------------------------
bool MemoryMapInterface::setName(std::string const& currentName, std::string const& newName)
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

    return std::string();
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

    return std::string();
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

    return std::string();
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
// Function: MemoryMapInterface::getAddressUnitBits()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getAddressUnitBitsExpression(std::string const& mapName) const
{
    QSharedPointer<MemoryMap> map = getMemoryMap(mapName);
    if (map)
    {
        return map->getAddressUnitBits(false).toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getAddressUnitBitsValue()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getAddressUnitBitsValue(std::string const& mapName) const
{
    if (auto map = getMemoryMap(mapName))
    {
        return parseExpressionToBaseNumber(map->getAddressUnitBits(false), 0).toStdString();
    }

    return std::string();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapInterface::getAddressUnitBitsFormattedExpression()
//-----------------------------------------------------------------------------
std::string MemoryMapInterface::getAddressUnitBitsFormattedExpression(std::string const& mapName) const
{
    if (auto map = getMemoryMap(mapName))
    {
        return formattedValueFor(map->getAddressUnitBits(false)).toStdString();
    }

    return std::string();
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
int MemoryMapInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const& valueID) const
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
// Function: MemoryMapInterface::getItem()
//-----------------------------------------------------------------------------
QSharedPointer<NameGroup> MemoryMapInterface::getItem(std::string const& itemName) const
{
    return getMemoryMap(itemName);
}
