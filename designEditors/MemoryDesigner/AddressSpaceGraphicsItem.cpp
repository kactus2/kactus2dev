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

#include <QBrush>
#include <QFont>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: AddressSpaceGraphicsItem::AddressSpaceGraphicsItem()
//-----------------------------------------------------------------------------
AddressSpaceGraphicsItem::AddressSpaceGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
    QSharedPointer<ConnectivityComponent> containingInstance, QGraphicsItem* parent):
MainMemoryGraphicsItem(memoryItem->getName(), containingInstance->getName(), memoryItem->getAUB(), parent),
SubMemoryLayout(memoryItem, "segment", this),
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

    QSharedPointer<MemoryItem> emptySegment (new MemoryItem("Empty", "segment"));
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
