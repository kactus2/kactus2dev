//-----------------------------------------------------------------------------
// File: SubMemoryLayout.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.09.2016
//
// Description:
// Layout for sub memory items of a memory graphics item within memory designer.
//-----------------------------------------------------------------------------

#include "SubMemoryLayout.h"

#include <common/KactusColors.h>

#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>

#include <QBrush>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::SubMemoryLayout()
//-----------------------------------------------------------------------------
SubMemoryLayout::SubMemoryLayout(QSharedPointer<MemoryItem> memoryItem, QString const& subItemType,
    MemoryDesignerGraphicsItem* mainGraphicsItem):
subMemoryItems_(),
memoryItem_(memoryItem),
mainGraphicsItem_(mainGraphicsItem),
subItemType_(subItemType)
{

}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::~SubMemoryLayout()
//-----------------------------------------------------------------------------
SubMemoryLayout::~SubMemoryLayout()
{

}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::setupSubItems()
//-----------------------------------------------------------------------------
void SubMemoryLayout::setupSubItems(qreal subItemPositionX)
{
    quint64 itemBaseAddress = mainGraphicsItem_->getBaseAddress();
    quint64 itemLastAddress = mainGraphicsItem_->getLastAddress();

    if (!memoryItem_->getChildItems().isEmpty())
    {
        QMap<quint64, MemoryDesignerChildGraphicsItem*> subItemsInOrder;

        foreach (QSharedPointer<MemoryItem> subMemoryItem, memoryItem_->getChildItems())
        {
            if (subMemoryItem->getType().compare(subItemType_, Qt::CaseInsensitive) == 0)
            {
                MemoryDesignerChildGraphicsItem* newSubItem = createNewSubItem(subMemoryItem, false);

                positionNewSubItem(subItemPositionX, itemBaseAddress, newSubItem);

                subItemsInOrder.insert(newSubItem->getBaseAddress(), newSubItem);
            }
        }

        if (!subItemsInOrder.isEmpty())
        {
            if (subItemsInOrder.firstKey() > 0)
            {
                MemoryDesignerChildGraphicsItem* newEmptyItem = createEmptySubItem(0, subItemsInOrder.firstKey() - 1);
                positionNewSubItem(subItemPositionX, itemBaseAddress, newEmptyItem);
            }

            QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(subItemsInOrder);

            quint64 currentOffset = itemBaseAddress;
            quint64 subItemLastAddress = itemBaseAddress;

            while (subItemIterator.hasNext())
            {
                subItemIterator.next();

                currentOffset = subItemIterator.key();
                if (currentOffset > subItemLastAddress + 1)
                {
                    MemoryDesignerChildGraphicsItem* newEmptyItem =
                        createEmptySubItem(subItemLastAddress + 1, currentOffset - 1);
                    positionNewSubItem(subItemPositionX, itemBaseAddress, newEmptyItem);
                }

                quint64 rangeEnd = subItemIterator.value()->getLastAddress();

                subItemLastAddress = qMax(subItemLastAddress, rangeEnd);
            }

            if (subItemLastAddress < itemLastAddress)
            {
                MemoryDesignerChildGraphicsItem* newEmptyItem =
                    createEmptySubItem(subItemLastAddress + 1, itemLastAddress);
                positionNewSubItem(subItemPositionX, itemBaseAddress, newEmptyItem);
            }
        }
    }
    else
    {
        MemoryDesignerChildGraphicsItem* newEmptyItem = createEmptySubItem(itemBaseAddress, itemLastAddress);
        positionNewSubItem(subItemPositionX, itemBaseAddress, newEmptyItem);
    }
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::positionNewSubItem()
//-----------------------------------------------------------------------------
void SubMemoryLayout::positionNewSubItem(qreal subItemXPosition, quint64 mainItemBaseAddress,
    MemoryDesignerChildGraphicsItem* newSubItem)
{
    quint64 offset = newSubItem->getBaseAddress();
    qreal segmentTransferY = (offset - mainItemBaseAddress) * MemoryDesignerConstants::RANGEINTERVAL;

    newSubItem->setPos(subItemXPosition, segmentTransferY);

    subMemoryItems_.insert(offset, newSubItem);
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::changeChildItemRanges()
//-----------------------------------------------------------------------------
void SubMemoryLayout::changeChildItemRanges(quint64 offset)
{
    QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subMemoryIterator(subMemoryItems_);

    while(subMemoryIterator.hasNext())
    {
        subMemoryIterator.next();

        subMemoryIterator.value()->changeAddressRange(offset);
    }
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getSubMemoryItems()
//-----------------------------------------------------------------------------
QMap<quint64, MemoryDesignerChildGraphicsItem*> SubMemoryLayout::getSubMemoryItems() const
{
    return subMemoryItems_;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::condenseChildItems()
//-----------------------------------------------------------------------------
qreal SubMemoryLayout::condenseChildItems(qreal minimumSubItemHeight)
{
    quint64 positionY = 0;

    QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subMemoryIterator(subMemoryItems_);
    while (subMemoryIterator.hasNext())
    {
        subMemoryIterator.next();

        MemoryDesignerChildGraphicsItem* subItem = subMemoryIterator.value();

        positionY = condenseSubItem(subItem, minimumSubItemHeight, positionY);
    }

    return positionY;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::condenseSubItem()
//-----------------------------------------------------------------------------
quint64 SubMemoryLayout::condenseSubItem(MemoryDesignerChildGraphicsItem* subItem, qreal minimumSubItemHeight,
    quint64 positionY)
{
    SubMemoryLayout* subLayout = dynamic_cast<SubMemoryLayout*>(subItem);
    if (subLayout)
    {
        quint64 newSubItemHeight = subLayout->condenseChildItems(minimumSubItemHeight);
        subItem->condense(newSubItemHeight);
    }
    else
    {
        quint64 subBaseAddress = subItem->getBaseAddress();
        quint64 subLastAddress = subItem->getLastAddress();

        if (subLastAddress - subBaseAddress > 1)
        {
            subItem->condense(minimumSubItemHeight);
        }
    }

    subItem->setPos(subItem->pos().x(), positionY);

    return positionY + subItem->boundingRect().bottom();
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getMinimumRequiredHeight()
//-----------------------------------------------------------------------------
qreal SubMemoryLayout::getMinimumRequiredHeight(qreal minimumSubItemHeight, quint64 connectionBaseAddress,
    quint64 connectionEndAddress) const
{
    qreal height = 0;

    foreach (MemoryDesignerChildGraphicsItem* subItem, subMemoryItems_)
    {
        SubMemoryLayout* subLayout = dynamic_cast<SubMemoryLayout*>(subItem);
        if (subLayout)
        {
            height += subLayout->getMinimumRequiredHeight(
                minimumSubItemHeight, connectionBaseAddress, connectionEndAddress);
        }
        else
        {
            quint64 subBaseAddress = subItem->getBaseAddress();
            quint64 subLastAddress = subItem->getLastAddress();

            if (connectionEndAddress >= subBaseAddress && subLastAddress >= connectionBaseAddress)
            {
                if (subLastAddress - subBaseAddress > 2)
                {
                    height += minimumSubItemHeight;
                }
                else
                {
                    height += subItem->boundingRect().height() - 1;
                }
            }
        }
    }

    return height;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::addConnectionToSubItems()
//-----------------------------------------------------------------------------
void SubMemoryLayout::addConnectionToSubItems(MemoryConnectionItem* connectionItem)
{
    quint64 connectionBaseAddress = connectionItem->getRangeStartValue().toULongLong(0, 16);
    quint64 connectionLastAddress = connectionItem->getRangeEndValue().toULongLong(0, 16);

    QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(subMemoryItems_);
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();

        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
        quint64 subItemBaseAddress = subItem->getBaseAddress();
        quint64 subItemLastAddress = subItem->getLastAddress();

        if (connectionLastAddress >= subItemBaseAddress && subItemLastAddress >= connectionBaseAddress)
        {
            subItem->addMemoryConnection(connectionItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getCompressedHeight()
//-----------------------------------------------------------------------------
quint64 SubMemoryLayout::getCompressedHeight(qreal minimumSubItemHeight, SubMemoryLayout* mainItem)
{
    quint64 newHeight = 0;
    qreal newSubItemHeight = 0;

    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections (new QVector<MemoryConnectionItem*>());

    QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(subMemoryItems_);
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();

        newSubItemHeight = minimumSubItemHeight;

        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
        SubMemoryLayout* subLayout = dynamic_cast<SubMemoryLayout*>(subItem);
        if (subLayout)
        {
            newSubItemHeight = subLayout->getCompressedHeight(minimumSubItemHeight, mainItem);
            subItem->condense(newSubItemHeight);
            subItem->setPos(subItem->pos().x(), newHeight);
            newHeight += newSubItemHeight;
        }
        else
        {
            if (!subItem->getMemoryConnections().isEmpty())
            {
                newSubItemHeight =
                    getSubItemHeight(mainItem, subItem, minimumSubItemHeight, newHeight, movedConnections);
            }

            newHeight = condenseSubItem(subItem, newSubItemHeight, newHeight);
        }
    }

    return newHeight;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getSubItemHeight()
//-----------------------------------------------------------------------------
qreal SubMemoryLayout::getSubItemHeight(SubMemoryLayout* mainItem, MemoryDesignerChildGraphicsItem* subItem,
    qreal minimumSubItemHeight, quint64 yPosition,
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    qreal newSubItemHeight = 0;

    quint64 subItemBaseAddress = subItem->getBaseAddress();
    quint64 subItemLastAddress = subItem->getLastAddress();

    QMapIterator<quint64, MemoryConnectionItem*> subItemConnectionIterator(subItem->getMemoryConnections());
    while (subItemConnectionIterator.hasNext())
    {
        subItemConnectionIterator.next();

        MemoryConnectionItem* connectionItem = subItemConnectionIterator.value();

        qreal subItemConnectionHeight = getSubItemHeightForConnection(mainItem, subItemBaseAddress,
            subItemLastAddress, subItem, connectionItem, yPosition, newSubItemHeight, minimumSubItemHeight);

        newSubItemHeight += subItemConnectionHeight;
    }

    return newSubItemHeight;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getSubItemHeightForConnection()
//-----------------------------------------------------------------------------
quint64 SubMemoryLayout::getSubItemHeightForConnection(SubMemoryLayout* mainItem, quint64 subItemBaseAddress,
    quint64 subItemLastAddress, MemoryDesignerChildGraphicsItem* subItem, MemoryConnectionItem* connectionItem,
    quint64 yPosition, quint64 newSubItemHeight, qreal minimumSubItemHeight) const
{
    qreal subItemConnectionHeight = 0;
    if (subItemLastAddress - subItemBaseAddress <= 2)
    {
        subItemConnectionHeight = subItem->boundingRect().height() - 1;
    }
    else
    {
        QVector<MemoryDesignerChildGraphicsItem*> subItemsInConnection =
            getSubItemsInConnection(mainItem, connectionItem);

        MemoryDesignerGraphicsItem* graphicsItem = dynamic_cast<MemoryDesignerGraphicsItem*>(mainItem);
        QString mainName = graphicsItem->name();
        QString subItemName = subItem->name();

        qreal availableArea =
            getAvailableArea(mainItem, yPosition, subItemsInConnection, newSubItemHeight, connectionItem);

        int condensableSubItems = 0;
        foreach (MemoryDesignerChildGraphicsItem* comparisonItem, subItemsInConnection)
        {
            quint64 comparisonBaseAddress = comparisonItem->getBaseAddress();
            quint64 comparisonLastAddress = comparisonItem->getLastAddress();
            if (comparisonLastAddress - comparisonBaseAddress > 2)
            {
                condensableSubItems++;
            }
            else
            {
                availableArea -= comparisonItem->boundingRect().height() - 1;
            }
        }

        subItemConnectionHeight = availableArea / condensableSubItems - 1;
        if (newSubItemHeight + subItemConnectionHeight < minimumSubItemHeight)
        {
            subItemConnectionHeight = minimumSubItemHeight;
        }

        quint64 connectionHeight = subItemConnectionHeight;
        qreal heightDecimals = subItemConnectionHeight - connectionHeight;
        subItemConnectionHeight = connectionHeight;
        if (heightDecimals > 0)
        {
            subItemConnectionHeight++;
        }
    }

    return subItemConnectionHeight;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getSubItemsInConnection()
//-----------------------------------------------------------------------------
QVector<MemoryDesignerChildGraphicsItem*> SubMemoryLayout::getSubItemsInConnection(SubMemoryLayout* memoryItem,
    MemoryConnectionItem* connectionItem) const
{
    QVector<MemoryDesignerChildGraphicsItem*> subItemsInConnection;

    QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(memoryItem->getSubMemoryItems());
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();

        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();

        SubMemoryLayout* subLayout = dynamic_cast<SubMemoryLayout*>(subItem);
        if (subLayout)
        {
            QVector<MemoryDesignerChildGraphicsItem*> newSubItemsInConnection =
                subLayout->getSubItemsInConnection(subLayout, connectionItem);
            foreach (MemoryDesignerChildGraphicsItem* connectedSubItem, newSubItemsInConnection)
            {
                subItemsInConnection.append(connectedSubItem);
            }
        }
        else if (subItem->getConnectionsInVector().contains(connectionItem))
        {
            subItemsInConnection.append(subItem);
        }
    }

    return subItemsInConnection;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getAvailableArea()
//-----------------------------------------------------------------------------
qreal SubMemoryLayout::getAvailableArea(SubMemoryLayout* mainItem, quint64 yPosition,
    QVector<MemoryDesignerChildGraphicsItem*> subItemsInConnection, quint64 newSubItemHeight,
    MemoryConnectionItem* connectionItem) const
{
    quint64 connectionLastAddress = connectionItem->getRangeEndValue().toULongLong(0, 16);

    quint64 subItemLastAddress = 0;

    foreach (MemoryDesignerChildGraphicsItem* subItem, subItemsInConnection)
    {
        quint64 comparisonLastAddress = subItem->getLastAddress();
        if (comparisonLastAddress > subItemLastAddress)
        {
            subItemLastAddress = comparisonLastAddress;
        }
    }

    qreal availableAreaInLayout = 0;
    MemoryDesignerGraphicsItem* mainGraphicsItem = dynamic_cast<MemoryDesignerGraphicsItem*>(mainItem);
    if (mainGraphicsItem)
    {
        availableAreaInLayout =
            mainGraphicsItem->boundingRect().height() - (yPosition + newSubItemHeight);
    }

    qreal connectionHeight = connectionItem->boundingRect().height();

    qreal availableArea = availableAreaInLayout;
    if (connectionLastAddress > subItemLastAddress)
    {
        availableArea = qMin(availableArea, connectionHeight - MemoryDesignerConstants::RANGEINTERVAL * 2);
    }
    else
    {
        availableArea = connectionHeight;
    }

    return availableArea;
}
