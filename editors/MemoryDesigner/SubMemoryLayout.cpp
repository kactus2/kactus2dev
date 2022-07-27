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

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryDesignerGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryConnectionItem.h>

#include <QBrush>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::SubMemoryLayout()
//-----------------------------------------------------------------------------
SubMemoryLayout::SubMemoryLayout(QSharedPointer<MemoryItem const> memoryItem, QString const& subItemType,
    bool filterSubItems, MemoryDesignerGraphicsItem* mainGraphicsItem):
subMemoryItems_(),
itemType_(memoryItem->getType()),
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
    QSharedPointer<MemoryItem const> memoryItem)
{
    if (!filterSubItems_)
    {
        quint64 itemBaseAddress = mainGraphicsItem_->getBaseAddress();
        quint64 itemLastAddress = mainGraphicsItem_->getLastAddress();

        if (!memoryItem->getChildItems().isEmpty())
        {
            QMultiMap<quint64, MemoryDesignerChildGraphicsItem*> subItemsInOrder;

            foreach (QSharedPointer<MemoryItem const> subMemoryItem, memoryItem->getChildItems())
            {
                if (subMemoryItem->getType().compare(subItemType, Qt::CaseInsensitive) == 0)
                {
                    MemoryDesignerChildGraphicsItem* newSubItem = createNewSubItem(subMemoryItem, false);
                    quint64 subItemBaseAddress = newSubItem->getBaseAddress();
                    quint64 subItemLastAddress = newSubItem->getLastAddress();

                    if (subItemBaseAddress > itemLastAddress || subItemLastAddress > itemLastAddress)
                    {
                        setFaultySubItemColor(newSubItem);
                    }

                    positionNewSubItem(subItemPositionX, itemBaseAddress, subItemBaseAddress, newSubItem);
                    subItemsInOrder.insert(subItemBaseAddress, newSubItem);
                }
            }

            if (!subItemsInOrder.isEmpty())
            {
                quint64 subItemLastAddress = itemBaseAddress;

                if (subItemsInOrder.firstKey() > itemBaseAddress)
                {
                    subItemLastAddress = subItemsInOrder.firstKey() - 1;
                    createAndPositionNewEmptySubItem(
                        itemBaseAddress, subItemLastAddress, subItemPositionX, itemBaseAddress);
                }

                QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(subItemsInOrder);

                while (subItemIterator.hasNext())
                {
                    subItemIterator.next();

                    quint64 currentOffset = subItemIterator.key();
                    if (currentOffset > subItemLastAddress + 1)
                    {
                        createAndPositionNewEmptySubItem(
                            subItemLastAddress + 1, currentOffset - 1, subItemPositionX, itemBaseAddress);
                    }

                    quint64 rangeEnd = subItemIterator.value()->getLastAddress();

                    subItemLastAddress = qMax(subItemLastAddress, rangeEnd);
                }

                if (subItemLastAddress < itemLastAddress)
                {
                    createAndPositionNewEmptySubItem(
                        subItemLastAddress + 1, itemLastAddress, subItemPositionX, itemBaseAddress);
                }
            }
        }

        if (getSubMemoryItems().isEmpty())
        {
            createAndPositionNewEmptySubItem(itemBaseAddress, itemLastAddress, subItemPositionX, itemBaseAddress);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::createAndPositionNewEmptySubItem()
//-----------------------------------------------------------------------------
void SubMemoryLayout::createAndPositionNewEmptySubItem(quint64 subItemBaseAddress, quint64 subItemLastAddress,
    qreal subItemPositionX, quint64 mainItemBaseAddress)
{
    MemoryDesignerChildGraphicsItem* newEmptyItem = createEmptySubItem(subItemBaseAddress, subItemLastAddress);
    positionNewSubItem(subItemPositionX, mainItemBaseAddress, subItemBaseAddress, newEmptyItem);
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::positionNewSubItem()
//-----------------------------------------------------------------------------
void SubMemoryLayout::positionNewSubItem(qreal subItemXPosition, quint64 mainItemBaseAddress,
    quint64 subItemOffset, MemoryDesignerChildGraphicsItem* newSubItem)
{
    qreal segmentTransferY = (subItemOffset - mainItemBaseAddress) * MemoryDesignerConstants::RANGEINTERVAL;
    newSubItem->setPos(subItemXPosition, segmentTransferY);

    subMemoryItems_.insert(subItemOffset, newSubItem);
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::changeChildItemRanges()
//-----------------------------------------------------------------------------
void SubMemoryLayout::changeChildItemRanges(quint64 offset)
{
    QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subMemoryIterator(subMemoryItems_);

    while(subMemoryIterator.hasNext())
    {
        subMemoryIterator.next();

        subMemoryIterator.value()->changeAddressRange(offset);
    }
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getSubMemoryItems()
//-----------------------------------------------------------------------------
QMultiMap<quint64, MemoryDesignerChildGraphicsItem*> SubMemoryLayout::getSubMemoryItems() const
{
    return subMemoryItems_;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::condenseChildItems()
//-----------------------------------------------------------------------------
qreal SubMemoryLayout::condenseChildItems(quint64 itemBaseAddress, quint64 itemLastAddress,
    qreal minimumSubItemHeight, bool memoryItemsAreCondensed)
{
    quint64 positionY = 0;

    if (filterSubItems_)
    {
        if (memoryItemsAreCondensed)
        {
            positionY = minimumSubItemHeight;
        }
        else
        {
            quint64 addressRange = itemLastAddress - itemBaseAddress + 1;
            positionY = MemoryDesignerConstants::getAreaSizeForRange(addressRange);
        }
    }
    else
    {
        QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subMemoryIterator(subMemoryItems_);
        while (subMemoryIterator.hasNext())
        {
            subMemoryIterator.next();

            MemoryDesignerChildGraphicsItem* subItem = subMemoryIterator.value();

            positionY = condenseSubItem(subItem, minimumSubItemHeight, positionY, memoryItemsAreCondensed);
        }
    }

    return positionY;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::condenseSubItem()
//-----------------------------------------------------------------------------
quint64 SubMemoryLayout::condenseSubItem(MemoryDesignerChildGraphicsItem* subItem, qreal minimumSubItemHeight,
    quint64 positionY, bool memoryItemsAreCondensed)
{
    quint64 subBaseAddress = subItem->getBaseAddress();
    quint64 subLastAddress = subItem->getLastAddress();

    SubMemoryLayout* subLayout = dynamic_cast<SubMemoryLayout*>(subItem);
    if (subLayout)
    {
        quint64 newSubItemHeight = subLayout->condenseChildItems(
            subBaseAddress, subLastAddress, minimumSubItemHeight, memoryItemsAreCondensed);
        subItem->condense(newSubItemHeight);
    }
    else
    {
        if (subLastAddress - subBaseAddress > 1)
        {
            qreal newSubItemHeight = minimumSubItemHeight;

            if (!memoryItemsAreCondensed)
            {
                quint64 addressRange = subLastAddress - subBaseAddress + 1;
                newSubItemHeight = MemoryDesignerConstants::getAreaSizeForRange(addressRange);
            }

            subItem->condense(newSubItemHeight);
        }
    }

    subItem->setPos(subItem->pos().x(), positionY);

    return positionY + subItem->boundingRect().bottom();
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::addConnectionToSubItems()
//-----------------------------------------------------------------------------
void SubMemoryLayout::addConnectionToSubItems(MemoryConnectionItem* connectionItem)
{
    quint64 connectionBaseAddress = connectionItem->getRangeStartValue();
    quint64 connectionLastAddress = connectionItem->getRangeEndValue();

    QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(subMemoryItems_);
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

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::resizeSubItemNameLabels()
//-----------------------------------------------------------------------------
void SubMemoryLayout::resizeSubItemNameLabels()
{
    QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();

        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
        subItem->fitNameLabel();

        SubMemoryLayout* subItemLayout = dynamic_cast<SubMemoryLayout*>(subItem);
        if (subItemLayout)
        {
            subItemLayout->resizeSubItemNameLabels();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::createOverlappingSubItemMarkings()
//-----------------------------------------------------------------------------
void SubMemoryLayout::createOverlappingSubItemMarkings()
{
    QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();

        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();

        SubMemoryLayout* subItemLayout = dynamic_cast<SubMemoryLayout*>(subItem);
        if (subItemLayout)
        {
            subItemLayout->createOverlappingSubItemMarkings();
        }

        quint64 subItemBaseAddress = subItem->getBaseAddress();
        quint64 subItemLastAddress = subItem->getLastAddress();

        QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> comparisonItemIterator = subItemIterator;
        while (comparisonItemIterator.hasNext())
        {
            comparisonItemIterator.next();

            MemoryDesignerChildGraphicsItem* comparisonItem = comparisonItemIterator.value();

            quint64 comparisonBaseAddress = comparisonItem->getBaseAddress();
            quint64 comparisonLastAddress = comparisonItem->getLastAddress();

            if ((comparisonBaseAddress >= subItemBaseAddress && comparisonBaseAddress <= subItemLastAddress) ||
                (comparisonLastAddress >= subItemBaseAddress && comparisonLastAddress <= subItemLastAddress) ||
                (subItemBaseAddress >= comparisonBaseAddress && subItemBaseAddress <= comparisonLastAddress) ||
                (subItemLastAddress >= comparisonBaseAddress && subItemLastAddress <= comparisonLastAddress))
            {
                setFaultySubItemColor(subItem);
                setFaultySubItemColor(comparisonItem);
            }

            else if (comparisonBaseAddress > subItemLastAddress)
            {
                break;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::setFaultySubItemColor()
//-----------------------------------------------------------------------------
void SubMemoryLayout::setFaultySubItemColor(MemoryDesignerChildGraphicsItem* subItem)
{
    QBrush collisionBrush(KactusColors::MISSING_COMPONENT);
    subItem->setBrush(collisionBrush);
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getSubItemHeightAddition()
//-----------------------------------------------------------------------------
quint64 SubMemoryLayout::getSubItemHeightAddition() const
{
    quint64 heightAddition = 0;

    if (mainGraphicsItem_ && !subItemsAreFiltered())
    {
        quint64 mainItemLastAddress = mainGraphicsItem_->getLastAddress();
        quint64 mainItemLow = mainGraphicsItem_->sceneBoundingRect().bottom();

        QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
        while (subItemIterator.hasNext())
        {
            subItemIterator.next();

            MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
            quint64 subItemLastAddress = subItem->getLastAddress();
            SubMemoryLayout* subLayout = dynamic_cast<SubMemoryLayout*>(subItem);
            if (subLayout && !subLayout->subItemsAreFiltered())
            {
                heightAddition = subLayout->getSubItemHeightAddition();
            }
            else if (subItemLastAddress > mainItemLastAddress)
            {
                quint64 subItemLow = subItem->sceneBoundingRect().bottom();
                quint64 subItemAddition = subItemLow - mainItemLow;
                if (subItemAddition > heightAddition)
                {
                    heightAddition = subItemAddition;
                }
            }
        }
    }

    return heightAddition;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getHeightWithSubItems()
//-----------------------------------------------------------------------------
quint64 SubMemoryLayout::getHeightWithSubItems() const
{
    quint64 height = mainGraphicsItem_->boundingRect().height() + getSubItemHeightAddition();

    return height;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getSceneRectangleWithSubItems()
//-----------------------------------------------------------------------------
QRectF SubMemoryLayout::getSceneRectangleWithSubItems() const
{
    QRectF rectangle = mainGraphicsItem_->sceneBoundingRect();

    quint64 heightWithSubItems = getHeightWithSubItems();
    if (heightWithSubItems > rectangle.height())
    {
        rectangle.setHeight(heightWithSubItems);
    }

    return rectangle;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getUnCutAddresses()
//-----------------------------------------------------------------------------
QVector<quint64> SubMemoryLayout::getUnCutAddresses() const
{
    QVector<quint64> unCutAddresses;
    
    quint64 baseAddress = mainGraphicsItem_->getBaseAddress();
    unCutAddresses.append(baseAddress);
    quint64 lastAddress = mainGraphicsItem_->getLastAddress();
    if (baseAddress != lastAddress)
    {
        unCutAddresses.append(lastAddress);
    }

    if (!subItemsAreFiltered())
    {
        QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
        while (subItemIterator.hasNext())
        {
            subItemIterator.next();
            MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
            quint64 subItemBaseAddress = subItem->getBaseAddress();
            if (!unCutAddresses.contains(subItemBaseAddress))
            {
                unCutAddresses.append(subItemBaseAddress);
            }

            quint64 subItemLastAddress = subItem->getLastAddress();
            if (!unCutAddresses.contains(subItemLastAddress))
            {
                unCutAddresses.append(subItemLastAddress);
            }

            SubMemoryLayout* subItemLayout = dynamic_cast<SubMemoryLayout*>(subItem);
            if (subItemLayout && !subItemLayout->subItemsAreFiltered())
            {
                foreach (quint64 address, subItemLayout->getUnCutAddresses())
                {
                    unCutAddresses.append(address);
                }
            }
        }
    }

    return unCutAddresses;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::getUnCutCoordinates()
//-----------------------------------------------------------------------------
QVector<qreal> SubMemoryLayout::getUnCutCoordinates() const
{
    QVector<qreal> unCutCoordinates;

    qreal itemTop = mainGraphicsItem_->sceneBoundingRect().top();
    unCutCoordinates.append(itemTop);
    qreal itemLow = mainGraphicsItem_->sceneBoundingRect().bottom();
    if (itemTop != itemLow)
    {
        unCutCoordinates.append(itemLow);
    }

    if (!subItemsAreFiltered())
    {
        QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
        while (subItemIterator.hasNext())
        {
            subItemIterator.next();
            MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
            qreal subItemTop = subItem->sceneBoundingRect().top();
            if (!unCutCoordinates.contains(subItemTop))
            {
                unCutCoordinates.append(subItemTop);
            }

            qreal subItemLow = subItem->sceneBoundingRect().bottom();
            if (!unCutCoordinates.contains(subItemLow))
            {
                unCutCoordinates.append(subItemLow);
            }

            SubMemoryLayout* subItemLayout = dynamic_cast<SubMemoryLayout*>(subItem);
            if (subItemLayout && !subItemLayout->subItemsAreFiltered())
            {
                foreach (qreal coordinate, subItemLayout->getUnCutCoordinates())
                {
                    unCutCoordinates.append(coordinate);
                }
            }
        }
    }

    return unCutCoordinates;
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::compressSubItemsToUnCutAddresses()
//-----------------------------------------------------------------------------
void SubMemoryLayout::compressSubItemsToUnCutAddresses(QVector<quint64> unCutAddresses, const int CUTMODIFIER,
    bool memoryItemsAreCompressed)
{
    quint64 lastAddress = mainGraphicsItem_->getLastAddress();

    QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();
        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();

        SubMemoryLayout* subItemLayout = dynamic_cast<SubMemoryLayout*>(subItem);
        if (subItemLayout)
        {
            subItemLayout->compressSubItemsToUnCutAddresses(unCutAddresses, CUTMODIFIER, memoryItemsAreCompressed);
        }

        subItem->compressToUnCutAddresses(unCutAddresses, CUTMODIFIER, memoryItemsAreCompressed);

        quint64 subItemLastAddress = subItem->getLastAddress();
        if (subItemLastAddress > lastAddress)
        {
            lastAddress = subItemLastAddress;
        }
    }

    quint64 itemBaseAddress = mainGraphicsItem_->getBaseAddress();

    quint64 areaBegin = itemBaseAddress;
    foreach (quint64 areaEnd, unCutAddresses)
    {
        if (areaBegin < lastAddress && areaEnd > itemBaseAddress)
        {
            qint64 addressDifference = areaEnd - areaBegin;
            if (addressDifference > 0)
            {
                qint64 transferRows = 0;

                if (memoryItemsAreCompressed)
                {
                    transferRows = addressDifference - CUTMODIFIER;
                }
                else
                {
                    quint64 addressRange = addressDifference + 1;
                    int requiredRows = MemoryDesignerConstants::getRequiredRowsForRange(addressRange);

                    transferRows = addressRange - requiredRows;
                }

                if (transferRows > 0)
                {
                    qreal transferY = -transferRows * MemoryDesignerConstants::RANGEINTERVAL;

                    subItemIterator.toFront();
                    while (subItemIterator.hasNext())
                    {
                        subItemIterator.next();
                        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
                        quint64 subItemBaseAddress = subItem->getBaseAddress();
                        if (subItemBaseAddress > areaBegin)
                        {
                            subItem->moveBy(0, transferY);
                        }
                    }
                }
            }
        }
        else if (areaBegin >= lastAddress)
        {
            return;
        }

        areaBegin = areaEnd;
    }
}

//-----------------------------------------------------------------------------
// Function: SubMemoryLayout::compressSubItemsToUnCutCoordinates()
//-----------------------------------------------------------------------------
void SubMemoryLayout::compressSubItemsToUnCutCoordinates(QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER,
    bool memoryItemsAreCompressed)
{
    qreal lastCoordinate = mainGraphicsItem_->sceneBoundingRect().bottom();

    QMultiMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();
        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();

        SubMemoryLayout* subItemLayout = dynamic_cast<SubMemoryLayout*>(subItem);
        if (subItemLayout)
        {
            subItemLayout->compressSubItemsToUnCutCoordinates(
                unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);
        }

        subItem->compressToUnCutCoordinates(unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);

        qreal subItemLow = subItem->sceneBoundingRect().bottom();
        if (subItemLow > lastCoordinate)
        {
            lastCoordinate = subItemLow;
        }
    }

    QMap<MemoryDesignerChildGraphicsItem*, qreal> subItemTransferValues;

    qreal itemTop = mainGraphicsItem_->sceneBoundingRect().top();
    qreal areaBegin = itemTop;
    foreach (qreal areaEnd, unCutCoordinates)
    {
        if (areaBegin < lastCoordinate && areaEnd > itemTop)
        {
            qreal areaDifference = areaEnd - areaBegin;
            if (areaDifference > 0)
            {
                if (memoryItemsAreCompressed)
                {
                    areaDifference = areaDifference - CUTMODIFIER;
                }
                else
                {
                    qreal requiredArea = MemoryDesignerConstants::getRequiredAreaForUsedArea(areaDifference);
                    areaDifference = areaDifference - requiredArea;
                }

                if (areaDifference > 0)
                {
                    qreal transferY = -areaDifference;

                    subItemIterator.toFront();
                    while (subItemIterator.hasNext())
                    {
                        subItemIterator.next();
                        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();

                        qreal subItemTop = subItem->sceneBoundingRect().top();
                        if (subItemTop > areaBegin)
                        {
                            qreal newSubItemTransferValue = subItemTransferValues.value(subItem, 0) + transferY;
                            subItemTransferValues.insert(subItem, newSubItemTransferValue);
                        }
                    }
                }
            }
        }
        else if (areaBegin >= lastCoordinate)
        {
            break;
        }

        areaBegin = areaEnd;
    }

    QMapIterator<MemoryDesignerChildGraphicsItem*, qreal> subItemTransferIterator(subItemTransferValues);
    while (subItemTransferIterator.hasNext())
    {
        subItemTransferIterator.next();
        MemoryDesignerChildGraphicsItem* subItem = subItemTransferIterator.key();
        subItem->moveBy(0, subItemTransferIterator.value());
    }
}
