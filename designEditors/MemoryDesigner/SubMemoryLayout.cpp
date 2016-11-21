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
#include <designEditors/MemoryDesigner/MemoryDesignerGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>

#include <QBrush>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::SubMemoryLayout()
//-----------------------------------------------------------------------------
SubMemoryLayout::SubMemoryLayout(QSharedPointer<MemoryItem> memoryItem, QString const& subItemType,
    bool filterSubItems, MemoryDesignerGraphicsItem* mainGraphicsItem):
subMemoryItems_(),
memoryItem_(memoryItem),
mainGraphicsItem_(mainGraphicsItem),
subItemType_(subItemType),
filterSubItems_(filterSubItems)
{

}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::~SubMemoryLayout()
//-----------------------------------------------------------------------------
SubMemoryLayout::~SubMemoryLayout()
{

}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getSubItemType()
//-----------------------------------------------------------------------------
QString SubMemoryLayout::getSubItemType() const
{
    return subItemType_;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::setupSubItems()
//-----------------------------------------------------------------------------
void SubMemoryLayout::setupSubItems(qreal subItemPositionX, QString const& subItemType,
    QSharedPointer<MemoryItem> memoryItem)
{
    if (!filterSubItems_)
    {
        quint64 itemBaseAddress = mainGraphicsItem_->getBaseAddress();
        quint64 itemLastAddress = mainGraphicsItem_->getLastAddress();

        if (!memoryItem->getChildItems().isEmpty())
        {
            QMap<quint64, MemoryDesignerChildGraphicsItem*> subItemsInOrder;

            foreach (QSharedPointer<MemoryItem> subMemoryItem, memoryItem->getChildItems())
            {
                if (subMemoryItem->getType().compare(subItemType, Qt::CaseInsensitive) == 0)
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

    if (filterSubItems_)
    {
        positionY = minimumSubItemHeight;
    }
    else
    {
        QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subMemoryIterator(subMemoryItems_);
        while (subMemoryIterator.hasNext())
        {
            subMemoryIterator.next();

            MemoryDesignerChildGraphicsItem* subItem = subMemoryIterator.value();

            positionY = condenseSubItem(subItem, minimumSubItemHeight, positionY);
        }
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
    quint64 connectionEndAddress, quint64 itemBaseAddress, quint64 itemLastAddress, qreal itemHeight) const
{
    qreal height = 0;

    if (filterSubItems_)
    {
        height = getMinimumItemHeight(itemBaseAddress, itemLastAddress, itemHeight, minimumSubItemHeight);
    }
    else
    {
        foreach (MemoryDesignerChildGraphicsItem* subItem, subMemoryItems_)
        {
            quint64 subBaseAddress = subItem->getBaseAddress();
            quint64 subLastAddress = subItem->getLastAddress();
            qreal subItemHeight = subItem->boundingRect().height();

            SubMemoryLayout* subLayout = dynamic_cast<SubMemoryLayout*>(subItem);
            if (subLayout)
            {
                height += subLayout->getMinimumRequiredHeight(minimumSubItemHeight, connectionBaseAddress,
                    connectionEndAddress, subBaseAddress, subLastAddress, subItemHeight);
            }
            else
            {
                if (connectionEndAddress >= subBaseAddress && subLastAddress >= connectionBaseAddress)
                {
                    height +=
                        getMinimumItemHeight(subBaseAddress, subLastAddress, subItemHeight, minimumSubItemHeight);
                }
            }
        }
    }

    return height;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getMinimumItemHeight()
//-----------------------------------------------------------------------------
qreal SubMemoryLayout::getMinimumItemHeight(quint64 baseAddress, quint64 lastAddress, qreal itemHeight,
    qreal minimumItemHeight) const
{
    qreal height = 0;
    if (addressRangeIsWithinLimit(baseAddress, lastAddress))
    {
        height = minimumItemHeight;
    }
    else
    {
        height = itemHeight - 1;
    }

    return height;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::addressRangeIsWithinLimit()
//-----------------------------------------------------------------------------
bool SubMemoryLayout::addressRangeIsWithinLimit(quint64 baseAddress, quint64 lastAddress) const
{
    return lastAddress - baseAddress > 2;
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
quint64 SubMemoryLayout::getCompressedHeight(qreal minimumSubItemHeight, SubMemoryLayout* mainItem,
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    quint64 newHeight = 0;
    qreal newSubItemHeight = 0;

    if (filterSubItems_)
    {
        newHeight = getFilteredCompressedHeight(mainItem, newSubItemHeight, minimumSubItemHeight);

        MemoryDesignerGraphicsItem* graphicsItem = dynamic_cast<MemoryDesignerGraphicsItem*>(this);
        quint64 itemHeight = graphicsItem->boundingRect().height();
        if (newHeight > itemHeight)
        {
            newHeight = itemHeight;
        }
    }
    else
    {
        QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(subMemoryItems_);
        while (subItemIterator.hasNext())
        {
            subItemIterator.next();

            newSubItemHeight = minimumSubItemHeight;

            MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
            SubMemoryLayout* subLayout = dynamic_cast<SubMemoryLayout*>(subItem);
            if (subLayout)
            {
                newSubItemHeight = subLayout->getCompressedHeight(minimumSubItemHeight, this, movedConnections);
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
    quint64 subItemLastAddress, MemoryDesignerGraphicsItem* subItem, MemoryConnectionItem* connectionItem,
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

        qreal availableArea =
            getAvailableArea(mainItem, yPosition, subItemsInConnection, newSubItemHeight, connectionItem);

        int condensableSubItems = 0;
        foreach (MemoryDesignerChildGraphicsItem* comparisonItem, subItemsInConnection)
        {
            quint64 comparisonBaseAddress = comparisonItem->getBaseAddress();
            quint64 comparisonLastAddress = comparisonItem->getLastAddress();
            if (addressRangeIsWithinLimit(comparisonBaseAddress, comparisonLastAddress))
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
        if (subLayout && !subLayout->subItemsAreFiltered())
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

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getFilteredCompressedHeight()
//-----------------------------------------------------------------------------
quint64 SubMemoryLayout::getFilteredCompressedHeight(SubMemoryLayout* parentLayout, quint64 yPosition,
    qreal minimumSubItemHeight)
{
    qreal filteredHeight = 0;

    MemoryDesignerGraphicsItem* memoryItem = dynamic_cast<MemoryDesignerGraphicsItem*>(this);
    if (memoryItem)
    {
        quint64 baseAddress = memoryItem->getBaseAddress();
        quint64 lastAddress = memoryItem->getLastAddress();

        QString itemType = memoryItem_->getType();
        bool isMainItem = itemType.compare(MemoryDesignerConstants::ADDRESSSPACE_TYPE, Qt::CaseInsensitive) == 0 ||
            itemType.compare(MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0;

        quint64 previousConnectionBaseAddress = 0;

        QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(memoryItem->getMemoryConnections());
        while (connectionIterator.hasNext())
        {
            connectionIterator.next();
            MemoryConnectionItem* connectionItem = connectionIterator.value();

            quint64 connectionBaseAddress = connectionItem->getRangeStartValue().toULongLong(0, 16);

            if (isMainItem)
            {
                lastAddress += connectionBaseAddress;

                filteredHeight =
                    connectionItem->sceneBoundingRect().bottom() - memoryItem->sceneBoundingRect().top() - 1;

                quint64 connectionLastAddress = connectionItem->getRangeEndValue().toULongLong(0, 16);
                if (lastAddress < connectionLastAddress)
                {
                    filteredHeight -= MemoryDesignerConstants::RANGEINTERVAL * 2;
                }

                previousConnectionBaseAddress = connectionBaseAddress;
            }
            else
            {
                filteredHeight = getSubItemHeightForConnection(parentLayout, baseAddress, lastAddress, memoryItem,
                    connectionItem, yPosition, filteredHeight, minimumSubItemHeight);
            }
        }
    }

    if (filteredHeight == 0)
    {
        filteredHeight = minimumSubItemHeight;
    }

    return filteredHeight;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::subItemsAreFiltered()
//-----------------------------------------------------------------------------
bool SubMemoryLayout::subItemsAreFiltered() const
{
    return filterSubItems_;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::setFilterSubItems()
//-----------------------------------------------------------------------------
void SubMemoryLayout::setFilterSubItems(bool filterValue)
{
    filterSubItems_ = filterValue;
}
