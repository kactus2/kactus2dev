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

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::AddressSpaceGraphicsItem()
//-----------------------------------------------------------------------------
AddressSpaceGraphicsItem::AddressSpaceGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
    QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent):
MainMemoryGraphicsItem(memoryItem->getName(), containingInstance->getName(), memoryItem->getAUB(), parent),
SubMemoryLayout(memoryItem, MemoryDesignerConstants::ADDRESSSEGMENT_TYPE, this),
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
// Function: AddressSpaceGraphicsItem::changeChildItemRanges()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::changeChildItemRanges(quint64 offset)
{
    SubMemoryLayout::changeChildItemRanges(offset);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::condenseSpaceItem()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::condenseItemAndChildItems()
{
    qreal minimumSubItemHeight = 2 * MemoryDesignerConstants::RANGEINTERVAL;
    quint64 spaceNewHeight = 0;

    if (getMemoryConnections().isEmpty())
    {
        spaceNewHeight = condenseChildItems(minimumSubItemHeight);
    }
    else
    {
        spaceNewHeight = condenseSpaceSegments(minimumSubItemHeight);
    }

    condense(spaceNewHeight);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::condenseSpaceSegments()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGraphicsItem::condenseSpaceSegments(qreal minimumSubItemHeight)
{
    quint64 spaceNewHeight = 0;

    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections (new QVector<MemoryConnectionItem*>());

    QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();

        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
        QMap<quint64, MemoryConnectionItem*> segmentConnections =
            getSubItemConnections(subItem, getMemoryConnections());

        if (segmentConnections.isEmpty())
        {
            spaceNewHeight = condenseSubItem(subItem, minimumSubItemHeight, spaceNewHeight);
        }
        else
        {
            spaceNewHeight += condenseSegmentWithConnections(subItem, spaceNewHeight, segmentConnections, movedConnections);
        }
    }

    return spaceNewHeight;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::condenseSegmentWithConnections()
//-----------------------------------------------------------------------------
quint64 AddressSpaceGraphicsItem::condenseSegmentWithConnections(MemoryDesignerChildGraphicsItem* subItem,
    quint64 positionY, QMap<quint64, MemoryConnectionItem*> segmentConnections,
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    subItem->setPos(subItem->x(), positionY);

    qreal segmentConnectionsEnd = 0;

    quint64 currentConnectionBaseAddress = segmentConnections.firstKey();

    MemoryConnectionItem* firstConnection = segmentConnections.first();
    qreal yTransfer = subItem->sceneBoundingRect().top() - firstConnection->sceneBoundingRect().top();

    moveConnectionItem(firstConnection, yTransfer, movedConnections);

    quint64 connectionEnd = firstConnection->sceneBoundingRect().bottom();
    if (connectionEnd > segmentConnectionsEnd)
    {
        segmentConnectionsEnd = connectionEnd;
    }

    QMapIterator<quint64, MemoryConnectionItem*> segmentConnectionIterator(segmentConnections);

    quint64 previousConnectionLow = firstConnection->sceneBoundingRect().bottom();
    bool temporary = true;
    quint64 connectionRangeEnd = firstConnection->getRangeEndValue().toULongLong(&temporary, 16);

    while(segmentConnectionIterator.hasNext())
    {
        segmentConnectionIterator.next();

        MemoryConnectionItem* connectionItem = segmentConnectionIterator.value();
        if (connectionItem != firstConnection)
        {
            currentConnectionBaseAddress = segmentConnectionIterator.key();
            if (currentConnectionBaseAddress > connectionRangeEnd + 5)
            {
                yTransfer = previousConnectionLow - connectionItem->sceneBoundingRect().top() + GridSize;
            }

            moveConnectionItem(connectionItem, yTransfer, movedConnections);

            connectionRangeEnd = connectionItem->getConnectedEndItemLastAddress();

            previousConnectionLow = connectionItem->getSceneEndPoint();

            quint64 connectionEnd = connectionItem->sceneBoundingRect().bottom();
            if (connectionEnd > segmentConnectionsEnd)
            {
                segmentConnectionsEnd = connectionEnd;
            }
        }
    }

    quint64 subItemLow = subItem->sceneBoundingRect().bottom();
    if (subItemLow > segmentConnectionsEnd)
    {
        quint64 subItemNewHeight = segmentConnectionsEnd - subItem->sceneBoundingRect().top();
        subItem->condense(subItemNewHeight);
    }

    return subItem->boundingRect().bottom();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::moveConnectionItem()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::moveConnectionItem(MemoryConnectionItem* connectionItem, qreal yTransfer,
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    if (!movedConnections->contains(connectionItem))
    {
        connectionItem->condenseEndItemToConnection();

        connectionItem->onMoveConnectionInY(this, yTransfer);
        movedConnections->append(connectionItem);
    }
}
