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

#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryColumn.h>

#include <QFont>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: MemoryCollisionItem::MemoryCollisionItem()
//-----------------------------------------------------------------------------
MemoryCollisionItem::MemoryCollisionItem(MemoryConnectionItem* firstItem, MemoryConnectionItem* secondItem,
    QGraphicsScene* containingScene):
QObject(),
QGraphicsRectItem()
{
    const qreal LINEWIDTH = 1;

    QRectF firstItemRect = firstItem->sceneBoundingRect();
    QRectF secondItemRect = secondItem->sceneBoundingRect();

    qreal startX = qMax(firstItemRect.left(), secondItemRect.left());
    qreal startY = qMax(firstItemRect.top(), secondItemRect.top()) + LINEWIDTH / 2;
    qreal endX = qMax(firstItemRect.right(), secondItemRect.right());
    qreal endY = qMin(firstItemRect.bottom(), secondItemRect.bottom() + LINEWIDTH / 2);

    qreal itemWidth = endX - startX;
    qreal itemHeight = endY - startY;

    QPointF spaceItemStartPoint (startX, startY);

    setRect(spaceItemStartPoint.x(), spaceItemStartPoint.y(), itemWidth, itemHeight - 1);

    containingScene->addItem(this);

    setCollisionBrush();

    setLabels(firstItem->getRangeStartValue(), firstItem->getRangeEndValue(), secondItem->getRangeStartValue(),
        secondItem->getRangeEndValue());
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
    qreal collisionOpacity = 0.45;

    QBrush collisionBrush(KactusColors::MISSING_COMPONENT);
    setBrush(collisionBrush);
    setOpacity(collisionOpacity);

    QVector<MemoryCollisionItem*> collidingCollisionItems;
    foreach (QGraphicsItem* collidingItem, collidingItems())
    {
        MemoryCollisionItem* memoryCollisionItem = dynamic_cast<MemoryCollisionItem*>(collidingItem);
        if (memoryCollisionItem && memoryCollisionItem != this)
        {
            collidingCollisionItems.append(memoryCollisionItem);
        }
    }

    int collisionMarker = collidingCollisionItems.size();
    foreach (MemoryCollisionItem* collidingItem, collidingCollisionItems)
    {
        if (collidingItem->boundingRect().top() == boundingRect().top() &&
            collidingItem->boundingRect().bottom() == boundingRect().bottom())
        {
            collisionMarker = collisionMarker - 1;
            collisionOpacity = collisionOpacity / (collidingCollisionItems.size() - collisionMarker);
            collidingItem->setOpacity(collisionOpacity);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryCollisionItem::setLabels()
//-----------------------------------------------------------------------------
void MemoryCollisionItem::setLabels(QString firstRangeStart, QString firstRangeEnd, QString secondRangeStart,
    QString secondRangeEnd)
{
    QPointF topLeft = boundingRect().topLeft();
    QPointF lowLeft = boundingRect().bottomLeft();

    bool transformOk = true;
    int firstStartInInt = firstRangeStart.toInt(&transformOk, 16);
    int secondStartInInt = secondRangeStart.toInt(&transformOk, 16);
    QString collisionRangeStart = firstRangeStart;
    if (secondStartInInt > firstStartInInt)
    {
        collisionRangeStart = secondRangeStart;
    }

    int firstEndInInt = firstRangeEnd.toInt(&transformOk, 16);
    int secondEndInInt = secondRangeEnd.toInt(&transformOk, 16);
    QString collisionRangeEnd = firstRangeEnd;
    if (secondEndInInt < firstEndInInt)
    {
        collisionRangeEnd = secondRangeEnd;
    }

    QGraphicsTextItem* startPointRangeStart = new QGraphicsTextItem(collisionRangeStart, this);

    QFont labelFont = startPointRangeStart->font();
    labelFont.setWeight(QFont::Bold);
    startPointRangeStart->setFont(labelFont);

    QColor faultyTextColor(255, 0, 0);
    startPointRangeStart->setDefaultTextColor(faultyTextColor);

    int rangeStartInt = collisionRangeStart.toInt(&transformOk, 16);
    int rangeEndInt = collisionRangeEnd.toInt(&transformOk, 16);
    
    int startPointYCorrection = -3;
    if (rangeEndInt - rangeStartInt > 0)
    {
        QGraphicsTextItem* startPointRangeEnd = new QGraphicsTextItem(collisionRangeEnd, this);
        startPointRangeEnd->setPos(lowLeft.x() + 2, lowLeft.y() - startPointRangeEnd->boundingRect().height() + 1);
        startPointRangeEnd->setFont(labelFont);
        startPointRangeEnd->setDefaultTextColor(faultyTextColor);

        startPointYCorrection = 0;
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

                QGraphicsTextItem* collidingMapRangeStart = new QGraphicsTextItem(collisionRangeStart, this);
                collidingMapRangeStart->setDefaultTextColor(faultyTextColor);
                collidingMapRangeStart->setFont(labelFont);
                collidingMapRangeStart->setPos(
                    mapTopLeft.x() - (collidingMapRangeStart->boundingRect().width() + 2),
                    topLeft.y() + startPointYCorrection);

                if (rangeEndInt - rangeStartInt > 0)
                {
                    QGraphicsTextItem* collidingMapRangeEnd = new QGraphicsTextItem(collisionRangeEnd, this);
                    collidingMapRangeEnd->setDefaultTextColor(faultyTextColor);
                    collidingMapRangeEnd->setFont(labelFont);
                    collidingMapRangeEnd->setPos(
                        mapTopLeft.x() - (collidingMapRangeEnd->boundingRect().width() + 2),
                        lowLeft.y() - collidingMapRangeEnd->boundingRect().height() + 1);
                }

                collidingColumns.append(containingColumn);
            }
        }
    }
}
