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
AddressSegmentGraphicsItem::AddressSegmentGraphicsItem(QSharedPointer<MemoryItem const> segmentItem,
    bool isEmptySegment, QVector<QString> identifierChain,
    QSharedPointer<ConnectivityComponent const> containingInstance, AddressSpaceGraphicsItem* spaceItem):
MemoryDesignerChildGraphicsItem(segmentItem, QStringLiteral("Segment"), segmentItem->getOffset().toULongLong(),
    segmentItem->getRange().toULongLong(), getSegmentItemWidth(spaceItem), identifierChain, containingInstance,
    spaceItem)
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
    QRectF itemRectangle = boundingRect();
    QRectF nameRectange = getNameLabel()->boundingRect();

    qreal nameX = itemRectangle.right() - nameRectange.width();
    qreal nameY = (itemRectangle.height() / 2) - (nameRectange.height() / 2);

    getNameLabel()->setPos(nameX, nameY);

    qreal rangeEndX = itemRectangle.bottomLeft().x();
    qreal rangeEndY = itemRectangle.bottomLeft().y() - getRangeEndLabel()->boundingRect().height();

    getRangeStartLabel()->setPos(itemRectangle.topLeft());
    if (!getRangeStartLabel()->isVisible())
    {
        rangeEndY += 2;
    }
    getRangeEndLabel()->setPos(rangeEndX, rangeEndY);
}
