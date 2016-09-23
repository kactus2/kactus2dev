//-----------------------------------------------------------------------------
// File: AddressSegmentGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.09.2016
//
// Description:
// Graphics item for visualizing an address segment in the memory designer.
//-----------------------------------------------------------------------------

#include "AddressSegmentGraphicsItem.h"

#include <common/KactusColors.h>

#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/AddressSpaceGraphicsItem.h>

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: AddressSegmentGraphicsItem::AddressSegmentGraphicsItem()
//-----------------------------------------------------------------------------
AddressSegmentGraphicsItem::AddressSegmentGraphicsItem(QSharedPointer<MemoryItem> segmentItem, bool isEmptySegment,
    AddressSpaceGraphicsItem* spaceItem):
MemoryDesignerChildGraphicsItem(segmentItem->getName(), "segment", segmentItem->getOffset().toULongLong(),
    segmentItem->getRange().toULongLong(), getSegmentItemWidth(spaceItem), spaceItem)
{
    setColors(KactusColors::ADDRESS_SEGMENT, isEmptySegment);
    setLabelPositions();
}

//-----------------------------------------------------------------------------
// Function: AddressSegmentGraphicsItem::~AddressSegmentGraphicsItem()
//-----------------------------------------------------------------------------
AddressSegmentGraphicsItem::~AddressSegmentGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSegmentGraphicsItem::getSegmentItemWidth()
//-----------------------------------------------------------------------------
qreal AddressSegmentGraphicsItem::getSegmentItemWidth(AddressSpaceGraphicsItem* addressSpaceItem) const
{
    qreal segmentWidth = addressSpaceItem->boundingRect().width() / 2;
    return segmentWidth;
}

//-----------------------------------------------------------------------------
// Function: AddressSegmentGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void AddressSegmentGraphicsItem::setLabelPositions()
{
    qreal nameX = boundingRect().right() - getNameLabel()->boundingRect().width();
    qreal nameY = (boundingRect().height() / 2) - (getNameLabel()->boundingRect().height() / 2);

    getNameLabel()->setPos(nameX, nameY);

    qreal rangeEndX = boundingRect().bottomLeft().x();
    qreal rangeEndY = boundingRect().bottomLeft().y() - getRangeEndLabel()->boundingRect().height();

    getRangeStartLabel()->setPos(boundingRect().topLeft());
    if (!getRangeStartLabel()->isVisible())
    {
        rangeEndY += 2;
    }
    getRangeEndLabel()->setPos(rangeEndX, rangeEndY);
}
