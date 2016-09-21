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

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::MemoryMapGraphicsItem()
//-----------------------------------------------------------------------------
MemoryMapGraphicsItem::MemoryMapGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
    QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent):
MainMemoryGraphicsItem(memoryItem->getName(), containingInstance->getName(), memoryItem->getAUB(), parent),
baseAddress_(getMemoryMapStart(memoryItem)),
lastAddress_(getMemoryMapEnd(memoryItem)),
addressBlocks_()
{
    QBrush memoryMapBrush(KactusColors::MEM_MAP_COLOR);
    setBrush(memoryMapBrush);

    quint64 memoryHeight = (lastAddress_ - baseAddress_ + 1);
    int memoryWidth = 500;

    setGraphicsRectangle(memoryWidth, memoryHeight);
    setupToolTip("memory map", baseAddress_, lastAddress_);
    setLabelPositions();
    setupAddressBlocks(memoryItem);
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
            if (blockItem->getType().compare("addressBlock", Qt::CaseInsensitive) == 0)
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
        if (blockItem->getType().compare("addressBlock", Qt::CaseInsensitive) == 0)
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
// Function: MemoryMapGraphicsItem::setupAddressBlocks()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::setupAddressBlocks(QSharedPointer<MemoryItem> memoryItem)
{
    qreal blockXPosition = boundingRect().width() / 8;
    QString const& mapAUB = memoryItem->getAUB();

    if (!memoryItem->getChildItems().isEmpty())
    {
        qreal blockTransferY = 0;

        QMap<quint64, AddressBlockGraphicsItem*> blocksInOrder;

        foreach (QSharedPointer<MemoryItem> blockItem, memoryItem->getChildItems())
        {
            if (blockItem->getType().compare("addressBlock", Qt::CaseInsensitive) == 0)
            {
                AddressBlockGraphicsItem* newBlock = new AddressBlockGraphicsItem(blockItem, false, this);

                quint64 baseAddress = blockItem->getAddress().toULongLong();

                blockTransferY = baseAddress * GridSize * 1.5;

                newBlock->setPos(blockXPosition, blockTransferY);

                blocksInOrder.insert(baseAddress, newBlock);

                addressBlocks_.append(newBlock);
            }
        }

        if (!blocksInOrder.isEmpty())
        {
            if (blocksInOrder.firstKey() > 0)
            {
                createEmptyBlock(blockXPosition, 0, blocksInOrder.firstKey() - 1, mapAUB);
            }

            QMapIterator<quint64, AddressBlockGraphicsItem*> blockIterator(blocksInOrder);

            quint64 currentOffset = 0;
            quint64 blockLastAddress = 0;

            while (blockIterator.hasNext())
            {
                blockIterator.next();

                currentOffset = blockIterator.key();
                if (currentOffset > blockLastAddress + 1)
                {
                    createEmptyBlock(blockXPosition, blockLastAddress + 1, currentOffset - 1, mapAUB);
                }

                quint64 rangeEnd = blockIterator.value()->getLastAddress();

                blockLastAddress = qMax(blockLastAddress, rangeEnd);
            }

            if (blockLastAddress != getLastAddress())
            {
                createEmptyBlock(blockXPosition, blockLastAddress + 1, getLastAddress(), mapAUB);
            }
        }
    }
    else
    {
        createEmptyBlock(blockXPosition, 0, getLastAddress(), mapAUB);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::createEmptyBlock()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::createEmptyBlock(qreal blockPositionX, quint64 beginAddress, quint64 rangeEnd,
    QString const& mapAUB)
{
    quint64 emptyBlockOffsetInInt = beginAddress;
    quint64 emptyBlockRangeInt = rangeEnd - emptyBlockOffsetInInt + 1;

    QString emptyBlockBaseAddress = QString::number(emptyBlockOffsetInInt);
    QString emptyBlockRange = QString::number(emptyBlockRangeInt);

    QSharedPointer<MemoryItem> emptyBlock (new MemoryItem("Reserved", "address block"));
    emptyBlock->setAddress(emptyBlockBaseAddress);
    emptyBlock->setRange(emptyBlockRange);
    emptyBlock->setAUB(mapAUB);

    AddressBlockGraphicsItem* newBlock = new AddressBlockGraphicsItem(emptyBlock, true, this);

    quint64 segmentTransferY = emptyBlockBaseAddress.toULongLong() * GridSize * 1.5;
    newBlock->setPos(blockPositionX, segmentTransferY);

    addressBlocks_.append(newBlock);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::changeChildItemRanges()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::changeChildItemRanges(quint64 offset)
{
    foreach(AddressBlockGraphicsItem* blockItem, addressBlocks_)
    {
        blockItem->changeAddressRange(offset);
    }
}
