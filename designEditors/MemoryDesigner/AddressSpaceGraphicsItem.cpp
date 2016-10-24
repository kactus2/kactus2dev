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

#include <QBrush>
#include <QFont>
#include <QPen>

namespace AddressSpaceItemConstants
{
    const int MINMUMSUBITEMHEIGHT = 2 * MemoryDesignerConstants::RANGEINTERVAL;
};

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::AddressSpaceGraphicsItem()
//-----------------------------------------------------------------------------
AddressSpaceGraphicsItem::AddressSpaceGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
    QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent):
MainMemoryGraphicsItem(
    memoryItem, containingInstance->getName(), MemoryDesignerConstants::ADDRESSSEGMENT_TYPE, parent),
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
    setupSubItems(segmentPositionX);
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
void AddressSpaceGraphicsItem::condenseItemAndChildItems()
{
    if (!isCompressed())
    {
        quint64 spaceNewHeight = 0;

        if (getMemoryConnections().isEmpty())
        {
            spaceNewHeight = condenseChildItems(AddressSpaceItemConstants::MINMUMSUBITEMHEIGHT);
        }
        else
        {
            spaceNewHeight = getCompressedHeight(AddressSpaceItemConstants::MINMUMSUBITEMHEIGHT, this);
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

        foreach (MemoryConnectionItem* connectionItem, subItem->getMemoryConnections())
        {
            quint64 connectionBaseAddress = connectionItem->getRangeStartValue().toULongLong(0, 16);

            yTransfer = getTransferY(connectionBaseAddress, connectionRangeEnd, previousConnectionLow,
                connectionItem, yTransfer);

            moveConnectionItem(connectionItem, yTransfer, movedConnections);

            quint64 newConnectionEnd = connectionItem->getConnectedEndItemLastAddress();
            if (connectionItem == firstConnection || newConnectionEnd > connectionRangeEnd)
            {
                connectionRangeEnd = newConnectionEnd;
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

                quint64 subItemConnectionHeight = getSubItemHeightForConnection(this, subItemBaseAddress,
                    subItemLastAddress, subItem, connectionItem, yPosition, newSubItemHeight, minimumSubItemHeight);

                newSubItemHeight += subItemConnectionHeight + intervalBetweenConnections;

                connectionLowPoint = connectionItem->sceneBoundingRect().bottom();
            }
        }
    }

    return newSubItemHeight;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::moveConnectionItem()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::moveConnectionItem(MemoryConnectionItem* connectionItem, qreal yTransfer,
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    if (!movedConnections->contains(connectionItem))
    {
        connectionItem->compressEndItem();

        connectionItem->onMoveConnectionInY(this, yTransfer);
        movedConnections->append(connectionItem);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getTransferY()
//-----------------------------------------------------------------------------
qreal AddressSpaceGraphicsItem::getTransferY(quint64 currentConnectionBaseAddress, quint64 connectionRangeEnd,
    quint64 previousConnectionLow, MemoryConnectionItem* connectionItem, qreal yTransfer) const
{
    qreal newYTransfer = yTransfer;

    if (currentConnectionBaseAddress == connectionRangeEnd + 1)
    {
        newYTransfer = previousConnectionLow - connectionItem->sceneBoundingRect().top() - 0.5;
    }
    else if (currentConnectionBaseAddress > connectionRangeEnd + 1)
    {
        newYTransfer = previousConnectionLow - connectionItem->sceneBoundingRect().top() + GridSize;
    }

    return newYTransfer;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::getMinimumRequiredHeight()
//-----------------------------------------------------------------------------
qreal AddressSpaceGraphicsItem::getMinimumRequiredHeight(quint64 connectionBaseAddress,
    quint64 connectionEndAddress) const
{
    return SubMemoryLayout::getMinimumRequiredHeight(AddressSpaceItemConstants::MINMUMSUBITEMHEIGHT,
        connectionBaseAddress, connectionEndAddress);
}
