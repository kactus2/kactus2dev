//-----------------------------------------------------------------------------
// File: MemoryColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 17.10.2012
//
// Description:
// Memory column.
//-----------------------------------------------------------------------------

#include "MemoryColumn.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/layouts/VStaticLayout.h>
#include <common/layouts/VStackedLayout.h>
#include <common/layouts/VCollisionLayout.h>
#include <common/layouts/IVGraphicsLayout.h>

#include <editors/MemoryDesigner/AddressSpaceGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <editors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryConnectionItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: MemoryColumn()
//-----------------------------------------------------------------------------
MemoryColumn::MemoryColumn(QSharedPointer<ColumnDesc> desc, GraphicsColumnLayout* layout, int itemSpacing):
GraphicsColumn(desc, layout, false)
{
    QSharedPointer<IVGraphicsLayout<QGraphicsItem> > newItemLayout (
        new VCollisionLayout<QGraphicsItem>(itemSpacing));

    setItemLayout(newItemLayout);
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::isItemAllowed()
//-----------------------------------------------------------------------------
bool MemoryColumn::isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const
{
    if (allowedItems & ColumnTypes::MEMORY &&
        (item->type() == AddressSpaceGraphicsItem::Type || item->type() == MemoryMapGraphicsItem::Type))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::findGraphicsItemByMemoryItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryColumn::findGraphicsItemByMemoryItem(
    QSharedPointer<MemoryItem const> containedMemoryItem) const
{
    if (containedMemoryItem)
    {
        foreach (QGraphicsItem* item, getItems())
        {
            auto memoryGraphicsItem = dynamic_cast<MainMemoryGraphicsItem*>(item);
            if (memoryGraphicsItem && memoryGraphicsItem->getMemoryItem() == containedMemoryItem && memoryGraphicsItem->isOriginal())
            {
                return memoryGraphicsItem;
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::getGraphicsItemInOrder()
//-----------------------------------------------------------------------------
QMultiMap<qreal, MainMemoryGraphicsItem*> MemoryColumn::getGraphicsItemInOrder() const
{
    QMultiMap<qreal, MainMemoryGraphicsItem*> orderedGraphicsItems;

    for (QGraphicsItem* graphicsItem : getItems())
    {
        auto memoryItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
        if (memoryItem)
        {
            qreal itemScenePositionY = graphicsItem->scenePos().y();
            orderedGraphicsItems.insert(itemScenePositionY, memoryItem);
        }
    }

    return orderedGraphicsItems;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::containsMemoryMapItems()
//-----------------------------------------------------------------------------
bool MemoryColumn::containsMemoryMapItems() const
{
    return name().contains(MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME);
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::changeWidth()
//-----------------------------------------------------------------------------
void MemoryColumn::changeWidth(qreal deltaWidth)
{
    bool columnHasItems = getItems().size() > 0;

    qreal previousXPosition = 0;
    if (columnHasItems)
    {
        previousXPosition = getItems().first()->pos().x();
    }

    qreal previousWidth = getColumnDesc()->getWidth();
    setWidth(previousWidth + deltaWidth);

    foreach (QGraphicsItem* subItem, getItems())
    {
        auto mapItem = dynamic_cast<MemoryMapGraphicsItem*>(subItem);
        if (mapItem)
        {
            mapItem->setX(previousXPosition);
            mapItem->changeWidth(deltaWidth);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::getMaximumNeededChangeInWidth()
//-----------------------------------------------------------------------------
qreal MemoryColumn::getMaximumNeededChangeInWidth() const
{
    qreal maximumWidthChange = 0;
    foreach (QGraphicsItem* subItem, getItems())
    {
        auto mapItem = dynamic_cast<MemoryMapGraphicsItem*>(subItem);
        if (mapItem)
        {
            maximumWidthChange = qMax(maximumWidthChange, mapItem->getMaximumNeededChangeInFieldWidth());
        }
    }

    return maximumWidthChange;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::moveUnconnectedMemoryItems()
//-----------------------------------------------------------------------------
void MemoryColumn::moveUnconnectedMemoryItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems) const
{
    qreal currentPosition = getUnconnectedItemPosition(placedItems);

    foreach (QGraphicsItem* graphicsItem, getItems())
    {
        auto memoryItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
        if (memoryItem && memoryItem->getMemoryConnections().isEmpty())
        {
            memoryItem->setY(currentPosition);

            currentPosition +=
                memoryItem->getHeightWithSubItems() + MemoryDesignerConstants::UNCONNECTED_ITEM_INTERVAL;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::getLastItemLow()
//-----------------------------------------------------------------------------
quint64 MemoryColumn::getUnconnectedItemPosition(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems)
    const
{
    qreal lastItemLowLinePosition = 0;
    foreach (MainMemoryGraphicsItem* graphicsItem, *placedItems)
    {
        qreal extensionHeight = 0;
        
        if (auto extensionItem = graphicsItem->getExtensionItem(); extensionItem)
        {
            extensionHeight = extensionItem->sceneBoundingRect().height();
        }
        if (qreal subItemHeightAddition = graphicsItem->getSubItemHeightAddition(); subItemHeightAddition > extensionHeight)
        {
            extensionHeight = subItemHeightAddition;
        }

        qreal itemLow = graphicsItem->sceneBoundingRect().bottom() + extensionHeight;
        if (itemLow > lastItemLowLinePosition)
        {
            lastItemLowLinePosition = itemLow;
        }
    }

    quint64 positionY = 0;
    if (lastItemLowLinePosition == 0)
    {
        positionY = MemoryDesignerConstants::SPACEITEMINTERVAL;
    }
    else
    {
        positionY = lastItemLowLinePosition + MemoryDesignerConstants::CONNECTED_UNCONNECTED_INTERVAL;
    }

    return positionY;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::memoryMapOverlapsInColumn()
//-----------------------------------------------------------------------------
bool MemoryColumn::memoryMapOverlapsInColumn(MainMemoryGraphicsItem const* memoryGraphicsItem,
    QRectF memoryItemRect,
    int memoryPenWidth,
    QVector<MainMemoryGraphicsItem*> connectedSpaceItems,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps) const
{
    for (auto comparisonMemoryItem : *placedMaps)
    {
        if (comparisonMemoryItem->parentItem() == this && comparisonMemoryItem != memoryGraphicsItem)
        {
            QRectF comparisonRectangle = comparisonMemoryItem->getSceneRectangleWithSubItems();
            int comparisonLineWidth = comparisonMemoryItem->pen().width();

            bool overlap = MemoryDesignerConstants::itemOverlapsAnotherItem(
                memoryItemRect, memoryPenWidth, comparisonRectangle, comparisonLineWidth);

            if (overlap)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::itemOverlapsAnotherPlacedColumnItem()
//-----------------------------------------------------------------------------
bool MemoryColumn::itemOverlapsAnotherPlacedColumnItem(MainMemoryGraphicsItem const* memoryItem,
    QRectF const& itemRectangle,
    int lineWidth,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems) const
{
    for (MainMemoryGraphicsItem * comparisonItem : getGraphicsItemInOrder())
    {
        if (placedItems->contains(comparisonItem) && comparisonItem != memoryItem)
        {
            QRectF comparisonRectangle = comparisonItem->getSceneRectangleWithSubItems();
            if (comparisonItem->hasExtensionItem())
            {
                comparisonRectangle.setHeight(comparisonRectangle.height() + comparisonItem->getExtensionItem()->boundingRect().height());
            }

            int comparisonLineWidth = comparisonItem->pen().width();

            if (MemoryDesignerConstants::itemOverlapsAnotherItem(
                itemRectangle, lineWidth, comparisonRectangle, comparisonLineWidth))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::compressUnconnectedMemoryItems()
//-----------------------------------------------------------------------------
void MemoryColumn::compressUnconnectedMemoryItems(bool compressMemoryItems, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems)
{
    for (auto graphicsItem : getGraphicsItemInOrder())
    {
        if (placedItems->contains(graphicsItem) == false)
        {
            graphicsItem->compressItemAndChildItems(compressMemoryItems);
        }
    }
}
