//-----------------------------------------------------------------------------
// File: MemoryColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
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

#include <designEditors/MemoryDesigner/AddressSpaceGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>

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
// Function: ~MemoryColumn()
//-----------------------------------------------------------------------------
MemoryColumn::~MemoryColumn()
{

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
MainMemoryGraphicsItem* MemoryColumn::findGraphicsItemByMemoryItem(QSharedPointer<MemoryItem> containedMemoryItem)
    const
{
    if (containedMemoryItem)
    {
        foreach (QGraphicsItem* item, getItems())
        {
            MainMemoryGraphicsItem* memoryGraphicsItem = dynamic_cast<MainMemoryGraphicsItem*>(item);
            if (memoryGraphicsItem && memoryGraphicsItem->getMemoryItem() == containedMemoryItem)
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
QMap<qreal, MainMemoryGraphicsItem*> MemoryColumn::getGraphicsItemInOrder() const
{
    QMap<qreal, MainMemoryGraphicsItem*> orderedGraphicsItems;

    foreach (QGraphicsItem* graphicsItem, getItems())
    {
        MainMemoryGraphicsItem* memoryItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
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

    int previousXPosition = 0;
    if (columnHasItems)
    {
        previousXPosition = getItems().first()->pos().x();
    }

    int previousWidth = getColumnDesc()->getWidth();
    setWidth(previousWidth + deltaWidth);

    foreach (QGraphicsItem* subItem, getItems())
    {
        MemoryMapGraphicsItem* mapItem = dynamic_cast<MemoryMapGraphicsItem*>(subItem);
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
        MemoryMapGraphicsItem* mapItem = dynamic_cast<MemoryMapGraphicsItem*>(subItem);
        if (mapItem)
        {
            maximumWidthChange = qMax(maximumWidthChange, mapItem->getMaximumNeededChangeInFieldWidth());
        }
    }

    return maximumWidthChange;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::compressGraphicsItems()
//-----------------------------------------------------------------------------
void MemoryColumn::compressGraphicsItems(bool condenseMemoryItems, int& spaceYPlacement, MemoryColumn* spaceColumn,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnectionItems)
{
    int yTransfer = 0;
    quint64 spaceItemLowAfter = 0;

    QVector<MainMemoryGraphicsItem*> movedSpaceItems;

    foreach (QGraphicsItem* graphicsItem, getGraphicsItemInOrder())
    {
        MainMemoryGraphicsItem* memoryItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
        if (memoryItem && memoryItem->isVisible())
        {
            qreal memoryItemLowBefore = memoryItem->getLowestPointOfConnectedItems();

            if (condenseMemoryItems && !memoryItem->isCompressed())
            {
                memoryItem->condenseItemAndChildItems(movedConnectionItems);
            }

            extendMemoryItem(memoryItem, spaceYPlacement);

            if (condenseMemoryItems)
            {
                AddressSpaceGraphicsItem* spaceItem = dynamic_cast<AddressSpaceGraphicsItem*>(memoryItem);
                if (spaceItem && placedSpaceItems->contains(memoryItem) && this == spaceColumn &&
                    !movedSpaceItems.contains(spaceItem))
                {
                    quint64 spaceItemTop = spaceItem->sceneBoundingRect().top() + spaceItem->pen().width();
                    if (spaceItemTop != spaceItemLowAfter)
                    {
                        qint64 spaceInterval = (spaceItemTop + yTransfer) - spaceItemLowAfter;

                        if (spaceInterval < MemoryDesignerConstants::SPACEITEMINTERVAL)
                        {
                            quint64 yMovementAddition = MemoryDesignerConstants::SPACEITEMINTERVAL -
                                ((spaceItemTop + yTransfer) - spaceItemLowAfter);
                            yTransfer += yMovementAddition;
                        }

                        if (!movedSpaceItems.contains(memoryItem))
                        {
                            spaceItem->moveItemAndConnectedItems(yTransfer);

                            foreach (MainMemoryGraphicsItem* connectedSpace,
                                spaceItem->getAllConnectedSpaceItems())
                            {
                                movedSpaceItems.append(connectedSpace);
                            }

                            movedSpaceItems.append(spaceItem);
                        }
                    }

                    spaceItemLowAfter = spaceItem->getLowestPointOfConnectedItems();

                    yTransfer = spaceItemLowAfter - memoryItemLowBefore;

                    spaceYPlacement = spaceItemLowAfter + MemoryDesignerConstants::SPACEITEMINTERVAL;
                }
            }

            memoryItem->resizeSubItemNameLabels();
            memoryItem->createOverlappingSubItemMarkings();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::extendMemoryItem()
//-----------------------------------------------------------------------------
void MemoryColumn::extendMemoryItem(MainMemoryGraphicsItem* graphicsItem, int& spacePlacementY)
{
    MemoryConnectionItem* firstConnection = graphicsItem->getFirstConnection();
    MemoryConnectionItem* lastConnection = graphicsItem->getLastConnection();

    if (firstConnection && lastConnection)
    {
        QPointF itemTopLeft = graphicsItem->boundingRect().topLeft();
        QPointF itemLowRight = graphicsItem->boundingRect().bottomRight();

        int lineWidth = graphicsItem->pen().width();

        qreal connectionTop =
            graphicsItem->mapFromItem(firstConnection, firstConnection->boundingRect().topLeft()).y() + lineWidth;
        qreal connectionLow =
            graphicsItem->mapFromItem(lastConnection, lastConnection->boundingRect().bottomRight()).y();

        bool connectionsAreBeyond = false;

        qreal extensionTop = itemLowRight.y();
        if (connectionTop < itemTopLeft.y())
        {
            extensionTop = connectionTop;
            connectionsAreBeyond = true;
        }

        qreal extensionLow = itemTopLeft.y();
        if (connectionLow > itemLowRight.y())
        {
            extensionLow = connectionLow;
            connectionsAreBeyond = true;
        }

        if (connectionsAreBeyond)
        {
            qreal positionX = itemTopLeft.x() + lineWidth;
            qreal extensionWidth = itemLowRight.x() - itemTopLeft.x() - lineWidth;
            qreal positionY = extensionTop;
            qreal extensionHeight = extensionLow - extensionTop;

            MemoryExtensionGraphicsItem* extensionItem = new MemoryExtensionGraphicsItem(positionX, positionY,
                extensionWidth, extensionHeight, graphicsItem->getContainingInstance(), graphicsItem);
            graphicsItem->setExtensionItem(extensionItem);

            if (extensionLow > itemLowRight.y())
            {
                qreal placementAddition = extensionLow - itemLowRight.y();
                spacePlacementY += placementAddition;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::moveUnconnectedMemoryItems()
//-----------------------------------------------------------------------------
void MemoryColumn::moveUnconnectedMemoryItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems)
{
    int firstPosition = getUnconnectedItemPosition(placedItems);

    foreach (QGraphicsItem* graphicsItem, getItems())
    {
        MainMemoryGraphicsItem* memoryItem = dynamic_cast<MainMemoryGraphicsItem*>(graphicsItem);
        if (memoryItem && memoryItem->isVisible() && !placedItems->contains(memoryItem))
        {
            moveGraphicsItem(memoryItem, firstPosition, MemoryDesignerConstants::UNCONNECTED_ITEM_INTERVAL);
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
        MemoryExtensionGraphicsItem* extensionItem = graphicsItem->getExtensionItem();
        if (extensionItem)
        {
            extensionHeight = extensionItem->sceneBoundingRect().height();
        }
        qreal subItemHeightAddition = graphicsItem->getSubItemHeightAddition();
        if (subItemHeightAddition > extensionHeight)
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
// Function: MemoryColumn::moveGraphicsItem()
//-----------------------------------------------------------------------------
void MemoryColumn::moveGraphicsItem(MainMemoryGraphicsItem* memoryItem, int& placementY, const qreal itemInterval)
{
    memoryItem->setY(placementY);

    placementY += memoryItem->getHeightWithSubItems() + itemInterval;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::memoryMapOverlapsInColumn()
//-----------------------------------------------------------------------------
bool MemoryColumn::memoryMapOverlapsInColumn(MainMemoryGraphicsItem* memoryGraphicsItem, QRectF memoryItemRect,
    int memoryPenWidth, QVector<MainMemoryGraphicsItem*> connectedSpaceItems,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps) const
{
    foreach (QGraphicsItem* graphicsItem, childItems())
    {
        MemoryMapGraphicsItem* comparisonMemoryItem = dynamic_cast<MemoryMapGraphicsItem*>(graphicsItem);
        if (comparisonMemoryItem)
        {
            QRectF comparisonRectangle = comparisonMemoryItem->getSceneRectangleWithSubItems();
            int comparisonLineWidth = comparisonMemoryItem->pen().width();

            bool isNotSameItem = comparisonMemoryItem != memoryGraphicsItem;
            bool itemIsConnectionedToSpaceItems = comparisonMemoryItem->isConnectedToSpaceItems(connectedSpaceItems);
            bool itemIsPlaced = placedMaps->contains(comparisonMemoryItem);
            bool overlap = MemoryDesignerConstants::itemOverlapsAnotherItem(
                memoryItemRect, memoryPenWidth, comparisonRectangle, comparisonLineWidth);

            if (isNotSameItem && itemIsConnectionedToSpaceItems && itemIsPlaced && overlap)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::itemOverlapsAnotherItem()
//-----------------------------------------------------------------------------
bool MemoryColumn::itemOverlapsAnotherItem(quint64 itemBaseAddress, quint64 itemLastAddress, QRectF memoryItemRect,
    int memoryPenWidth, MainMemoryGraphicsItem* comparisonMemoryItem) const
{
    QRectF comparisonRectangle = comparisonMemoryItem->getSceneRectangleWithSubItems();

    if (memoryItemRect.x() == comparisonRectangle.x())
    {
        int mapItemLineWidth = comparisonMemoryItem->pen().width();

        qreal comparisonItemEnd = comparisonRectangle.bottom();

        foreach (MemoryConnectionItem* connectionItem, comparisonMemoryItem->getMemoryConnections())
        {
            qreal connectionLow = connectionItem->sceneBoundingRect().bottom();
            if (connectionLow > comparisonItemEnd)
            {
                comparisonItemEnd = connectionLow;
            }
        }

        comparisonRectangle.setBottom(comparisonItemEnd);

        quint64 comparisonBaseAddress = comparisonMemoryItem->getBaseAddress();
        quint64 comparisonLastAddress = comparisonMemoryItem->getLastAddress();
        MemoryConnectionItem* comparisonItemLastConnection = comparisonMemoryItem->getLastConnection();
        if (comparisonItemLastConnection)
        {
            comparisonBaseAddress += comparisonItemLastConnection->getRangeStartValue();
            comparisonLastAddress = comparisonItemLastConnection->getRangeEndValue();
        }
        if (MemoryDesignerConstants::itemOverlapsAnotherItem(
            memoryItemRect, memoryPenWidth, comparisonRectangle, mapItemLineWidth) ||
            (itemBaseAddress >= comparisonBaseAddress && itemBaseAddress <= comparisonLastAddress) ||
            (comparisonBaseAddress >= itemBaseAddress && comparisonBaseAddress <= itemLastAddress))
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::itemOverlapsAnotherColumnItem()
//-----------------------------------------------------------------------------
bool MemoryColumn::itemOverlapsAnotherColumnItem(MainMemoryGraphicsItem* memoryItem, QRectF itemRectangle,
    int lineWidth) const
{
    foreach (MainMemoryGraphicsItem* comparisonItem, getGraphicsItemInOrder())
    {
        if (comparisonItem != memoryItem)
        {
            QRectF comparisonRectangle = comparisonItem->getSceneRectangleWithSubItems();
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
