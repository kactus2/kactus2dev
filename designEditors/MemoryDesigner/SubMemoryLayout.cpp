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
    qreal segmentTransferY = (offset - mainItemBaseAddress) * GridSize * 1.5;

    newSubItem->setPos(subItemXPosition, segmentTransferY);

    subMemoryItems_.append(newSubItem);
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::changeChildItemRanges()
//-----------------------------------------------------------------------------
void SubMemoryLayout::changeChildItemRanges(quint64 offset)
{
    foreach (MemoryDesignerChildGraphicsItem* subItem, subMemoryItems_)
    {
        subItem->changeAddressRange(offset);
    }
}
