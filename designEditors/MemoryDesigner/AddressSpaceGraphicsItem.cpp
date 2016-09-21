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

#include <IPXACTmodels/Component/AddressSpace.h>

#include <common/KactusColors.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/ConnectivityComponent.h>
#include <designEditors/MemoryDesigner/AddressSegmentGraphicsItem.h>

#include <QBrush>
#include <QFont>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::AddressSpaceGraphicsItem()
//-----------------------------------------------------------------------------
AddressSpaceGraphicsItem::AddressSpaceGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
    QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent):
MainMemoryGraphicsItem(memoryItem->getName(), containingInstance->getName(), memoryItem->getAUB(), parent),
cpuIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/compile.png"), this)),
segments_()
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

    setupSegments(memoryItem);
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
// Function: AddressSpaceGraphicsItem::setupSegments()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::setupSegments(QSharedPointer<MemoryItem> spaceItem)
{
    qreal segmentXPosition = - boundingRect().width() / 4;

    if (!spaceItem->getChildItems().isEmpty())
    {
        qreal segmentTransferY = 0;

        QMap<quint64, AddressSegmentGraphicsItem*> segmentsInOrder;

        foreach (QSharedPointer<MemoryItem> segmentItem, spaceItem->getChildItems())
        {
            if (segmentItem->getType().compare("segment", Qt::CaseInsensitive) == 0)
            {
                AddressSegmentGraphicsItem* newSegment = new AddressSegmentGraphicsItem(segmentItem, false, this);

                quint64 offset = segmentItem->getOffset().toULongLong();

                segmentTransferY = offset * GridSize * 1.5;

                newSegment->setPos(segmentXPosition, segmentTransferY);

                segmentsInOrder.insert(offset, newSegment);

                segments_.append(newSegment);
            }
        }

        if (!segmentsInOrder.isEmpty())
        {
            if (segmentsInOrder.firstKey() > 0)
            {
                createEmptySegment(segmentXPosition, 0, segmentsInOrder.firstKey() - 1);
            }

            QMapIterator<quint64, AddressSegmentGraphicsItem*> segmentIterator(segmentsInOrder);

            quint64 currentOffset = 0;
            quint64 segmentLastAddress = 0;

            while (segmentIterator.hasNext())
            {
                segmentIterator.next();

                currentOffset = segmentIterator.key();
                if (currentOffset > segmentLastAddress + 1)
                {
                    createEmptySegment(segmentXPosition, segmentLastAddress + 1, currentOffset - 1);
                }

                quint64 rangeEnd = segmentIterator.value()->getLastAddress();

                segmentLastAddress = qMax(segmentLastAddress, rangeEnd);
            }

            if (segmentLastAddress != getLastAddress())
            {
                createEmptySegment(segmentXPosition, segmentLastAddress + 1, getLastAddress());
            }
        }
    }
    else
    {
        createEmptySegment(segmentXPosition, 0, getLastAddress());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::createEmptySegment()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::createEmptySegment(qreal segmentXPosition, quint64 beginAddress, quint64 rangeEnd)
{
    quint64 emptySegmentOffsetInt = beginAddress;
    quint64 emptySegmentRangeInt = rangeEnd - emptySegmentOffsetInt + 1;

    QString emptySegmentOffset = QString::number(emptySegmentOffsetInt);
    QString emptySegmentRange = QString::number(emptySegmentRangeInt);

    QSharedPointer<MemoryItem> emptySegment (new MemoryItem("Empty", "segment"));
    emptySegment->setOffset(emptySegmentOffset);
    emptySegment->setRange(emptySegmentRange);

    AddressSegmentGraphicsItem* newSegment = new AddressSegmentGraphicsItem(emptySegment, true, this);

    quint64 segmentTransferY = emptySegmentOffset.toULongLong() * GridSize * 1.5;
    newSegment->setPos(segmentXPosition, segmentTransferY);

    segments_.append(newSegment);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::changeChildItemRanges()
//-----------------------------------------------------------------------------
void AddressSpaceGraphicsItem::changeChildItemRanges(quint64 offset)
{
    foreach (AddressSegmentGraphicsItem* segmentItem, segments_)
    {
        segmentItem->changeAddressRange(offset);
    }
}
