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
AddressSpaceGraphicsItem::AddressSpaceGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
    QSharedPointer<ConnectivityComponent> containingInstance, bool filterSegments, QGraphicsItem* parent):
MainMemoryGraphicsItem(memoryItem, containingInstance, MemoryDesignerConstants::ADDRESSSEGMENT_TYPE,
    filterSegments, parent),
cpuIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/compile.png"), this))
{
    QBrush addressSpaceBrush(KactusColors::ADDRESS_SEGMENT);
    setBrush(addressSpaceBrush);

    quint64 spaceRangeInt = memoryItem->getRange().toULongLong();
    int spaceWidth = 250;

    setGraphicsRectangle(spaceWidth + 1, spaceRangeInt);

    setupLabels(0, spaceRangeInt - 1);
    setupToolTip(QStringLiteral("Address Space"));

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
    QSharedPointer<MemoryItem> subMemoryItem, bool isEmpty)
{
    return new AddressSegmentGraphicsItem(subMemoryItem, isEmpty, getContainingInstance(), this);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::createEmptySubItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem* AddressSpaceGraphicsItem::createEmptySubItem(quint64 beginAddress,
    quint64 rangeEnd)
{
    QString emptySegmentOffset = QString::number(beginAddress);
    QString emptySegmentRange = QString::number(rangeEnd - beginAddress + 1);

    QSharedPointer<MemoryItem> emptySegment (new MemoryItem("Empty", MemoryDesignerConstants::ADDRESSSEGMENT_TYPE));
    emptySegment->setOffset(emptySegmentOffset);
    emptySegment->setRange(emptySegmentRange);

    return createNewSubItem(emptySegment, true);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::condenseSpaceItem()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::condenseItemAndChildItems(
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    if (!isCompressed())
    {
        quint64 spaceNewHeight = 0;

        if (getMemoryConnections().isEmpty())
        {
            spaceNewHeight = condenseChildItems(AddressSpaceItemConstants::MINIMUMSUBITEMHEIGHT);
        }
        else
        {
            spaceNewHeight = getFilteredCompressedHeight();
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
quint64 AddressSpaceGraphicsItem::getFilteredCompressedHeight()
{
    const int CUTMODIFIER = 2;

    QMap<quint64, MemoryConnectionItem*> chainedConnections = getChainedMemoryConnections();
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(chainedConnections);

    QVector<quint64> unCutAddresses = getUnCutAddressesFromConnections(connectionIterator);
    qSort(unCutAddresses);

    connectionIterator.toFront();
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        MemoryConnectionItem* connectionItem = connectionIterator.value();
        connectionItem->condenseToUnCutAddresses(unCutAddresses, CUTMODIFIER);
    }

    QList<quint64> unCutAddressesInList = unCutAddresses.toList();

    QVector<MainMemoryGraphicsItem*> movedItems;

    connectionIterator.toFront();
    quint64 cutAreaBegin = getBaseAddress();
    quint64 itemLastAddress = getLastAddress();
    quint64 spaceCutArea = 0;

    foreach (quint64 cutAreaEnd, unCutAddresses)
    {
        qint64 addressDifference = cutAreaEnd - cutAreaBegin - CUTMODIFIER;
        if (addressDifference > 0)
        {
            if (cutAreaBegin < itemLastAddress)
            {
                spaceCutArea += addressDifference;
            }

            qreal cutTransfer = -addressDifference * MemoryDesignerConstants::RANGEINTERVAL;

            while (connectionIterator.hasNext())
            {
                connectionIterator.next();
                quint64 connectionBaseAddress = connectionIterator.key();
                if (connectionBaseAddress >= cutAreaEnd)
                {
                    MemoryConnectionItem* connectionItem = connectionIterator.value();
                    MainMemoryGraphicsItem* connectionStartItem = connectionItem->getConnectionStartItem();
                    if (!movedItems.contains(connectionStartItem))
                    {
                        connectionItem->onMoveConnectionInY(connectionStartItem, cutTransfer);
                        movedItems.append(connectionItem->getConnectionEndItem());
                    }
                }
            }

            connectionIterator.toFront();
            movedItems.clear();
        }

        cutAreaBegin = cutAreaEnd;
    }

    if (!subItemsAreFiltered())
    {
        compressSubItemsToUnCutAddresses(unCutAddresses, CUTMODIFIER);
    }

    qreal originalHeight = boundingRect().height();
    qreal cutAreaHeight = spaceCutArea * MemoryDesignerConstants::RANGEINTERVAL;
    qreal condensedHeight = originalHeight - cutAreaHeight;

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
// Function: AddressSpaceGraphicsItem::getAllConnectedSpaceItems()
//-----------------------------------------------------------------------------
QVector<MainMemoryGraphicsItem*> AddressSpaceGraphicsItem::getAllConnectedSpaceItems(
    MainMemoryGraphicsItem* origin) const
{
    QVector<MainMemoryGraphicsItem*> allConnectedSpaceItems;

    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        MainMemoryGraphicsItem* connectionStartItem = connectionItem->getConnectionStartItem();
        MainMemoryGraphicsItem* connectionEndItem = connectionItem->getConnectionEndItem();
        if (connectionStartItem == this && connectionEndItem != origin)
        {
            foreach (MainMemoryGraphicsItem* connectedSpaceItem,
                getSpaceItemsConnectedToSpaceItem(connectionEndItem, connectionStartItem))
            {
                allConnectedSpaceItems.append(connectedSpaceItem);
            }
        }
        else if (connectionEndItem == this && connectionStartItem != origin)
        {
            foreach (MainMemoryGraphicsItem* connectedSpaceItem,
                getSpaceItemsConnectedToSpaceItem(connectionStartItem, connectionEndItem))
            {
                allConnectedSpaceItems.append(connectedSpaceItem);
            }
        }
    }

    return allConnectedSpaceItems;

}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getSpaceItemsConnectedToSpaceItem()
//-----------------------------------------------------------------------------
QVector<MainMemoryGraphicsItem*> AddressSpaceGraphicsItem::getSpaceItemsConnectedToSpaceItem(
    MainMemoryGraphicsItem* memoryItem, MainMemoryGraphicsItem* originSpaceItem) const
{
    QVector<MainMemoryGraphicsItem*> connectedSpaceItems;

    AddressSpaceGraphicsItem* spaceItem = dynamic_cast<AddressSpaceGraphicsItem*>(memoryItem);
    if (spaceItem)
    {
        connectedSpaceItems = spaceItem->getAllConnectedSpaceItems(originSpaceItem);
        connectedSpaceItems.append(memoryItem);
    }

    return connectedSpaceItems;
}
