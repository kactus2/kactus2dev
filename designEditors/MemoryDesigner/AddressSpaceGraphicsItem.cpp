//-----------------------------------------------------------------------------
// File: AddressSpaceGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Graphics item for visualizing an address space in the memory designer.
//-----------------------------------------------------------------------------

#include "AddressSpaceGraphicsItem.h"

#include <common/KactusColors.h>

#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/ConnectivityComponent.h>
#include <designEditors/MemoryDesigner/AddressSegmentGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryExtensionGraphicsItem.h>

#include <QBrush>
#include <QFont>
#include <QPen>

namespace AddressSpaceItemConstants
{
    const int MINIMUMSUBITEMHEIGHT = 2 * MemoryDesignerConstants::RANGEINTERVAL;
};

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::AddressSpaceGraphicsItem()
//-----------------------------------------------------------------------------
AddressSpaceGraphicsItem::AddressSpaceGraphicsItem(QSharedPointer<MemoryItem const> memoryItem,
    QVector<QString> identifierChain, QSharedPointer<ConnectivityComponent const> containingInstance,
    bool filterSegments, QGraphicsItem* parent):
MainMemoryGraphicsItem(memoryItem, containingInstance, MemoryDesignerConstants::ADDRESSSEGMENT_TYPE,
    filterSegments, identifierChain, parent),
cpuIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/settings-code_editor.png"), this))
{
    QBrush addressSpaceBrush(KactusColors::ADDRESS_SEGMENT);
    setBrush(addressSpaceBrush);

    quint64 spaceRangeInt = memoryItem->getRange().toULongLong();
    int spaceWidth = 250;

    setGraphicsRectangle(spaceWidth + 1, spaceRangeInt);

    setupGraphicsItem(0, spaceRangeInt - 1, QStringLiteral("Address Space"));

    cpuIcon_->setPos(spaceWidth / 2 - cpuIcon_->pixmap().width() - GridSize, GridSize * 3);
    cpuIcon_->setVisible(false);

    qreal segmentPositionX = - boundingRect().width() / 4;
    setupSubItems(segmentPositionX, getSubItemType(), memoryItem);

    setLabelPositions();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::~AddressSpaceGraphicsItem()
//-----------------------------------------------------------------------------
AddressSpaceGraphicsItem::~AddressSpaceGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::setLabelPositions()
{
    qreal nameLabelY = boundingRect().height() / 2 - (getNameLabel()->boundingRect().height() * 3 / 4);
    qreal instanceNameLabelY = nameLabelY + (getNameLabel()->boundingRect().height() / 2);

    qreal nameLabelX = 0;
    if (subItemsAreFiltered())
    {
        nameLabelX = boundingRect().left();
    }

    getNameLabel()->setPos(nameLabelX, nameLabelY);
    getInstanceNameLabel()->setPos(nameLabelX, instanceNameLabelY);

    fitLabel(getNameLabel());
    fitLabel(getInstanceNameLabel());

    qreal rangeStartLabelX = boundingRect().right() - getRangeStartLabel()->boundingRect().width();
    qreal rangeStartLabelY = boundingRect().top();
    qreal rangeEndLabelX = boundingRect().right() - getRangeEndLabel()->boundingRect().width();
    qreal rangeEndLabelY = boundingRect().bottom() - getRangeEndLabel()->boundingRect().height();

    getRangeStartLabel()->setPos(rangeStartLabelX, rangeStartLabelY);
    getRangeEndLabel()->setPos(rangeEndLabelX, rangeEndLabelY);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::createNewSubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* AddressSpaceGraphicsItem::createNewSubItem(
    QSharedPointer<MemoryItem const> subMemoryItem, bool isEmpty)
{
    AddressSegmentGraphicsItem* segmentItem = new AddressSegmentGraphicsItem(
        subMemoryItem, isEmpty, getIdentifierChain(), getContainingInstance(), this);

    connect(segmentItem, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)),
        this, SIGNAL(openComponentDocument(VLNV const&, QVector<QString>)), Qt::UniqueConnection);

    return segmentItem;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::createEmptySubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* AddressSpaceGraphicsItem::createEmptySubItem(quint64 beginAddress,
    quint64 rangeEnd)
{
    QString emptySegmentOffset = QString::number(beginAddress);
    QString emptySegmentRange = QString::number(rangeEnd - beginAddress + 1);

    QSharedPointer<MemoryItem> emptySegment(new MemoryItem("Empty", MemoryDesignerConstants::ADDRESSSEGMENT_TYPE));
    emptySegment->setOffset(emptySegmentOffset);
    emptySegment->setRange(emptySegmentRange);

    return createNewSubItem(emptySegment, true);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::condenseSpaceItem()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::condenseItemAndChildItems(
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections, bool condenseMemoryItems)
{
    if (!isCompressed())
    {
        quint64 spaceNewHeight = 0;

        if (getMemoryConnections().isEmpty())
        {
            spaceNewHeight = condenseChildItems(getBaseAddress(), getLastAddress(),
                AddressSpaceItemConstants::MINIMUMSUBITEMHEIGHT, condenseMemoryItems);
        }
        else
        {
            spaceNewHeight = getFilteredCompressedHeight(condenseMemoryItems);
        }

        if (spaceNewHeight > 0)
        {
            condense(spaceNewHeight);

            setCompressed(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getFilteredCompressedHeight()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGraphicsItem::getFilteredCompressedHeight(bool memoryItemsAreCompressed)
{
    const int CUTMODIFIER = 2;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedMemoryItems (new QVector<MainMemoryGraphicsItem*> ());
    visitedMemoryItems->append(this);

    QMap<quint64, MemoryConnectionItem*> allConnectedConnections =
        getAllConnectionsFromConnectedItems(visitedMemoryItems);
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(allConnectedConnections);
    connectionIterator.toFront();

    visitedMemoryItems->clear();
    visitedMemoryItems->append(this);

    if (hasProblemConnection(connectionIterator))
    {
        return getFilteredCompressedHeightByCoordinates(
            memoryItemsAreCompressed, visitedMemoryItems, connectionIterator);
    }

    connectionIterator.toFront();
    QVector<quint64> unCutAddresses = getUnCutAddressesFromConnections(connectionIterator);
    qSort(unCutAddresses);

    connectionIterator.toFront();
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        MemoryConnectionItem* connectionItem = connectionIterator.value();
        connectionItem->condenseToUnCutAddresses(
            visitedMemoryItems, unCutAddresses, CUTMODIFIER, memoryItemsAreCompressed);
    }

    connectionIterator.toFront();
    quint64 cutAreaBegin = getBaseAddress();
    quint64 itemLastAddress = getLastAddress();
    quint64 spaceCutArea = 0;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems(new QVector<MainMemoryGraphicsItem*> ());

    foreach (quint64 cutAreaEnd, unCutAddresses)
    {
        quint64 addressDifference = cutAreaEnd - cutAreaBegin;
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
                if (cutAreaBegin < itemLastAddress)
                {
                    spaceCutArea += transferRows;
                }

                qreal cutTransfer = transferRows * MemoryDesignerConstants::RANGEINTERVAL;
                cutTransfer = -cutTransfer;

                while (connectionIterator.hasNext())
                {
                    connectionIterator.next();

                    MemoryConnectionItem* connectionItem = connectionIterator.value();
                    connectionItem->moveCutConnectionAndConnectedItems(
                        movedItems, this, cutAreaBegin, cutAreaEnd, cutTransfer);
                }

                connectionIterator.toFront();
                movedItems->clear();
            }
        }

        cutAreaBegin = cutAreaEnd;
    }

    if (!subItemsAreFiltered())
    {
        compressSubItemsToUnCutAddresses(unCutAddresses, CUTMODIFIER, memoryItemsAreCompressed);
    }

    qreal originalHeight = boundingRect().height();
    qreal cutAreaHeight = spaceCutArea * MemoryDesignerConstants::RANGEINTERVAL;
    qreal condensedHeight = originalHeight - cutAreaHeight;

    return qMin(originalHeight, condensedHeight);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getFilteredCompressedHeightByCoordinates()
//-----------------------------------------------------------------------------
qreal AddressSpaceGraphicsItem::getFilteredCompressedHeightByCoordinates(bool memoryItemsAreCompressed,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedMemoryItems,
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator)
{
    const qreal CUTMODIFIER = 3 * MemoryDesignerConstants::RANGEINTERVAL;

    connectionIterator.toFront();
    QVector<qreal> unCutCoordinates = getUnCutCoordinatesFromConnections(connectionIterator);
    qSort(unCutCoordinates);

    connectionIterator.toFront();
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        MemoryConnectionItem* connectionItem = connectionIterator.value();
        connectionItem->condenseToUnCutCoordinates(
            visitedMemoryItems, unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);
    }

    connectionIterator.toFront();
    qreal cutAreaBegin = sceneBoundingRect().top();
    qreal itemLow = sceneBoundingRect().bottom();
    qreal spaceCutArea = 0;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems(new QVector<MainMemoryGraphicsItem*> ());

    QSharedPointer<QMap<MainMemoryGraphicsItem*, qreal> > itemMovementValues(
        new QMap<MainMemoryGraphicsItem*, qreal> ());

    foreach (qreal cutAreaEnd, unCutCoordinates)
    {
        qreal areaDifference = cutAreaEnd - cutAreaBegin;
        if (areaDifference > 0)
        {
            if (memoryItemsAreCompressed)
            {
                areaDifference = areaDifference - CUTMODIFIER;
            }
            else
            {
                qreal areaRequired = MemoryDesignerConstants::getRequiredAreaForUsedArea(areaDifference);
                areaDifference = areaDifference - areaRequired;
            }

            if (areaDifference > 0)
            {
                if (cutAreaBegin < itemLow)
                {
                    spaceCutArea += areaDifference;
                }

                qreal cutTransfer = areaDifference;
                cutTransfer = -cutTransfer;

                while (connectionIterator.hasNext())
                {
                    connectionIterator.next();

                    MemoryConnectionItem* connectionItem = connectionIterator.value();
                    connectionItem->modifyMovementValuesForItems(
                        itemMovementValues, movedItems, this, cutAreaBegin, cutAreaEnd, cutTransfer);
                }

                connectionIterator.toFront();
                movedItems->clear();
            }
        }

        cutAreaBegin = cutAreaEnd;
    }

    QMapIterator<MainMemoryGraphicsItem*, qreal> itemMovementIterator(*itemMovementValues);
    while (itemMovementIterator.hasNext())
    {
        itemMovementIterator.next();
        MainMemoryGraphicsItem* movedItem = itemMovementIterator.key();
        qreal movementValue = itemMovementIterator.value();
        movedItem->moveBy(0, movementValue);
    }

    connectionIterator.toFront();
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        connectionIterator.value()->reDrawConnection();
    }

    if (!subItemsAreFiltered())
    {
        compressSubItemsToUnCutCoordinates(unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);
    }

    qreal originalHeight = boundingRect().height();
    qreal condensedHeight = originalHeight - spaceCutArea;

    return qMin(originalHeight, condensedHeight);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getChainedMemoryConnections()
//-----------------------------------------------------------------------------
QMap<quint64, MemoryConnectionItem*> AddressSpaceGraphicsItem::getChainedMemoryConnections() const
{
    QMultiMap<quint64, MemoryConnectionItem*> chainedConnections = getMemoryConnections();
    QMapIterator<quint64, MemoryConnectionItem*> chainIterator (chainedConnections);
    while (chainIterator.hasNext())
    {
        chainIterator.next();
        MemoryConnectionItem* chainConnection = chainIterator.value();
        
        MainMemoryGraphicsItem* chainEndItem = chainConnection->getConnectionEndItem();
        AddressSpaceGraphicsItem* chainSpace = dynamic_cast<AddressSpaceGraphicsItem*>(chainEndItem);
        if (chainSpace && chainSpace != this)
        {
            QMap<quint64, MemoryConnectionItem*> secondConnections = chainSpace->getChainedMemoryConnections();
            QMapIterator<quint64, MemoryConnectionItem*> secondChainIterator (secondConnections);
            while (secondChainIterator.hasNext())
            {
                secondChainIterator.next();
                MemoryConnectionItem* secondChainConnection = secondChainIterator.value();

                if (!chainedConnections.values().contains(secondChainConnection))
                {
                    quint64 secondChainBaseAddress = secondChainIterator.key();
                    if (chainedConnections.keys().contains(secondChainBaseAddress))
                    {
                        QVector<MemoryConnectionItem*> sameKeyItems =
                            chainedConnections.values(secondChainBaseAddress).toVector();
                        sameKeyItems.append(secondChainConnection);

                        int amountOfSameKeys = chainedConnections.remove(secondChainBaseAddress) + 1;
                        for (int connectionIndex = amountOfSameKeys - 1; connectionIndex >= 0; --connectionIndex)
                        {
                            MemoryConnectionItem* sameKeyConnection = sameKeyItems.at(connectionIndex);
                            chainedConnections.insertMulti(secondChainBaseAddress, sameKeyConnection);
                        }
                    }
                    else
                    {
                        chainedConnections.insertMulti(secondChainBaseAddress, secondChainConnection);
                    }
                }
            }
        }
    }

    return chainedConnections;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getUnCutAddresses()
//-----------------------------------------------------------------------------
QVector<quint64> AddressSpaceGraphicsItem::getUnCutAddressesFromConnections(
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator) const
{
    QVector<quint64> unCutAddresses = getUnCutAddresses();

    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        quint64 connectionBaseAddress = connectionIterator.key();
        if (!unCutAddresses.contains(connectionBaseAddress))
        {
            unCutAddresses.append(connectionBaseAddress);
        }
        quint64 connectionLastAddress = connectionIterator.value()->getRangeEndValue();
        if (!unCutAddresses.contains(connectionLastAddress))
        {
            unCutAddresses.append(connectionLastAddress);
        }

        MemoryConnectionItem* connectionItem = connectionIterator.value();
        MainMemoryGraphicsItem* connectionStart = connectionItem->getConnectionStartItem();
        if (connectionStart != this)
        {
            foreach (quint64 address, connectionStart->getUnCutAddresses())
            {
                if (!unCutAddresses.contains(address))
                {
                    unCutAddresses.append(address);
                }
            }
        }

        MainMemoryGraphicsItem* connectionEnd = connectionItem->getConnectionEndItem();
        foreach (quint64 address, connectionEnd->getUnCutAddresses())
        {
            if (!unCutAddresses.contains(address))
            {
                unCutAddresses.append(address);
            }
        }
    }
    
    return unCutAddresses;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getUnCutCoordinatesFromConnections()
//-----------------------------------------------------------------------------
QVector<qreal> AddressSpaceGraphicsItem::getUnCutCoordinatesFromConnections(
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator) const
{
    QVector<qreal> unCutCoordinates = getUnCutCoordinates();

    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        MemoryConnectionItem* connectionItem = connectionIterator.value();

        qreal connectionTop = connectionItem->sceneBoundingRect().top();
        if (!unCutCoordinates.contains(connectionTop))
        {
            unCutCoordinates.append(connectionTop);
        }
        qreal connectionLow = connectionItem->sceneBoundingRect().bottom();
        if (!unCutCoordinates.contains(connectionLow))
        {
            unCutCoordinates.append(connectionLow);
        }

        MainMemoryGraphicsItem* connectionStart = connectionItem->getConnectionStartItem();
        if (connectionStart != this)
        {
            foreach (qreal coordinate, connectionStart->getUnCutCoordinates())
            {
                if (!unCutCoordinates.contains(coordinate))
                {
                    unCutCoordinates.append(coordinate);
                }
            }
        }

        MainMemoryGraphicsItem* connectionEnd = connectionItem->getConnectionEndItem();
        foreach (qreal coordinate, connectionEnd->getUnCutCoordinates())
        {
            if (!unCutCoordinates.contains(coordinate))
            {
                unCutCoordinates.append(coordinate);
            }
        }
    }

    return unCutCoordinates;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getAllConnectedSpaceItems()
//-----------------------------------------------------------------------------
QVector<MainMemoryGraphicsItem*> AddressSpaceGraphicsItem::getAllConnectedSpaceItems() const
{
    QVector<MainMemoryGraphicsItem*> allConnectedSpaceItems;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedItems (new QVector<MainMemoryGraphicsItem*> ());

    QMap<quint64, MemoryConnectionItem*> allConnections = getAllConnectionsFromConnectedItems(visitedItems);
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator (allConnections);
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        MemoryConnectionItem* connectionItem = connectionIterator.value();
        MainMemoryGraphicsItem* connectionStartItem = connectionItem->getConnectionStartItem();
        if (connectionStartItem && connectionStartItem != this &&
            dynamic_cast<AddressSpaceGraphicsItem*>(connectionStartItem) &&
            !allConnectedSpaceItems.contains(connectionStartItem))
        {
            allConnectedSpaceItems.append(connectionStartItem);
        }

        MainMemoryGraphicsItem* connectionEndItem = connectionItem->getConnectionEndItem();
        if (connectionEndItem && connectionEndItem != this &&
            dynamic_cast<AddressSpaceGraphicsItem*>(connectionEndItem) &&
            !allConnectedSpaceItems.contains(connectionEndItem))
        {
            allConnectedSpaceItems.append(connectionEndItem);
        }
    }

    return allConnectedSpaceItems;

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getSpaceItemsConnectedToSpaceItem()
//-----------------------------------------------------------------------------
QVector<MainMemoryGraphicsItem*> AddressSpaceGraphicsItem::getSpaceItemsConnectedToSpaceItem(
    MainMemoryGraphicsItem* memoryItem) const
{
    QVector<MainMemoryGraphicsItem*> connectedSpaceItems;

    AddressSpaceGraphicsItem* spaceItem = dynamic_cast<AddressSpaceGraphicsItem*>(memoryItem);
    if (spaceItem)
    {
        connectedSpaceItems = spaceItem->getAllConnectedSpaceItems();
        connectedSpaceItems.append(memoryItem);
    }

    return connectedSpaceItems;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::hasProblemConnection()
//-----------------------------------------------------------------------------
bool AddressSpaceGraphicsItem::hasProblemConnection(
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator) const
{
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        MemoryConnectionItem* connectionItem = connectionIterator.value();

        QMapIterator<quint64, MemoryConnectionItem*> comparisonIterator(connectionIterator);
        while (comparisonIterator.hasNext())
        {
            comparisonIterator.next();
            MemoryConnectionItem* comparisonConnection = comparisonIterator.value();

            if (connectionItem->getConnectionEndItem() == comparisonConnection->getConnectionEndItem() &&
                connectionItem->getRangeStartValue() != comparisonConnection->getRangeStartValue() &&
                connectionItem->sceneBoundingRect().top() == comparisonConnection->sceneBoundingRect().top())
            {
                return true;
            }
        }
    }

    return false;
}
