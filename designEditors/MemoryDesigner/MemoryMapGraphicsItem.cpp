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
#include <designEditors/MemoryDesigner/RegisterGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryExtensionGraphicsItem.h>

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
MemoryMapGraphicsItem::MemoryMapGraphicsItem(QSharedPointer<MemoryItem> memoryItem, bool filterAddressBlocks,
    bool filterRegisters, bool filterFields, QSharedPointer<ConnectivityComponent> containingInstance,
    QGraphicsItem* parent):
MainMemoryGraphicsItem(memoryItem, containingInstance->getName(), MemoryDesignerConstants::ADDRESSBLOCK_TYPE,
    filterAddressBlocks, parent),
addressUnitBits_(memoryItem->getAUB()),
filterAddressBlocks_(filterAddressBlocks),
filterRegisters_(filterRegisters),
filterFields_(filterFields),
subItemWidth_(0)
{
    QPair<quint64, quint64> memoryRanges = getMemoryRanges(memoryItem);
    quint64 baseAddress = memoryRanges.first;
    quint64 lastAddress = memoryRanges.second;

    QBrush memoryMapBrush(KactusColors::MEM_MAP_COLOR);
    setBrush(memoryMapBrush);

    quint64 memoryHeight = (lastAddress - baseAddress + 1);
    int memoryWidth = 1280;
    if (filterRegisters_ || (filterAddressBlocks_ && filterFields_))
    {
        memoryWidth = 250;
    }
    else if (filterFields_)
    {
        memoryWidth = MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 6;
    }
    else if (filterAddressBlocks_)
    {
        memoryWidth -= MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2 + 1;
    }

    setGraphicsRectangle(memoryWidth, memoryHeight);
    setupLabels(baseAddress, lastAddress);
    setupToolTip(QStringLiteral("Memory Map"));

    qreal blockXPosition = MemoryDesignerConstants::MAPSUBITEMPOSITIONX;

    subItemWidth_ = getSubItemWidth();

    setupSubItems(blockXPosition, memoryItem);
    setLabelPositions();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::~MemoryMapGraphicsItem()
//-----------------------------------------------------------------------------
MemoryMapGraphicsItem::~MemoryMapGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::setupSubItems()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::setupSubItems(qreal blockXPosition, QSharedPointer<MemoryItem> memoryItem)
{
    QString subItemType = getSubItemType();
    QSharedPointer<MemoryItem> usedMemoryItem = memoryItem;

    if (filterAddressBlocks_ && !filterRegisters_)
    {
        setFilterSubItems(filterRegisters_);

        subItemType = MemoryDesignerConstants::REGISTER_TYPE;
        usedMemoryItem = QSharedPointer<MemoryItem>(new MemoryItem(memoryItem->getName(), memoryItem->getType()));
        usedMemoryItem->setAUB(memoryItem->getAUB());

        foreach (QSharedPointer<MemoryItem> subItem, memoryItem->getChildItems())
        {
            if (subItem->getType().compare(MemoryDesignerConstants::ADDRESSBLOCK_TYPE, Qt::CaseInsensitive) == 0)
            {
                foreach (QSharedPointer<MemoryItem> registerItem, subItem->getChildItems())
                {
                    usedMemoryItem->addChild(registerItem);
                }
            }
        }
    }

    SubMemoryLayout::setupSubItems(blockXPosition, subItemType, usedMemoryItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::getMemoryRanges()
//-----------------------------------------------------------------------------
QPair<quint64, quint64> MemoryMapGraphicsItem::getMemoryRanges(QSharedPointer<MemoryItem> memoryItem) const
{
    quint64 baseAddress = 0;
    quint64 lastAddress = 0;
    if (memoryItem->getChildItems().size() > 0)
    {
        bool firstBlock = true;

        foreach (QSharedPointer<MemoryItem> blockItem, memoryItem->getChildItems())
        {
            if (blockItem->getType().compare(MemoryDesignerConstants::ADDRESSBLOCK_TYPE, Qt::CaseInsensitive) == 0)
            {
                quint64 blockBaseAddress = blockItem->getAddress().toULongLong();
                quint64 blockRange = blockItem->getRange().toULongLong();
                quint64 blockEndPoint = blockBaseAddress + blockRange - 1;

                if (firstBlock)
                {
                    baseAddress = blockBaseAddress;
                    firstBlock = false;
                }
                else
                {
                    if (blockBaseAddress < baseAddress)
                    {
                        baseAddress = blockBaseAddress;
                    }
                }

                if (blockEndPoint > lastAddress)
                {
                    lastAddress = blockEndPoint;
                }
            }
        }
    }

    QPair<quint64, quint64> memoryRanges(baseAddress, lastAddress);
    return memoryRanges;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::setLabelPositions()
{
    QRectF nameLabelRect = getNameLabel()->boundingRect();
    QRectF instanceLabelRect = getInstanceNameLabel()->boundingRect();

    qreal nameLabelY = boundingRect().height() / 2 - (nameLabelRect.height() * 3 / 4);
    qreal instanceNameLabelY = nameLabelY + nameLabelRect.height() / 2 + 1;

    qreal nameLabelX = 0;
    qreal instanceNameLabelX = 0;

    if (subItemsAreFiltered())
    {
        nameLabelX = boundingRect().right() - nameLabelRect.width();
        instanceNameLabelX = boundingRect().right() - instanceLabelRect.width();
    }
    else
    {
        nameLabelX =
            boundingRect().left() + (MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2) - nameLabelRect.width();
        instanceNameLabelX =
            boundingRect().left() + (MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2) - instanceLabelRect.width();
    }

    getNameLabel()->setPos(nameLabelX, nameLabelY);
    getInstanceNameLabel()->setPos(instanceNameLabelX, instanceNameLabelY);

    fitLabel(getNameLabel());
    fitLabel(getInstanceNameLabel());

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
    MemoryDesignerChildGraphicsItem* childItem = 0;

    if (!filterAddressBlocks_)
    {
        childItem = new AddressBlockGraphicsItem(
            subMemoryItem, isEmpty, filterRegisters_, filterFields_, subItemWidth_, getContainingInstance(), this);
    }
    else if (!filterRegisters_)
    {
        childItem = new RegisterGraphicsItem(
            subMemoryItem, isEmpty, subItemWidth_, filterFields_, getContainingInstance(), this);
    }

    return childItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::getSubItemWidth()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphicsItem::getSubItemWidth() const
{
    int subItemWidth = (boundingRect().width() - MemoryDesignerConstants::MAPSUBITEMPOSITIONX * 2) + 1;
    return subItemWidth;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::createEmptySubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* MemoryMapGraphicsItem::createEmptySubItem(quint64 beginAddress, quint64 rangeEnd)
{
    QSharedPointer<MemoryItem> emptyItem;

    QString emptyBaseAddress = QString::number(beginAddress);

    if (!filterAddressBlocks_)
    {
        QString emptyBlockRange = QString::number(rangeEnd - beginAddress + 1);

        emptyItem = QSharedPointer<MemoryItem>(
            new MemoryItem(MemoryDesignerConstants::RESERVED_NAME, MemoryDesignerConstants::ADDRESSBLOCK_TYPE));
        emptyItem->setAddress(emptyBaseAddress);
        emptyItem->setRange(emptyBlockRange);
        emptyItem->setAUB(addressUnitBits_);
    }
    else if (!filterRegisters_)
    {
        quint64 emptyRegisterRangeInt = rangeEnd - beginAddress + 1;
        QString emptyRegisterRange = QString::number(emptyRegisterRangeInt);

        unsigned int intAUB = addressUnitBits_.toInt();
        quint64 registerSize = emptyRegisterRangeInt * intAUB;

        emptyItem = QSharedPointer<MemoryItem>(
            new MemoryItem(MemoryDesignerConstants::RESERVED_NAME, MemoryDesignerConstants::REGISTER_TYPE));
        emptyItem->setAddress(emptyBaseAddress);
        emptyItem->setSize(QString::number(registerSize));
        emptyItem->setAUB(addressUnitBits_);
    }

    return createNewSubItem(emptyItem, true);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::condenseItemAndChildItems()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::condenseItemAndChildItems(QSharedPointer<QVector<MemoryConnectionItem*> >)
{
    if (!isCompressed())
    {
        quint64 memoryMapNewHeight = 0;
        int subItemHeight = getMinimumHeightForSubItems();
        
        if (getMemoryConnections().isEmpty())
        {
            memoryMapNewHeight = condenseChildItems(subItemHeight);
        }

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
void MemoryMapGraphicsItem::compressMapItem(QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    if (!isCompressed())
    {
        quint64 newMapHeight = getCompressedHeight(getMinimumHeightForSubItems(), this, movedConnections);

        if (newMapHeight > 0)
        {
            condense(newMapHeight);

            setCompressed(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::getMinimumRequiredHeight()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphicsItem::getMinimumRequiredHeight(quint64 connectionBaseAddress, quint64 connectionEndAddress)
    const
{
    int subItemHeight = getMinimumHeightForSubItems();

    qreal height = boundingRect().height();

    return SubMemoryLayout::getMinimumRequiredHeight(
        subItemHeight, connectionBaseAddress, connectionEndAddress, getBaseAddress(), getLastAddress(), height);
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

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::isConnectedToSpaceItems()
//-----------------------------------------------------------------------------
bool MemoryMapGraphicsItem::isConnectedToSpaceItems(QVector<MainMemoryGraphicsItem*> spaceItems) const
{
    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        if (spaceItems.contains(connectionItem->getConnectionStartItem()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::changeWidth()
//-----------------------------------------------------------------------------
void MemoryMapGraphicsItem::changeWidth(qreal widthChange)
{
    qreal newWidth = boundingRect().width() + widthChange - 1;
    setRect(-newWidth / 2, 0, newWidth, boundingRect().height() - 1);
    setLabelPositions();

    qreal xPosition = pos().x() + widthChange / 2;
    setX(xPosition);

    if (getExtensionItem())
    {
        getExtensionItem()->setRect(
            -newWidth / 2, boundingRect().bottom() - 1, newWidth, getExtensionItem()->boundingRect().height() - 1);
    }

    if (!subItemsAreFiltered() && !getSubMemoryItems().isEmpty())
    {
        QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
        while (subItemIterator.hasNext())
        {
            subItemIterator.next();
            MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();

            subItem->setX(subItem->pos().x() - widthChange / 2);

            if (!filterAddressBlocks_)
            {
                AddressBlockGraphicsItem* blockItem = dynamic_cast<AddressBlockGraphicsItem*>(subItem);
                if (blockItem)
                {
                    blockItem->changeWidth(widthChange);
                }
            }
            else if (!filterRegisters_)
            {
                RegisterGraphicsItem* registerItem = dynamic_cast<RegisterGraphicsItem*>(subItem);
                if (registerItem)
                {
                    registerItem->changeWidth(widthChange);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapGraphicsItem::getMaximumNeededChangeInFieldWidth()
//-----------------------------------------------------------------------------
qreal MemoryMapGraphicsItem::getMaximumNeededChangeInFieldWidth() const
{
    qreal maximumNeededWithChange = 0;
    if (!filterFields_)
    {
        foreach (MemoryDesignerChildGraphicsItem* subItem, getSubMemoryItems())
        {
            if (!filterAddressBlocks_)
            {
                AddressBlockGraphicsItem* blockItem = dynamic_cast<AddressBlockGraphicsItem*>(subItem);
                if (blockItem)
                {
                    maximumNeededWithChange =
                        qMax(maximumNeededWithChange, blockItem->getMaximumNeededChangeInFieldWidth());
                }
            }
            else
            {
                RegisterGraphicsItem* registerItem = dynamic_cast<RegisterGraphicsItem*>(subItem);
                if (registerItem)
                {
                    maximumNeededWithChange =
                        qMax(maximumNeededWithChange, registerItem->getMaximumNeededChangeInFieldWidth());
                }
            }
        }
    }

    return maximumNeededWithChange;
}
