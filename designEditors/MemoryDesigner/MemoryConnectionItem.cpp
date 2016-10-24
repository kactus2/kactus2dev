//-----------------------------------------------------------------------------
// File: MemoryConnectionItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.08.2016
//
// Description:
// Memory designer connection item between address spaces and memory maps.
//-----------------------------------------------------------------------------

#include "MemoryConnectionItem.h"

#include <common/KactusColors.h>

#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryColumn.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/MemoryMapGraphicsItem.h>

#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::MemoryConnectionItem()
//-----------------------------------------------------------------------------
MemoryConnectionItem::MemoryConnectionItem(MainMemoryGraphicsItem* startItem, QString const& firstStartValue,
    QString const& firstEndValue, MainMemoryGraphicsItem* endItem, QString const& secondStartValue,
    QString const& secondEndValue, QGraphicsScene* containingScene, int yTransfer, QGraphicsItem* parent):
QGraphicsPathItem(parent),
amountOfNumbers_(getAmountOfNumbers(firstStartValue, firstEndValue)),
firstItemStartLabel_(new QGraphicsTextItem(formatValueToHexadecimal(firstStartValue), this)),
firstItemEndLabel_(new QGraphicsTextItem(formatValueToHexadecimal(firstEndValue), this)),
secondItemStartLabel_(new QGraphicsTextItem(formatValueToHexadecimal(secondStartValue), this)),
secondItemEndLabel_(new QGraphicsTextItem(formatValueToHexadecimal(secondEndValue), this)),
startItem_(startItem),
endItem_(endItem),
yTransfer_(yTransfer),
rangeStart_(firstStartValue),
rangeEnd_(firstEndValue),
connectionWidth_(getConnectionWidth())
{
    if (firstStartValue.compare(firstEndValue) == 0 && secondStartValue.compare(secondEndValue) == 0)
    {
        firstItemEndLabel_->hide();
        secondItemEndLabel_->hide();
    }

    if (rangeStart_.toInt() - rangeEnd_.toInt() == 1)
    {
        secondItemEndLabel_->hide();
    }

    containingScene->addItem(this);

    createPath();
    repositionLabels();

    startItem_->addMemoryConnection(this);
    endItem_->addMemoryConnection(this);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::~MemoryConnectionItem()
//-----------------------------------------------------------------------------
MemoryConnectionItem::~MemoryConnectionItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::drawPath()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::createPath()
{
    setPath(QPainterPath());

    const qreal LINEWIDTH = 1;
    qreal connectionWidth = connectionWidth_;

    QPointF startItemTopRight = startItem_->mapToScene(startItem_->boundingRect().topRight());
    QPointF endItemTopLeft = endItem_->mapToScene(endItem_->boundingRect().topLeft());

    QPointF highStartPoint(startItemTopRight.x(), startItemTopRight.y() + yTransfer_ + LINEWIDTH / 2);
    QPointF lowStartPoint(highStartPoint.x(), highStartPoint.y() + connectionWidth);
    QPointF highEndPoint(endItemTopLeft.x(), highStartPoint.y());
    QPointF lowEndPoint(highEndPoint.x(), lowStartPoint.y());

    QPainterPath path;
    path.moveTo(highStartPoint);
    path.lineTo(highEndPoint);
    path.moveTo(lowStartPoint);
    path.lineTo(lowEndPoint);

    QPen newPen = pen();
    newPen.setWidth(LINEWIDTH);
    setPen(newPen);

    setPath(path);
    avoidCollisionsOnPath(highStartPoint, highEndPoint, lowStartPoint, lowEndPoint);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getConnectionWidth()
//-----------------------------------------------------------------------------
qreal MemoryConnectionItem::getConnectionWidth() const
{
    qreal connectionWidth =
        (rangeEnd_.toULongLong() - rangeStart_.toULongLong() + 1) * MemoryDesignerConstants::RANGEINTERVAL;

    quint64 connectionBaseAddress = getRangeStartValue().toULongLong(0, 16);
    quint64 connectionLastAddress = getRangeEndValue().toULongLong(0, 16);

    qreal endItemHeight = endItem_->getMinimumRequiredHeight(connectionBaseAddress, connectionLastAddress);
    qreal startItemHeight = startItem_->getMinimumRequiredHeight(connectionBaseAddress, connectionLastAddress);

    qreal comparisonHeight = endItemHeight;
    if (startItemHeight > endItemHeight)
    {
        comparisonHeight = startItemHeight;
    }

    if (comparisonHeight < connectionWidth)
    {
        connectionWidth = comparisonHeight;

        quint64 connectionSize = connectionLastAddress - connectionBaseAddress;

        quint64 startLastAddress = startItem_->getLastAddress();
        quint64 startBaseAddress = startItem_->getBaseAddress();
        quint64 endLastAddress = endItem_->getLastAddress();
        quint64 endBaseAddress = endItem_->getBaseAddress();

        quint64 startItemSize = startLastAddress - startBaseAddress;
        quint64 endItemSize = endLastAddress - endBaseAddress;
        if (connectionSize > startItemSize || connectionSize > endItemSize)
        {
            connectionWidth += MemoryDesignerConstants::RANGEINTERVAL * 2;
        }
    }

    return connectionWidth;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::avoidCollisionsOnPath()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::avoidCollisionsOnPath(QPointF highStartPoint, QPointF highEndPoint,
    QPointF lowStartPoint, QPointF lowEndPoint)
{
    QList<QGraphicsItem*> collisions = collidingItems(Qt::IntersectsItemBoundingRect);
    if (!collisions.isEmpty())
    {
        QList<QPair<QPointF, QPointF> > highCollisionPoints;
        QList<QPair<QPointF, QPointF> > lowCollisionPoints;

        MemoryColumn* startColumn = dynamic_cast<MemoryColumn*>(startItem_->parentItem());
        MemoryColumn* endColumn = dynamic_cast<MemoryColumn*>(endItem_->parentItem());

        foreach (QGraphicsItem* collidingItem, collisions)
        {
            MemoryDesignerGraphicsItem* memoryItem = dynamic_cast<MemoryDesignerGraphicsItem*>(collidingItem);
            if (memoryItem && memoryItem != startItem_ && memoryItem != endItem_)
            {
                MemoryColumn* itemColumn = dynamic_cast<MemoryColumn*>(memoryItem->parentItem());
                if (itemColumn && startColumn && endColumn && itemColumn != startColumn && itemColumn != endColumn)
                {
                    QPointF highCollisionStart = memoryItem->mapToScene(memoryItem->boundingRect().topLeft());
                    if (highCollisionStart.y() < highStartPoint.y())
                    {
                        highCollisionStart.setY(highStartPoint.y());
                        QPointF highCollisionEnd = memoryItem->mapToScene(memoryItem->boundingRect().topRight());
                        highCollisionEnd.setY(highStartPoint.y());

                        QPair<QPointF, QPointF> collisionPoints(highCollisionStart, highCollisionEnd);
                        highCollisionPoints.append(collisionPoints);
                    }

                    QPointF lowCollisionStart = memoryItem->mapToScene(memoryItem->boundingRect().bottomLeft());
                    if (lowCollisionStart.y() > lowStartPoint.y())
                    {
                        lowCollisionStart.setY(lowStartPoint.y());
                        QPointF lowCollisionEnd = memoryItem->mapToScene(memoryItem->boundingRect().bottomRight());
                        lowCollisionEnd.setY(lowStartPoint.y());

                        QPair<QPointF, QPointF> collisionPoints(lowCollisionStart, lowCollisionEnd);
                        lowCollisionPoints.append(collisionPoints);
                    }
                }
            }
        }

        if (!highCollisionPoints.isEmpty() || !lowCollisionPoints.isEmpty())
        {
            QPainterPath collisionPath;

            QPointF highCollisionStartPoint = highStartPoint;
            for (int i = 0; i < highCollisionPoints.size(); ++i)
            {
                QPair<QPointF, QPointF> collisionPoint = highCollisionPoints.at(i);
                collisionPath.moveTo(highCollisionStartPoint);
                collisionPath.lineTo(collisionPoint.first);
                highCollisionStartPoint = collisionPoint.second;
            }

            collisionPath.moveTo(highCollisionStartPoint);
            collisionPath.lineTo(highEndPoint);

            QPointF lowCollisionStartPoint = lowStartPoint;
            for (int i = 0; i < lowCollisionPoints.size(); ++i)
            {
                QPair<QPointF, QPointF> collisionPoint = lowCollisionPoints.at(i);
                collisionPath.moveTo(lowCollisionStartPoint);
                collisionPath.lineTo(collisionPoint.first);
                lowCollisionStartPoint = collisionPoint.second;
            }

            collisionPath.moveTo(lowCollisionStartPoint);
            collisionPath.lineTo(lowEndPoint);

            setPath(collisionPath);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::setLabels()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::repositionLabels()
{
    QFont labelFont = firstItemStartLabel_->font();
    labelFont.setWeight(QFont::Bold);

    firstItemStartLabel_->setFont(labelFont);
    firstItemEndLabel_->setFont(labelFont);
    secondItemStartLabel_->setFont(labelFont);
    secondItemEndLabel_->setFont(labelFont);

    QPointF topLeft = boundingRect().topLeft();
    QPointF topRight = boundingRect().topRight();
    QPointF lowerLeft = boundingRect().bottomLeft();
    QPointF lowerRight = boundingRect().bottomRight();

    const int YCORRECTION = 20;

    firstItemStartLabel_->setPos(topLeft.x() - firstItemStartLabel_->boundingRect().width(), topLeft.y());
    firstItemEndLabel_->setPos(
        lowerLeft.x() - firstItemStartLabel_->boundingRect().width(), lowerLeft.y() - YCORRECTION);

    secondItemStartLabel_->setPos(topRight);
    secondItemEndLabel_->setPos(lowerRight.x(), lowerRight.y() - YCORRECTION);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getAmountOfNumbers()
//-----------------------------------------------------------------------------
int MemoryConnectionItem::getAmountOfNumbers(QString const& rangeStart, QString const& rangeEnd) const
{
    int startSize = rangeStart.size();
    int endSize = rangeEnd.size();

    int amountOfNumbers = endSize;
    if (startSize > endSize)
    {
        amountOfNumbers = startSize;
    }

    while (amountOfNumbers % 4)
    {
        amountOfNumbers++;
    }

    return amountOfNumbers;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::formatValueToHexadecimal()
//-----------------------------------------------------------------------------
QString MemoryConnectionItem::formatValueToHexadecimal(QString const& value) const
{
    quint64 intValue = value.toULongLong();

    QString formattedRange = QString::number(intValue, 16).toUpper();

    int rangeSize = formattedRange.size();

    int amountOfZeros = amountOfNumbers_ - rangeSize;
    while (amountOfZeros > 0)
    {
        formattedRange.prepend('0');
        amountOfZeros--;
    }

    return formattedRange;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::onMoveConnection()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::onMoveConnection(MainMemoryGraphicsItem* movementOrigin, QPointF movementDelta)
{
    MainMemoryGraphicsItem* connectedItem;

    if (movementOrigin == startItem_)
    {
        connectedItem = endItem_;
    }
    else if (movementOrigin == endItem_)
    {
        connectedItem = startItem_;
    }

    if (connectedItem)
    {
        connectedItem->moveByConnection(this, movementDelta);
    }

    createPath();
    repositionLabels();
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::onMoveConnectionInY()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::onMoveConnectionInY(MainMemoryGraphicsItem* movementOrigin, qreal yTransfer)
{
    moveBy(0, yTransfer);

    moveConnectedItem(movementOrigin, yTransfer);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::moveConnectedItem()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::moveConnectedItem(MainMemoryGraphicsItem* movementOrigin, qreal yTransfer)
{
    MainMemoryGraphicsItem* connectedItem;

    if (movementOrigin == startItem_)
    {
        connectedItem = endItem_;
    }
    else if (movementOrigin == endItem_)
    {
        connectedItem = startItem_;
    }

    if (connectedItem)
    {
        connectedItem->moveBy(0, yTransfer);

        reDrawConnection();

        foreach (MemoryConnectionItem* connectionItem, connectedItem->getMemoryConnections())
        {
            if (connectionItem != this)
            {
                connectionItem->moveConnectedItem(connectedItem, yTransfer);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getRangeStartValue()
//-----------------------------------------------------------------------------
QString MemoryConnectionItem::getRangeStartValue() const
{
    return formatValueToHexadecimal(rangeStart_);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getRangeEndValue()
//-----------------------------------------------------------------------------
QString MemoryConnectionItem::getRangeEndValue() const
{
    return formatValueToHexadecimal(rangeEnd_);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::reDrawConnection()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::reDrawConnection()
{
    createPath();
    repositionLabels();
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getSceneEndPoint()
//-----------------------------------------------------------------------------
quint64 MemoryConnectionItem::getSceneEndPoint() const
{
    quint64 sceneEndPoint = sceneBoundingRect().bottom();

    quint64 endItemSceneEndPoint = endItem_->sceneBoundingRect().bottom();

    if (endItemSceneEndPoint > sceneEndPoint)
    {
        sceneEndPoint = endItemSceneEndPoint;
    }

    return sceneEndPoint;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::compressEndItem()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::compressEndItem()
{
    if (endItem_)
    {
        MemoryMapGraphicsItem* mapItem = dynamic_cast<MemoryMapGraphicsItem*>(endItem_);
        if (mapItem && !mapItem->isCompressed())
        {
            mapItem->compressMapItem();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getConnectedEndItemEndPoint()
//-----------------------------------------------------------------------------
quint64 MemoryConnectionItem::getConnectedEndItemLastAddress() const
{
    quint64 baseAddress = getRangeStartValue().toULongLong(0, 16);
    quint64 lastAddress = getRangeEndValue().toULongLong(0, 16);
    
    quint64 endItemLastAddress = endItem_->getLastAddress() + baseAddress;
    if (endItemLastAddress > lastAddress)
    {
        lastAddress = endItemLastAddress;
    }

    return lastAddress;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getEndItemHeight()
//-----------------------------------------------------------------------------
qreal MemoryConnectionItem::getEndItemHeight() const
{
    qreal endItemHeight = 0;

    if (endItem_)
    {
        endItemHeight = endItem_->boundingRect().height();
    }

    return endItemHeight;
}
