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
#include <designEditors/MemoryDesigner/MemoryExtensionGraphicsItem.h>

#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::MemoryConnectionItem()
//-----------------------------------------------------------------------------
MemoryConnectionItem::MemoryConnectionItem(MainMemoryGraphicsItem* startItem, QString const& firstStartValue,
    QString const& firstEndValue, MainMemoryGraphicsItem* endItem, QGraphicsScene* containingScene,
    bool memoryItemsAreCondensed, int yTransfer, QGraphicsItem* parent):
QGraphicsPathItem(parent),
amountOfNumbers_(getAmountOfNumbers(firstStartValue, firstEndValue)),
firstItemStartLabel_(new QGraphicsTextItem(this)),
firstItemEndLabel_(new QGraphicsTextItem(this)),
secondItemStartLabel_(new QGraphicsTextItem(this)),
secondItemEndLabel_(new QGraphicsTextItem(this)),
startItem_(startItem),
endItem_(endItem),
yTransfer_(yTransfer),
connectionBaseAddress_(firstStartValue.toULongLong()),
connectionLastAddress_(firstEndValue.toULongLong()),
connectionWidth_(0),
memoryItemsAreCondensed_(memoryItemsAreCondensed)
{
    QString startValueInHexa = formatValueToHexadecimal(firstStartValue);
    QString endValueInHexa = formatValueToHexadecimal(firstEndValue);
    firstItemStartLabel_->setPlainText(startValueInHexa);
    firstItemEndLabel_->setPlainText(endValueInHexa);
    secondItemStartLabel_->setPlainText(startValueInHexa);
    secondItemEndLabel_->setPlainText(endValueInHexa);

    if (connectionBaseAddress_ == connectionLastAddress_)
    {
        firstItemEndLabel_->hide();
        secondItemEndLabel_->hide();
    }

    containingScene->addItem(this);

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
// Function: MemoryConnectionItem::createPath()
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
        (connectionLastAddress_ - connectionBaseAddress_ + 1) * MemoryDesignerConstants::RANGEINTERVAL;

    if (memoryItemsAreCondensed_)
    {
        qreal endItemHeight = endItem_->getMinimumRequiredHeight(connectionBaseAddress_, connectionLastAddress_);
        qreal startItemHeight =
            startItem_->getMinimumRequiredHeight(connectionBaseAddress_, connectionLastAddress_);

        qreal comparisonHeight = endItemHeight;
        if (startItemHeight > endItemHeight)
        {
            comparisonHeight = startItemHeight;
        }

        if (comparisonHeight < connectionWidth)
        {
            connectionWidth = comparisonHeight;

            quint64 connectionSize = connectionLastAddress_ - connectionBaseAddress_;

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
            if (memoryItem && (dynamic_cast<MainMemoryGraphicsItem*>(memoryItem) ||
                dynamic_cast<MemoryExtensionGraphicsItem*>(memoryItem)) && memoryItem != startItem_ &&
                memoryItem != endItem_)
            {
                MemoryColumn* itemColumn = dynamic_cast<MemoryColumn*>(memoryItem->parentItem());
                if (itemColumn && startColumn && endColumn && itemColumn != startColumn && itemColumn != endColumn)
                {
                    QRectF itemSceneRect = memoryItem->sceneBoundingRect();
                    QPointF highCollisionStart = itemSceneRect.topLeft();
                    if (highCollisionStart.y() < highStartPoint.y())
                    {
                        highCollisionStart.setY(highStartPoint.y());
                        QPointF highCollisionEnd = itemSceneRect.topRight();
                        highCollisionEnd.setY(highStartPoint.y());

                        QPair<QPointF, QPointF> collisionPoints(highCollisionStart, highCollisionEnd);
                        highCollisionPoints.append(collisionPoints);
                    }

                    QPointF lowCollisionStart = itemSceneRect.bottomLeft();
                    if (lowCollisionStart.y() > lowStartPoint.y())
                    {
                        lowCollisionStart.setY(lowStartPoint.y());
                        QPointF lowCollisionEnd = itemSceneRect.bottomRight();
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

    QRectF connectionRect = boundingRect();

    QPointF topLeft = connectionRect.topLeft();
    QPointF topRight = connectionRect.topRight();
    QPointF lowerLeft = connectionRect.bottomLeft();
    QPointF lowerRight = connectionRect.bottomRight();

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
    yTransfer_ += yTransfer;

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
quint64 MemoryConnectionItem::getRangeStartValue() const
{
    return connectionBaseAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getRangeEndValue()
//-----------------------------------------------------------------------------
quint64 MemoryConnectionItem::getRangeEndValue() const
{
    return connectionLastAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::reDrawConnection()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::reDrawConnection()
{
    if (sceneBoundingRect().right() != getConnectionEndItem()->sceneBoundingRect().left())
    {
        createPath();
        repositionLabels();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::repositionCollidingTextLabels()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::repositionCollidingRangeLabels()
{
    repositionSingleRangeLabel(firstItemStartLabel_);
    repositionSingleRangeLabel(firstItemEndLabel_);
    repositionSingleRangeLabel(secondItemStartLabel_);
    repositionSingleRangeLabel(secondItemEndLabel_);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::repositionCollidingTextLabel()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::repositionSingleRangeLabel(QGraphicsTextItem* rangeLabel) const
{
    const int RECTANGLEMODIFIER = 7;

    QRectF labelRectangle = rangeLabel->sceneBoundingRect();
    labelRectangle.setTop(labelRectangle.top() + RECTANGLEMODIFIER);
    labelRectangle.setBottom(labelRectangle.bottom() - RECTANGLEMODIFIER);

    foreach (QGraphicsItem* collidingItem, rangeLabel->collidingItems())
    {
        QGraphicsTextItem* collidingLabel = dynamic_cast<QGraphicsTextItem*>(collidingItem);
        if (collidingLabel && collidingLabel != rangeLabel)
        {
            QRectF collidingRectangle = collidingLabel->sceneBoundingRect();
            collidingRectangle.setTop(collidingRectangle.top() + RECTANGLEMODIFIER);
            collidingRectangle.setBottom(collidingRectangle.bottom() - RECTANGLEMODIFIER);

            if (itemCollidesWithAnotherItem(labelRectangle, 0, collidingRectangle, 0))
            {
                quint64 itemValue = rangeLabel->toPlainText().toULongLong(0, 16);
                quint64 collidingValue = collidingLabel->toPlainText().toULongLong(0, 16);
                if (itemValue < collidingValue)
                {
                    rangeLabel->moveBy(0, -6);
                    collidingLabel->moveBy(0, 7);
                }

                return;
            }
        }
    }
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
void MemoryConnectionItem::compressEndItem(QSharedPointer<QVector<MemoryConnectionItem*> > movedConnections)
{
    if (endItem_)
    {
        MemoryMapGraphicsItem* mapItem = dynamic_cast<MemoryMapGraphicsItem*>(endItem_);
        if (mapItem && !mapItem->isCompressed())
        {
            mapItem->compressMapItem(movedConnections);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getConnectedEndItemEndPoint()
//-----------------------------------------------------------------------------
quint64 MemoryConnectionItem::getConnectedEndItemLastAddress() const
{
    quint64 lastAddress = connectionLastAddress_;

    quint64 endItemLastAddress = endItem_->getLastAddress() + connectionBaseAddress_;
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

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::endItemIsMemoryMap()
//-----------------------------------------------------------------------------
bool MemoryConnectionItem::endItemIsMemoryMap() const
{
    MemoryMapGraphicsItem* mapItem = dynamic_cast<MemoryMapGraphicsItem*>(endItem_);
    if (mapItem)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getStartItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionItem::getConnectionStartItem() const
{
    return startItem_;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getConnectionEndItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionItem::getConnectionEndItem() const
{
    return endItem_;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::setConnectionWidth()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::setConnectionWidth()
{
    connectionWidth_ = getComparedConnectionHeight(this);
    reDrawConnection();
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getComparedConnectionHeight()
//-----------------------------------------------------------------------------
qreal MemoryConnectionItem::getComparedConnectionHeight(MemoryConnectionItem* connectionItem) const
{
    qreal connectionHeight = connectionItem->getConnectionWidth();
    if (memoryItemsAreCondensed_)
    {
        qreal containedConnectionsHeight = 0;

        quint64 comparisonLastAddress = 0;

        MainMemoryGraphicsItem* connectionEndItem = connectionItem->getConnectionEndItem();
        foreach (MemoryConnectionItem* comparisonConnection, connectionEndItem->getMemoryConnections())
        {
            if (comparisonConnection->getConnectionStartItem() == connectionEndItem)
            {
                containedConnectionsHeight += getComparedConnectionHeight(comparisonConnection);
                comparisonLastAddress = comparisonConnection->getRangeEndValue();
            }
        }

        if (connectionLastAddress_ < comparisonLastAddress)
        {
            containedConnectionsHeight -= MemoryDesignerConstants::RANGEINTERVAL * 2;
        }

        connectionHeight = qMax(connectionHeight, containedConnectionsHeight);

        foreach (MemoryConnectionItem* comparisonConnection, startItem_->getMemoryConnections())
        {
            if (comparisonConnection->getConnectionEndItem() == startItem_)
            {
                qreal comparisonHeight = comparisonConnection->boundingRect().height() - 1;
                if (comparisonHeight >= connectionHeight)
                {
                    quint64 comparisonLastAddress = comparisonConnection->getRangeEndValue();
                    if (comparisonLastAddress < connectionLastAddress_)
                    {
                        connectionHeight = comparisonHeight + MemoryDesignerConstants::RANGEINTERVAL * 2;
                    }
                }
            }
        }
    }

    return connectionHeight;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::itemCollidesWithAnotherItem()
//-----------------------------------------------------------------------------
bool MemoryConnectionItem::itemCollidesWithAnotherItem(QRectF firstRectangle, int firstPenWidth,
    QRectF secondRectangle, int secondPenWidth) const
{
    qreal firstItemTop = firstRectangle.top() + firstPenWidth;
    qreal firstItemLow = firstRectangle.bottom() - firstPenWidth;
    qreal secondItemTop = secondRectangle.top() + secondPenWidth;
    qreal secondItemLow = secondRectangle.bottom() - secondPenWidth;

    if (
        ((firstItemTop > secondItemTop && firstItemTop < secondItemLow) ||
        (firstItemLow < secondItemLow && firstItemLow > secondItemTop) ||

        (secondItemTop > firstItemTop && secondItemTop < firstItemLow) ||
        (secondItemLow < firstItemLow && secondItemLow > firstItemTop) ||

        (firstItemTop == secondItemTop && firstItemLow == secondItemLow)))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::labelCollidesWithRanges()
//-----------------------------------------------------------------------------
bool MemoryConnectionItem::labelCollidesWithRanges(QGraphicsTextItem* label,
    const MainMemoryGraphicsItem* connectedItem) const
{
    if (connectedItem == startItem_)
    {
        return label->collidesWithItem(firstItemStartLabel_) || label->collidesWithItem(firstItemEndLabel_);
    }
    else if (connectedItem == endItem_)
    {
        return label->collidesWithItem(secondItemStartLabel_) || label->collidesWithItem(secondItemEndLabel_);
    }
    else
    {
        return false;
    }
}
