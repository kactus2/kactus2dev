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
MainMemoryGraphicsItem(memoryItem, containingInstance->getName(), MemoryDesignerConstants::ADDRESSSEGMENT_TYPE,
    filterSegments, parent),
cpuIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/compile.png"), this))
{
    QBrush addressSpaceBrush(KactusColors::ADDRESS_SEGMENT);
    setBrush(addressSpaceBrush);

    quint64 spaceRangeInt = memoryItem->getRange().toULongLong();
    int spaceWidth = 250;

    setGraphicsRectangle(spaceWidth + 1, spaceRangeInt);

    setupToolTip("address space", 0, spaceRangeInt - 1);

    cpuIcon_->setPos(spaceWidth / 2 - cpuIcon_->pixmap().width() - GridSize, GridSize * 3);
    cpuIcon_->setVisible(false);

    setLabelPositions();

    qreal segmentPositionX = - boundingRect().width() / 4;
    setupSubItems(segmentPositionX, getSubItemType(), memoryItem);
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
    qreal aubLabelY = boundingRect().top() - getAUBLabel()->boundingRect().height();
    qreal namelabelY = aubLabelY - getNameLabel()->boundingRect().height() + GridSize / 2;
    qreal instanceNameLabelY = namelabelY - getInstanceNameLabel()->boundingRect().height() + GridSize / 2;

    getInstanceNameLabel()->setPos(-getInstanceNameLabel()->boundingRect().width() / 2, instanceNameLabelY);
    getNameLabel()->setPos(-getNameLabel()->boundingRect().width() / 2, namelabelY);
    getAUBLabel()->setPos(-getAUBLabel()->boundingRect().width() / 2, aubLabelY);

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
    return new AddressSegmentGraphicsItem(subMemoryItem, isEmpty, this);
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
            spaceNewHeight =
                getCompressedHeight(AddressSpaceItemConstants::MINIMUMSUBITEMHEIGHT, this, movedConnections);
        }

        if (spaceNewHeight > 0)
        {
            condense(spaceNewHeight);

            setCompressed(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getSubItemHeight()
//-----------------------------------------------------------------------------
qreal AddressSpaceGraphicsItem::getSubItemHeight(SubMemoryLayout* mainItem,
    MemoryDesignerChildGraphicsItem* subItem, qreal minimumSubItemHeight, quint64 yPosition,
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    qreal newSubItemHeight = 0;

    if (mainItem == this)
    {
        MemoryConnectionItem* firstConnection = subItem->getMemoryConnections().first();
        quint64 connectionRangeEnd = firstConnection->getRangeEndValue().toULongLong(0, 16);
        qreal yTransfer = (sceneBoundingRect().top() + yPosition) - firstConnection->sceneBoundingRect().top();
        quint64 previousConnectionLow = firstConnection->sceneBoundingRect().bottom();

        quint64 subItemBaseAddress = subItem->getBaseAddress();
        quint64 subItemLastAddress = subItem->getLastAddress();

        qreal intervalBetweenConnections = 0;

        quint64 connectionLowPoint = firstConnection->sceneBoundingRect().bottom();
        quint64 connectionHighPoint = firstConnection->sceneBoundingRect().top();

        quint64 previousConnectionBaseAddress = firstConnection->getRangeStartValue().toULongLong(0, 16);
        MemoryConnectionItem* previousConnection = firstConnection;

        foreach (MemoryConnectionItem* connectionItem, subItem->getMemoryConnections())
        {
            if (connectionItem->getConnectionStartItem() == mainItem)
            {
                quint64 connectionBaseAddress = connectionItem->getRangeStartValue().toULongLong(0, 16);

                yTransfer = getTransferY(connectionBaseAddress, previousConnectionBaseAddress,
                    connectionRangeEnd, previousConnectionLow, connectionItem, previousConnection, yTransfer);

                moveConnectionItem(connectionItem, yTransfer, movedConnections);

                quint64 newConnectionEnd = getConnectionRangeEndValue(connectionItem);
                if (connectionItem == firstConnection || newConnectionEnd > connectionRangeEnd)
                {
                    previousConnection = connectionItem;
                    connectionRangeEnd = newConnectionEnd;
                    previousConnectionBaseAddress = connectionBaseAddress;
                    previousConnectionLow = connectionItem->getSceneEndPoint();
                    connectionHighPoint = connectionItem->sceneBoundingRect().top();

                    if (connectionItem != firstConnection && connectionHighPoint > connectionLowPoint)
                    {
                        intervalBetweenConnections = connectionHighPoint - connectionLowPoint + 1;
                    }
                    else
                    {
                        intervalBetweenConnections = 0;
                    }

                    if (connectionItem != firstConnection && connectionHighPoint < connectionLowPoint)
                    {
                        quint64 connectionLastAddress = connectionItem->getRangeEndValue().toULongLong(0, 16);
                        if (subItemLastAddress >= connectionLastAddress)
                        {
                            intervalBetweenConnections =
                                intervalBetweenConnections - (connectionLowPoint - connectionHighPoint) + 1;

                            yTransfer -= MemoryDesignerConstants::RANGEINTERVAL * 2;
                        }
                    }

                    quint64 subItemConnectionHeight =
                        getSubItemHeightForConnection(this, subItemBaseAddress, subItemLastAddress, subItem,
                        connectionItem, yPosition, newSubItemHeight, minimumSubItemHeight);

                    newSubItemHeight += subItemConnectionHeight + intervalBetweenConnections;

                    connectionLowPoint = connectionItem->sceneBoundingRect().bottom();
                }
            }
        }
    }

    return newSubItemHeight;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getConnectionEndPoint()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGraphicsItem::getConnectionRangeEndValue(MemoryConnectionItem* connectionItem) const
{
    quint64 newConnectionEnd = 0;

    qreal connectionHeight = connectionItem->boundingRect().height();
    qreal endItemHeight = connectionItem->getConnectionEndItem()->boundingRect().height();
    if (connectionHeight == endItemHeight)
    {
        newConnectionEnd = connectionItem->getRangeEndValue().toULongLong(0, 16);
    }
    else
    {
        newConnectionEnd = connectionItem->getConnectedEndItemLastAddress();
    }

    return newConnectionEnd;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::moveConnectionItem()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::moveConnectionItem(MemoryConnectionItem* connectionItem, qreal yTransfer,
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    if (!movedConnections->contains(connectionItem))
    {
        connectionItem->compressEndItem(movedConnections);

        connectionItem->onMoveConnectionInY(this, yTransfer);
        movedConnections->append(connectionItem);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getTransferY()
//-----------------------------------------------------------------------------
qreal AddressSpaceGraphicsItem::getTransferY(quint64 currentConnectionBaseAddress,
    quint64 previousConnectionBaseAddress, quint64 previousConnectionRangeEnd, quint64 previousConnectionLow,
    MemoryConnectionItem* connectionItem, MemoryConnectionItem* previousConnection, qreal yTransfer) const
{
    qreal newYTransfer = yTransfer;

    if (currentConnectionBaseAddress == previousConnectionRangeEnd + 1)
    {
        newYTransfer = previousConnectionLow - connectionItem->sceneBoundingRect().top() - 0.5;
    }
    else if (currentConnectionBaseAddress > previousConnectionRangeEnd + 1)
    {
        newYTransfer = previousConnectionLow - connectionItem->sceneBoundingRect().top() +
            MemoryDesignerConstants::RANGEINTERVAL;
    }
    else if (currentConnectionBaseAddress <= previousConnectionRangeEnd &&
        currentConnectionBaseAddress > previousConnectionBaseAddress)
    {
        quint64 addressDifference = (previousConnectionRangeEnd - currentConnectionBaseAddress) + 1;
        qreal addressTransfer = addressDifference * MemoryDesignerConstants::RANGEINTERVAL;

        MainMemoryGraphicsItem* previousEndItem = previousConnection->getConnectionEndItem();
        if (previousEndItem->sceneBoundingRect().bottom() < previousConnectionLow)
        {
            qreal previousExtensionHeight = previousConnectionLow - previousEndItem->sceneBoundingRect().bottom();
            addressTransfer = qMin(addressTransfer, previousExtensionHeight);
        }

        newYTransfer = previousConnectionLow - connectionItem->sceneBoundingRect().top() - addressTransfer;

        qreal maximumTransfer = previousConnection->sceneBoundingRect().top() -
            connectionItem->sceneBoundingRect().top() + MemoryDesignerConstants::RANGEINTERVAL;
        newYTransfer = qMax(newYTransfer, maximumTransfer);
    }

    return newYTransfer;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getMinimumRequiredHeight()
//-----------------------------------------------------------------------------
qreal AddressSpaceGraphicsItem::getMinimumRequiredHeight(quint64 connectionBaseAddress,
    quint64 connectionEndAddress) const
{
    qreal height = boundingRect().height();

    return SubMemoryLayout::getMinimumRequiredHeight(AddressSpaceItemConstants::MINIMUMSUBITEMHEIGHT,
        connectionBaseAddress, connectionEndAddress, getBaseAddress(), getLastAddress(), height);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getFilteredCompressedHeight()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGraphicsItem::getFilteredCompressedHeight(SubMemoryLayout*, quint64,
    qreal minimumSubItemHeight)
{
    MemoryConnectionItem* firstConnection = getMemoryConnections().first();
    quint64 connectionRangeEnd = firstConnection->getRangeEndValue().toULongLong(0, 16);
    qreal yTransfer = (sceneBoundingRect().top()) - firstConnection->sceneBoundingRect().top();
    quint64 previousConnectionLow = firstConnection->sceneBoundingRect().bottom();

    quint64 itemLastAddress = getLastAddress();
    
    quint64 connectionHighPoint = firstConnection->sceneBoundingRect().top();
    quint64 connectionLowPoint = sceneBoundingRect().top() + minimumSubItemHeight;

    quint64 connectionsTopY = connectionHighPoint;
    quint64 connectionsLowY = connectionLowPoint;

    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections (new QVector<MemoryConnectionItem*>());

    quint64 previousConnectionBaseAddress = firstConnection->getRangeStartValue().toULongLong(0, 16);
    MemoryConnectionItem* previousConnection = firstConnection;

    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        if (connectionItem->getConnectionStartItem() == this)
        {
            quint64 connectionBaseAddress = connectionItem->getRangeStartValue().toULongLong(0, 16);

            yTransfer = getFilteredConnectionYTransfer(connectionItem, connectionBaseAddress, previousConnection,
                previousConnectionBaseAddress, connectionRangeEnd, previousConnectionLow, yTransfer);

            moveConnectionItem(connectionItem, yTransfer, movedConnections);

            quint64 newConnectionEnd = getConnectionRangeEndValue(connectionItem);
            if (connectionItem == firstConnection || newConnectionEnd > connectionRangeEnd)
            {
                connectionRangeEnd = newConnectionEnd;
                previousConnection = connectionItem;
                previousConnectionBaseAddress = connectionBaseAddress;

                previousConnectionLow = getFilteredPreviousConnectionLow(connectionItem);

                connectionHighPoint = connectionItem->sceneBoundingRect().top();

                quint64 connectionLastAddress = connectionItem->getRangeEndValue().toULongLong(0, 16);
                quint64 connectionBaseAddress = connectionItem->getRangeStartValue().toULongLong(0, 16);
                if (connectionItem != firstConnection && connectionHighPoint < connectionLowPoint)
                {
                    if (itemLastAddress >= connectionLastAddress)
                    {
                        yTransfer -= MemoryDesignerConstants::RANGEINTERVAL * 2;
                    }
                }

                connectionLowPoint = connectionItem->sceneBoundingRect().bottom();

                if (connectionHighPoint < connectionsTopY)
                {
                    connectionsTopY = connectionHighPoint;
                }
                if (connectionLowPoint > connectionsLowY)
                {
                    connectionsLowY = connectionLowPoint;
                    if (connectionLastAddress > itemLastAddress + connectionBaseAddress)
                    {
                        connectionsLowY -= MemoryDesignerConstants::RANGEINTERVAL * 2;
                    }
                }
            }
        }
        else
        {
            connectionsLowY = connectionItem->sceneBoundingRect().bottom();
        }
    }

    qreal newItemHeight = connectionsLowY - connectionsTopY - pen().width();
    newItemHeight = qMax(newItemHeight, minimumSubItemHeight);

    return newItemHeight;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getFilteredConnectionYTransfer()
//-----------------------------------------------------------------------------
qreal AddressSpaceGraphicsItem::getFilteredConnectionYTransfer(MemoryConnectionItem* connectionItem,
    quint64 connectionBaseAddress, MemoryConnectionItem* previousConnection, quint64 previousBaseAddress,
    quint64 connectionRangeEnd, quint64 previousConnectionLow, qreal oldTransferY) const
{
    qreal newTransferY = 0;

    if (connectionItem->endItemIsMemoryMap())
    {
        newTransferY = getTransferY(connectionBaseAddress, previousBaseAddress, connectionRangeEnd,
            previousConnectionLow, connectionItem, previousConnection, oldTransferY);
    }
    else if (connectionBaseAddress >= connectionRangeEnd + 1)
    {
        newTransferY = previousConnectionLow - connectionItem->sceneBoundingRect().top() - 0.5;
    }

    return newTransferY;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getFilteredPreviousConnectionLow()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGraphicsItem::getFilteredPreviousConnectionLow(MemoryConnectionItem* connectionItem) const
{
    quint64 previousConnectionLow = 0;

    if (connectionItem->endItemIsMemoryMap())
    {
        previousConnectionLow = connectionItem->getSceneEndPoint();
    }
    else
    {
        previousConnectionLow = connectionItem->sceneBoundingRect().bottom();
    }

    return previousConnectionLow;
}
