//-----------------------------------------------------------------------------
// File: MemoryMapGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.08.2016
//
// Description:
// Graphics item for visualizing a memory map in the memory designer.
//-----------------------------------------------------------------------------

#include "MemoryMapGraphicsItem.h"

#include <common/KactusColors.h>

#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/ConnectivityComponent.h>
#include <designEditors/MemoryDesigner/AddressBlockGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>

#include <QBrush>
#include <QFont>
#include <QPen>

namespace MemoryMapItemConstants
{
    const int MINIMUMSUBITEMHEIGHT = 2 * MemoryDesignerConstants::RANGEINTERVAL;
};

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::MemoryMapGraphicsItem()
//-----------------------------------------------------------------------------
MemoryMapGraphicsItem::MemoryMapGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
    QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent):
MainMemoryGraphicsItem(
    memoryItem, containingInstance->getName(), MemoryDesignerConstants::ADDRESSBLOCK_TYPE, parent),
addressUnitBits_(memoryItem->getAUB())
{
    quint64 baseAddress = getMemoryMapStart(memoryItem);
    quint64 lastAddress = getMemoryMapEnd(memoryItem);

    QBrush memoryMapBrush(KactusColors::MEM_MAP_COLOR);
    setBrush(memoryMapBrush);

    quint64 memoryHeight = (lastAddress - baseAddress + 1);
    int memoryWidth = 500;

    setGraphicsRectangle(memoryWidth, memoryHeight);
    setupToolTip("memory map", baseAddress, lastAddress);
    setLabelPositions();
    
    qreal blockXPosition = boundingRect().width() / 8;
    setupSubItems(blockXPosition);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::~MemoryMapGraphicsItem()
//-----------------------------------------------------------------------------
MemoryMapGraphicsItem::~MemoryMapGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::getMemoryMapStart()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphicsItem::getMemoryMapStart(QSharedPointer<MemoryItem> memoryItem) const
{
    quint64 baseAddress = 0;
    if (memoryItem->getChildItems().size() > 0)
    {
        bool firstBlock = true;

        foreach (QSharedPointer<MemoryItem> blockItem, memoryItem->getChildItems())
        {
            if (blockItem->getType().compare(MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
            {
                if (firstBlock)
                {
                    baseAddress = blockItem->getAddress().toULongLong();
                    firstBlock = false;
                }
                else
                {
                    quint64 childAddress = blockItem->getAddress().toULongLong();
                    if (childAddress < baseAddress)
                    {
                        baseAddress = childAddress;
                    }
                }
            }
        }
    }

    return baseAddress;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::getMemoryMapEnd()
//-----------------------------------------------------------------------------
quint64 MemoryMapGraphicsItem::getMemoryMapEnd(QSharedPointer<MemoryItem> memoryItem) const
{
    quint64 lastAddress = 0;

    foreach (QSharedPointer<MemoryItem> blockItem, memoryItem->getChildItems())
    {
        if (blockItem->getType().compare(MemoryDesignerConstants::ADDRESSBLOCK_TYPE, Qt::CaseInsensitive) == 0)
        {
            quint64 childBaseAddress = blockItem->getAddress().toULongLong();
            quint64 childRange = blockItem->getRange().toULongLong();

            quint64 blockEndPoint = childBaseAddress + childRange - 1;
            if (blockEndPoint > lastAddress)
            {
                lastAddress = blockEndPoint;
            }
        }
    }

    return lastAddress;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::setLabelPositions()
{
    QPointF instanceLabelPosition = boundingRect().topRight();
    getInstanceNameLabel()->setPos(instanceLabelPosition);

    qreal labelYPosition = getInstanceNameLabel()->boundingRect().height() - GridSize / 2;
    getNameLabel()->setPos(instanceLabelPosition.x(), instanceLabelPosition.y() + labelYPosition);

    labelYPosition = labelYPosition + getNameLabel()->boundingRect().height() - GridSize / 2;
    getAUBLabel()->setPos(instanceLabelPosition.x(), instanceLabelPosition.y() + labelYPosition);

    qreal rangeEndLabelYPosition = boundingRect().bottom() - getRangeEndLabel()->boundingRect().height();
    getRangeStartLabel()->setPos(boundingRect().topLeft());
    getRangeEndLabel()->setPos(boundingRect().left(), rangeEndLabelYPosition);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::createNewSubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* MemoryMapGraphicsItem::createNewSubItem(QSharedPointer<MemoryItem> subMemoryItem,
    bool isEmpty)
{
    return new AddressBlockGraphicsItem(subMemoryItem, isEmpty, this);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::createEmptySubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* MemoryMapGraphicsItem::createEmptySubItem(quint64 beginAddress, quint64 rangeEnd)
{
    QString emptyBlockBaseAddress = QString::number(beginAddress);
    QString emptyBlockRange = QString::number(rangeEnd - beginAddress + 1);

    QSharedPointer<MemoryItem> emptyBlockItem
        (new MemoryItem("Reserved", MemoryDesignerConstants::ADDRESSBLOCK_TYPE));
    emptyBlockItem->setAddress(emptyBlockBaseAddress);
    emptyBlockItem->setRange(emptyBlockRange);
    emptyBlockItem->setAUB(addressUnitBits_);

    return createNewSubItem(emptyBlockItem, true);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::condenseItemAndChildItems()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::condenseItemAndChildItems()
{
    if (!isCompressed() && getMemoryConnections().isEmpty())
    {
        quint64 memoryMapNewHeight = 0;

        int subItemHeight = getMinimumHeightForSubItems();

        memoryMapNewHeight = condenseChildItems(subItemHeight);

        if (memoryMapNewHeight > 0)
        {
            condense(memoryMapNewHeight);

            setCompressed(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::compressMapItem()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::compressMapItem()
{
    if (!isCompressed())
    {
        quint64 newMapHeight = getCompressedHeight(getMinimumHeightForSubItems(), this);

        if (newMapHeight > 0)
        {
            condense(newMapHeight);

            setCompressed(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::hasExtensionItem()
//-----------------------------------------------------------------------------
bool MemoryMapGraphicsItem::hasExtensionItem() const
{
    foreach (QGraphicsItem* childItem, childItems())
    {
        MemoryExtensionGraphicsItem* extensionItem = dynamic_cast<MemoryExtensionGraphicsItem*>(childItem);
        if (extensionItem)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::getMinimumRequiredHeight()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphicsItem::getMinimumRequiredHeight(quint64 connectionBaseAddress, quint64 connectionEndAddress)
    const
{
    int subItemHeight = getMinimumHeightForSubItems();

    return SubMemoryLayout::getMinimumRequiredHeight(subItemHeight, connectionBaseAddress, connectionEndAddress);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::getMinimumHeightForSubItems()
//-----------------------------------------------------------------------------
int MemoryMapGraphicsItem::getMinimumHeightForSubItems() const
{
    int blockHeight = MemoryMapItemConstants::MINIMUMSUBITEMHEIGHT;
    if (getSubMemoryItems().size() == 1)
    {
        MemoryDesignerChildGraphicsItem* blockItem = getSubMemoryItems().first();
        SubMemoryLayout* blockLayout = dynamic_cast<SubMemoryLayout*>(blockItem);
        if (blockLayout && blockLayout->getSubMemoryItems().size() == 1)
        {
            blockHeight = blockHeight * 1.5;
        }
    }

    return blockHeight;
}
