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
// Function: SubMemoryLayout::getSubItemConnections()
//-----------------------------------------------------------------------------
QMap<quint64, MemoryConnectionItem*> SubMemoryLayout::getSubItemConnections(
    MemoryDesignerChildGraphicsItem* subItem, QVector<MemoryConnectionItem*> parentConnections)
{
    quint64 subItemBaseAddress = subItem->getBaseAddress();
    quint64 subItemLastAddress = subItem->getLastAddress();

    QMap<quint64, MemoryConnectionItem*> subItemConnections;

    foreach (MemoryConnectionItem* connectionItem, parentConnections)
    {
        bool temporary = true;
        quint64 connectionRangeStart = connectionItem->getRangeStartValue().toULongLong(&temporary, 16);
        quint64 connectionRangeEnd = connectionItem->getRangeEndValue().toULongLong(&temporary, 16);

        if ((subItemLastAddress >= connectionRangeStart && subItemLastAddress <= connectionRangeEnd) ||

            (subItemBaseAddress >= connectionRangeStart && subItemBaseAddress <= connectionRangeEnd) ||

            (connectionRangeStart >= subItemBaseAddress && connectionRangeEnd <= subItemLastAddress))
        {
            subItemConnections.insertMulti(connectionRangeStart, connectionItem);
        }
    }

    return subItemConnections;
}
