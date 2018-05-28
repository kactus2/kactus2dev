//-----------------------------------------------------------------------------
// File: MemoryCollisionItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.09.2016
//
// Description:
// Graphics item for displaying colliding memory usage in memory designer.
//-----------------------------------------------------------------------------

#include "MemoryCollisionItem.h"

#include <common/KactusColors.h>

#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryConnectionItem.h>
#include <editors/MemoryDesigner/MemoryDesignerGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryColumn.h>

#include <QFont>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: MemoryCollisionItem::MemoryCollisionItem()
//-----------------------------------------------------------------------------
MemoryCollisionItem::MemoryCollisionItem(MemoryConnectionItem* firstItem, MemoryConnectionItem* secondItem,
    QGraphicsScene* containingScene):
QObject(),
QGraphicsRectItem(),
firstConnection_(firstItem),
secondConnection_(secondItem),
firstRangeStart_(firstItem->getRangeStartValue()),
firstRangeEnd_(firstItem->getRangeEndValue()),
secondRangeStart_(secondItem->getRangeStartValue()),
secondRangeEnd_(secondItem->getRangeEndValue())
{
    setRectangle();

    containingScene->addItem(this);

    setCollisionBrush();

    setLabels();
}

//-----------------------------------------------------------------------------
// Function: MemoryCollisionItem::~MemoryCollisionItem()
//-----------------------------------------------------------------------------
MemoryCollisionItem::~MemoryCollisionItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryCollisionItem::setCollisionBrush()
//-----------------------------------------------------------------------------
void MemoryCollisionItem::setCollisionBrush()
{
    qreal mainCollisionOpacity = 0.4;

    QVector<MemoryCollisionItem*> collidingCollisionItems;
    foreach (QGraphicsItem* collidingItem, collidingItems())
    {
        MemoryCollisionItem* memoryCollisionItem = dynamic_cast<MemoryCollisionItem*>(collidingItem);
        if (memoryCollisionItem && memoryCollisionItem != this)
        {
            collidingCollisionItems.append(memoryCollisionItem);
        }
    }

    if (!collidingCollisionItems.isEmpty())
    {
        collidingCollisionItems.removeFirst();

        mainCollisionOpacity = 0.3;
        qreal collisionOpacity = mainCollisionOpacity;

        int collisionMarker = collidingCollisionItems.size();
        for (int collisionIndex = collidingCollisionItems.size() - 1; collisionIndex >= 0; --collisionIndex)
        {
            MemoryCollisionItem* collidingItem = collidingCollisionItems.at(collisionIndex);

            if (collidingItem->boundingRect().top() == boundingRect().top() &&
                collidingItem->boundingRect().bottom() == boundingRect().bottom())
            {
                collisionMarker = collisionMarker - 1;
                collisionOpacity = collisionOpacity / (collidingCollisionItems.size() - collisionMarker);
                collidingItem->setOpacity(collisionOpacity);
            }
        }
    }

    QBrush collisionBrush(KactusColors::MISSING_COMPONENT);
    setBrush(collisionBrush);
    setOpacity(mainCollisionOpacity);
}

//-----------------------------------------------------------------------------
// Function: MemoryCollisionItem::setLabels()
//-----------------------------------------------------------------------------
void MemoryCollisionItem::setLabels()
{
    QPointF topLeft = boundingRect().topLeft();
    QPointF lowLeft = boundingRect().bottomLeft();

    quint64 collisionRangeStart = qMax(firstRangeStart_, secondRangeStart_);
    quint64 collisionRangeEnd = qMin(firstRangeEnd_, secondRangeEnd_);

    QString collisionBaseInHexa = QString::number(collisionRangeStart, 16).toUpper();
    QString collisionLastInHexa = QString::number(collisionRangeEnd, 16).toUpper();

    int amountOfNumbers =
        MemoryDesignerConstants::getAmountOfNumbersInRange(collisionBaseInHexa, collisionLastInHexa);
    collisionBaseInHexa = MemoryDesignerConstants::getValueWithZerosAdded(collisionBaseInHexa, amountOfNumbers);
    collisionLastInHexa = MemoryDesignerConstants::getValueWithZerosAdded(collisionLastInHexa, amountOfNumbers);

    QGraphicsTextItem* startPointRangeStart = new QGraphicsTextItem(collisionBaseInHexa, this);

    QFont labelFont = startPointRangeStart->font();
    labelFont.setWeight(QFont::Bold);
    startPointRangeStart->setFont(labelFont);

    QColor faultyTextColor(255, 0, 0);
    startPointRangeStart->setDefaultTextColor(faultyTextColor);
    
    int startPointYCorrection = -3;
    qreal endPointYCorrection = 0;
    if (collisionRangeEnd - collisionRangeStart > 0)
    {
        QGraphicsTextItem* startPointRangeEnd = new QGraphicsTextItem(collisionLastInHexa, this);

        if (boundingRect().height() <= MemoryDesignerConstants::RANGEINTERVAL + 1)
        {
            startPointYCorrection = -startPointRangeStart->boundingRect().height() + 4;
            endPointYCorrection = 4;
        }
        else
        {
            startPointYCorrection = 0;
            endPointYCorrection = startPointRangeEnd->boundingRect().height() - 1;
        }

        startPointRangeEnd->setPos(lowLeft.x() + 2, lowLeft.y() - endPointYCorrection);
        startPointRangeEnd->setFont(labelFont);
        startPointRangeEnd->setDefaultTextColor(faultyTextColor);
    }

    startPointRangeStart->setPos(topLeft.x() + 2, topLeft.y() + startPointYCorrection);

    QVector<MemoryColumn*> collidingColumns;

    foreach (QGraphicsItem* collidingItem, collidingItems())
    {
        MemoryMapGraphicsItem* mapGraphicsItem = dynamic_cast<MemoryMapGraphicsItem*>(collidingItem);
        QGraphicsRectItem* extensionItem = dynamic_cast<QGraphicsRectItem*>(collidingItem);
        if (extensionItem && !mapGraphicsItem)
        {
            mapGraphicsItem = dynamic_cast<MemoryMapGraphicsItem*>(extensionItem->parentItem());
        }

        if (mapGraphicsItem)
        {
            QGraphicsItem* mapParentItem = mapGraphicsItem->parentItem();
            MemoryColumn* containingColumn = dynamic_cast<MemoryColumn*>(mapParentItem);
            if (containingColumn && !collidingColumns.contains(containingColumn))
            {
                QPointF mapTopLeft = mapFromItem(mapGraphicsItem, mapGraphicsItem->boundingRect().topLeft());

                QGraphicsTextItem* collidingMapRangeStart = new QGraphicsTextItem(collisionBaseInHexa, this);
                collidingMapRangeStart->setDefaultTextColor(faultyTextColor);
                collidingMapRangeStart->setFont(labelFont);
                collidingMapRangeStart->setPos(
                    mapTopLeft.x() - (collidingMapRangeStart->boundingRect().width() + 2),
                    topLeft.y() + startPointYCorrection);

                if (collisionRangeEnd - collisionRangeStart > 0)
                {
                    QGraphicsTextItem* collidingMapRangeEnd = new QGraphicsTextItem(collisionLastInHexa, this);
                    collidingMapRangeEnd->setDefaultTextColor(faultyTextColor);
                    collidingMapRangeEnd->setFont(labelFont);
                    collidingMapRangeEnd->setPos(
                        mapTopLeft.x() - (collidingMapRangeEnd->boundingRect().width() + 2),
                        lowLeft.y() - endPointYCorrection);
                }

                collidingColumns.append(containingColumn);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryCollisionItem::reDrawCollision()
//-----------------------------------------------------------------------------
void MemoryCollisionItem::reDrawCollision()
{
    setRectangle();

    int childItemCount = childItems().size();
    for (int childItemIndex = 0; childItemIndex < childItemCount; ++childItemIndex)
    {
        QGraphicsItem* childItem = childItems().takeFirst();
        delete childItem;
    }

    setLabels();
}

//-----------------------------------------------------------------------------
// Function: MemoryCollisionItem::setRectangle()
//-----------------------------------------------------------------------------
void MemoryCollisionItem::setRectangle()
{
    const qreal LINEWIDTH = 1;

    QRectF firstItemRectangle = firstConnection_->sceneBoundingRect();
    QRectF secondItemRectangle = secondConnection_->sceneBoundingRect();

    qreal startX = qMax(firstItemRectangle.left(), secondItemRectangle.left());
    qreal startY = qMax(firstItemRectangle.top(), secondItemRectangle.top()) + LINEWIDTH / 2;
    qreal endX = qMax(firstItemRectangle.right(), secondItemRectangle.right());
    qreal endY = qMin(firstItemRectangle.bottom(), secondItemRectangle.bottom() + LINEWIDTH / 2);

    qreal itemWidth = endX - startX;
    qreal itemHeight = endY - startY;

    QPointF spaceItemStartPoint (startX, startY);

    setRect(spaceItemStartPoint.x(), spaceItemStartPoint.y(), itemWidth, itemHeight - 1);
}