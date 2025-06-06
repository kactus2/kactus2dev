//-----------------------------------------------------------------------------
// File: MemoryConnectionItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.08.2016
//
// Description:
// Memory designer connection item between.
//-----------------------------------------------------------------------------

#include "MemoryConnectionItem.h"

#include <common/KactusColors.h>

#include <editors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryColumn.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryItem.h>

#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::MemoryConnectionItem()
//-----------------------------------------------------------------------------
MemoryConnectionItem::MemoryConnectionItem(MainMemoryGraphicsItem* startItem,
    quint64 firstStartValue, quint64 firstEndValue,
    quint64 secondStartValue, quint64 secondEndvalue,
    MainMemoryGraphicsItem* endItem,
    QGraphicsScene* containingScene,
    qreal yTransfer,
    QGraphicsItem* parent):
QGraphicsPathItem(parent),
firstItemStartLabel_(new QGraphicsTextItem(this)),
firstItemEndLabel_(new QGraphicsTextItem(this)),
secondItemStartLabel_(new QGraphicsTextItem(this)),
secondItemEndLabel_(new QGraphicsTextItem(this)),
startItem_(startItem),
endItem_(endItem),
connectionBaseAddress_(firstStartValue),
connectionLastAddress_(firstEndValue),
connectionWidth_(getConnectionWidth()),
yTransfer_(yTransfer)
{
    setupLabels(firstStartValue, firstEndValue, secondStartValue, secondEndvalue);

    if (connectionBaseAddress_ == connectionLastAddress_)
    {
        firstItemEndLabel_->hide();
        secondItemEndLabel_->hide();
    }

    containingScene->addItem(this);
    setZValue(10000);

    startItem_->addMemoryConnection(this);
    endItem_->addMemoryConnection(this);

    createPath();
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::createPath()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::createPath()
{
    setPath(QPainterPath());

    const qreal LINEWIDTH = 1;

    QPointF startItemTopRight = startItem_->sceneBoundingRect().topRight();
    QPointF endItemTopLeft = endItem_->sceneBoundingRect().topLeft();

    QPointF highStartPoint(startItemTopRight.x(), startItemTopRight.y() + yTransfer_ + LINEWIDTH / 2);
    QPointF lowStartPoint(highStartPoint.x(), highStartPoint.y() + connectionWidth_);
    QPointF highEndPoint(endItemTopLeft.x(), highStartPoint.y());
    QPointF lowEndPoint(highEndPoint.x(), lowStartPoint.y());

    QPainterPath path = createConnectionPath(highStartPoint, highEndPoint, lowStartPoint, lowEndPoint, LINEWIDTH);

    setPath(path);
    avoidCollisionsOnPath(highStartPoint, highEndPoint, lowStartPoint, lowEndPoint);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::createConnectionPath()
//-----------------------------------------------------------------------------
QPainterPath MemoryConnectionItem::createConnectionPath(QPointF const& highStartPoint, QPointF const& highEndPoint,
    QPointF const& lowStartPoint, QPointF const& lowEndPoint, int const& LINEWIDTH)
{
    QPainterPath path;
    path.moveTo(highStartPoint);

    QPen connectionPen = pen();
    connectionPen.setWidth(LINEWIDTH);

    if (dynamic_cast<MemoryMapGraphicsItem*>(endItem_) && !isLocalMapConnection())
    {
        path.lineTo(highEndPoint);
        path.moveTo(lowStartPoint);
        path.lineTo(lowEndPoint);
    }
    else
    {
        bool connectionIsLocal = isLocalMapConnection();
        if (connectionIsLocal)
        {
            connectionPen.setColor(QColor(0, 128, 255));
        }
        else
        {
            connectionPen.setColor(QColor(60, 153, 60));
        }

        path = createCollidingPathForUnusualConnection(
            connectionIsLocal, highStartPoint, highEndPoint, lowStartPoint, lowEndPoint);
    }

    setPen(connectionPen);

    return path;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::isLocalMapConnection()
//-----------------------------------------------------------------------------
bool MemoryConnectionItem::isLocalMapConnection() const
{
    QSharedPointer<MemoryItem const> startMemory = startItem_->getMemoryItem();
    QSharedPointer<MemoryItem const> endMemory = endItem_->getMemoryItem();

    foreach (QSharedPointer<MemoryItem const> startSubMemory, startMemory->getChildItems())
    {
        if (startSubMemory == endMemory)
        {
            return true;
        }
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getConnectionWidth()
//-----------------------------------------------------------------------------
qreal MemoryConnectionItem::getConnectionWidth() const
{
    qreal connectionWidth =
        (connectionLastAddress_ - connectionBaseAddress_ + 1) * MemoryDesignerConstants::RANGEINTERVAL;

    return connectionWidth;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::compressToUnCutCoordinates()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::compressToUnCutCoordinates(QVector<MainMemoryGraphicsItem*>& condensedItems,
    QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER, bool memoryItemsAreCompressed)
{
    qreal cutArea = 0;
    qreal connectionTop = sceneBoundingRect().top();
    qreal connectionLow = sceneBoundingRect().bottom();
    qreal previousCoordinate = connectionTop;
    foreach(qreal coordinate, unCutCoordinates)
    {
        if (coordinate > connectionTop && coordinate <= connectionLow)
        {
            if (qreal singleCut = coordinate - previousCoordinate; singleCut > 0)
            {
                if (memoryItemsAreCompressed)
                {
                    singleCut = singleCut - CUTMODIFIER;
                }
                else
                {
                    qreal requiredArea = MemoryDesignerConstants::getRequiredAreaForUsedArea(singleCut);
                    singleCut = singleCut - requiredArea;
                }

                if (singleCut > 0)
                {
                    cutArea += singleCut;
                }
            }

            previousCoordinate = coordinate;
        }

        if (coordinate > connectionLow)
        {
            break;
        }
    }

    if (cutArea > 0)
    {
        qreal condensedHeight = boundingRect().height() - cutArea;
        setCondensedHeight(condensedHeight);
    }

    compressEndItemToCoordinates(condensedItems, unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::compressEndItemToCoordinates()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::compressEndItemToCoordinates(
    QVector<MainMemoryGraphicsItem*>& compressedItems, QVector<qreal> unCutCoordinates,
    const qreal CUTMODIFIER, bool memoryItemsAreCompressed)
{
    if (!compressedItems.contains(endItem_))
    {
        compressedItems.append(endItem_);
        endItem_->compressToUnCutCoordinates(unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);
    }

    if (!compressedItems.contains(startItem_))
    {
        compressedItems.append(startItem_);
        startItem_->compressToUnCutCoordinates(unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::moveConnectedItems()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::moveConnectedItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems,
    MainMemoryGraphicsItem* movementOrigin, qreal transferY)
{
    moveConnectedItemWithoutConnections(movedItems, movementOrigin, getConnectionStartItem(), transferY);
    moveConnectedItemWithoutConnections(movedItems, movementOrigin, getConnectionEndItem(), transferY);

    reDrawConnection();
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::moveConnectedItem()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::moveConnectedItemWithoutConnections(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems, MainMemoryGraphicsItem const* movementOrigin,
    MainMemoryGraphicsItem* connectedItem, qreal transferY)
{
    if (connectedItem && connectedItem != movementOrigin && !movedItems->contains(connectedItem))
    {
        movedItems->append(connectedItem);
        connectedItem->moveBy(0, transferY);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::setCondensedHeight()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::setCondensedHeight(qreal newHeight)
{
    connectionWidth_ = newHeight;
    reDrawConnection();
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::avoidCollisionsOnPath()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::avoidCollisionsOnPath(QPointF const& highStartPoint, QPointF const& highEndPoint,
    QPointF const& lowStartPoint, QPointF const& lowEndPoint)
{
    QList<QGraphicsItem*> collisions = collidingItems(Qt::IntersectsItemBoundingRect);
    if (!collisions.isEmpty())
    {
        qreal connectionTop = highStartPoint.y();
        qreal connectionLow = lowStartPoint.y();

        QMultiMap<qreal, QPair<QPointF, QPointF> > highCollisionPoints;
        QMultiMap<qreal, QPair<QPointF, QPointF> > lowCollisionPoints;

        MemoryColumn const* startColumn = dynamic_cast<MemoryColumn*>(startItem_->parentItem());
        MemoryColumn const* endColumn = dynamic_cast<MemoryColumn*>(endItem_->parentItem());

        foreach (QGraphicsItem* collidingItem, collisions)
        {
            auto memoryItem = dynamic_cast<MemoryDesignerGraphicsItem*>(collidingItem);
            if (memoryItem)
            {
                bool isMainItem = dynamic_cast<MainMemoryGraphicsItem*>(memoryItem);
                bool isExtensionItem = dynamic_cast<MemoryExtensionGraphicsItem*>(memoryItem);
                bool isNotConnectedItem = memoryItem != startItem_ && memoryItem != endItem_;
                if (memoryItem && (isMainItem || isExtensionItem) && isNotConnectedItem)
                {
                    MemoryColumn const* itemColumn = dynamic_cast<MemoryColumn*>(memoryItem->parentItem());
                    if (!itemColumn && isExtensionItem)
                    {
                        MainMemoryGraphicsItem const* extensionParent = dynamic_cast<MainMemoryGraphicsItem*>(memoryItem->parentItem());
                        if (extensionParent)
                        {
                            itemColumn = dynamic_cast<MemoryColumn*>(extensionParent->parentItem());
                        }
                    }
                    bool columnIsNotConnectedColumn = itemColumn != startColumn && itemColumn != endColumn;
                    if (itemColumn && startColumn && endColumn && columnIsNotConnectedColumn)
                    {
                        QRectF collisionRectangle = memoryItem->sceneBoundingRect();
                        qreal collisionTop = collisionRectangle.top();
                        qreal collisionLow = collisionRectangle.bottom();

                        qreal collisionLeft = collisionRectangle.left();
                        qreal collisionRight = collisionRectangle.right();

                        if (collisionTop < connectionTop && collisionLow > connectionTop)
                        {
                            QPointF collisionStart (collisionLeft, connectionTop);
                            QPointF collisionEnd (collisionRight, connectionTop);

                            QPair<QPointF, QPointF> collisionPoints(collisionStart, collisionEnd);
                            highCollisionPoints.insert(collisionLeft, collisionPoints);
                        }

                        if (collisionTop < connectionLow && collisionLow > connectionLow)
                        {
                            QPointF collisionStart (collisionLeft, connectionLow);
                            QPointF collisionEnd (collisionRight, connectionLow);

                            QPair<QPointF, QPointF> collisionPoints(collisionStart, collisionEnd);
                            lowCollisionPoints.insert(collisionLeft, collisionPoints);
                        }
                    }
                }
            }
        }

        if (!highCollisionPoints.isEmpty() || !lowCollisionPoints.isEmpty())
        {
            createCollisionPath(
                highStartPoint, highEndPoint, lowStartPoint, lowEndPoint, highCollisionPoints, lowCollisionPoints);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::createCollisionPath()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::createCollisionPath(QPointF highStartPoint, QPointF highEndPoint,
    QPointF lowStartPoint, QPointF lowEndPoint,
    QMultiMap<qreal, QPair<QPointF, QPointF> > highCollisionPoints,
    QMultiMap<qreal, QPair<QPointF, QPointF> > lowCollisionPoints)
{
    QPainterPath collisionPath;
    
    if (dynamic_cast<MemoryMapGraphicsItem*>(endItem_) && !isLocalMapConnection())
    {
        collisionPath = eraseCollisionsFromPath(collisionPath, highStartPoint, highCollisionPoints);
        collisionPath.lineTo(highEndPoint);

        collisionPath = eraseCollisionsFromPath(collisionPath, lowStartPoint, lowCollisionPoints);
        collisionPath.lineTo(lowEndPoint);
    }
    else
    {
        QMultiMap<qreal, QPair<QPointF, QPointF> > newHighCollisions;
        QMultiMapIterator highCollisionIterator(highCollisionPoints);
        while (highCollisionIterator.hasNext())
        {
            highCollisionIterator.next();

            QPointF newCollisionStartPoint = highCollisionIterator.value().first;
            newCollisionStartPoint.setY(newCollisionStartPoint.y() + BRIDGEMODIFIER_);
            QPointF newCollisionEndPoint = highCollisionIterator.value().second;
            newCollisionEndPoint.setY(newCollisionEndPoint.y() + BRIDGEMODIFIER_);

            QPair<QPointF, QPointF> collisionPoints;
            collisionPoints.first = newCollisionStartPoint;
            collisionPoints.second = newCollisionEndPoint;

            newHighCollisions.insert(highCollisionIterator.key(), collisionPoints);
        }

        QMultiMap<qreal, QPair<QPointF, QPointF> > newLowCollisions;
        QMultiMapIterator lowCollisionIterator(lowCollisionPoints);
        while (lowCollisionIterator.hasNext())
        {
            lowCollisionIterator.next();

            QPointF newCollisionStartPoint = lowCollisionIterator.value().first;
            newCollisionStartPoint.setY(newCollisionStartPoint.y() - BRIDGEMODIFIER_);
            QPointF newCollisionEndPoint = lowCollisionIterator.value().second;
            newCollisionEndPoint.setY(newCollisionEndPoint.y() - BRIDGEMODIFIER_);

            QPair<QPointF, QPointF> collisionPoints;
            collisionPoints.first = newCollisionStartPoint;
            collisionPoints.second = newCollisionEndPoint;

            newLowCollisions.insert(lowCollisionIterator.key(), collisionPoints);
        }

        collisionPath = createCollidingPathForUnusualConnection(isLocalMapConnection(), highStartPoint, highEndPoint,
            lowStartPoint, lowEndPoint, newHighCollisions, newLowCollisions);
    }

    setPath(collisionPath);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::createCollidingPathForLocalConnection()
//-----------------------------------------------------------------------------
QPainterPath MemoryConnectionItem::createCollidingPathForUnusualConnection(bool connectionIsLocal,
    QPointF highStartPoint, QPointF highEndPoint, QPointF lowStartPoint, QPointF lowEndPoint,
    QMultiMap<qreal, QPair<QPointF, QPointF> > highCollisionPoints,
    QMultiMap<qreal, QPair<QPointF, QPointF> > lowCollisionPoints)
{
    QPainterPath collisionPath;

    qreal bridgeHighLineY = highStartPoint.y();
    qreal bridgeLowLineY = lowStartPoint.y();
    qreal bridgeHighY = highStartPoint.y() + BRIDGEMODIFIER_;
    qreal bridgeLowY = lowStartPoint.y() - BRIDGEMODIFIER_;
    qreal bridgeStartX = highStartPoint.x() + firstItemStartLabel_->boundingRect().width();
    qreal bridgeFirstPillarX = bridgeStartX + BRIDGEMODIFIER_;
    qreal bridgeEndX = highEndPoint.x() - secondItemStartLabel_->boundingRect().width();
    qreal bridgeLastPillarX = bridgeEndX - BRIDGEMODIFIER_;

    QPointF highCollisionStartPoint (bridgeFirstPillarX, bridgeHighY);
    QPointF lowCollisionStartPoint (bridgeFirstPillarX, bridgeLowY);

    collisionPath.moveTo(highStartPoint);
    if (connectionIsLocal)
    {
        collisionPath.lineTo(bridgeFirstPillarX, bridgeHighLineY);
        collisionPath.lineTo(highCollisionStartPoint);
    }
    else
    {
        collisionPath.lineTo(bridgeStartX, bridgeHighLineY);
        collisionPath.lineTo(bridgeFirstPillarX, bridgeHighY);
    }

    if (!highCollisionPoints.isEmpty() && highCollisionPoints.first().first.y() <= bridgeHighY)
    {
        collisionPath = eraseCollisionsFromPath(collisionPath, highCollisionStartPoint, highCollisionPoints);
    }

    if (connectionIsLocal)
    {
        collisionPath.lineTo(bridgeLastPillarX, bridgeHighY);
        collisionPath.lineTo(bridgeLastPillarX, bridgeHighLineY);
        collisionPath.lineTo(highEndPoint);

        collisionPath.moveTo(lowStartPoint);
        collisionPath.lineTo(bridgeFirstPillarX, bridgeLowLineY);
        collisionPath.lineTo(lowCollisionStartPoint);
    }
    else
    {
        collisionPath.lineTo(bridgeLastPillarX, bridgeHighY);
        collisionPath.lineTo(bridgeEndX, bridgeHighLineY);
        collisionPath.lineTo(highEndPoint);

        collisionPath.moveTo(lowStartPoint);
        collisionPath.lineTo(bridgeStartX, bridgeLowLineY);
        collisionPath.lineTo(bridgeFirstPillarX, bridgeLowY);
    }

    if (!lowCollisionPoints.isEmpty() && lowCollisionPoints.first().first.y() <= bridgeLowY)
    {
        collisionPath = eraseCollisionsFromPath(collisionPath, lowCollisionStartPoint, lowCollisionPoints);
    }

    if (connectionIsLocal)
    {
        collisionPath.lineTo(bridgeLastPillarX, bridgeLowY);
        collisionPath.lineTo(bridgeLastPillarX, bridgeLowLineY);
    }
    else
    {
        collisionPath.lineTo(bridgeLastPillarX, bridgeLowY);
        collisionPath.lineTo(bridgeEndX, bridgeLowLineY);
   }

    collisionPath.lineTo(lowEndPoint);
    return collisionPath;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::eraseCollisionsFromPath()
//-----------------------------------------------------------------------------
QPainterPath MemoryConnectionItem::eraseCollisionsFromPath(QPainterPath collisionPath, QPointF collisionBegin,
    QMultiMap<qreal, QPair<QPointF, QPointF> > collisionPoints)
{
    QPainterPath erasedPath = collisionPath;
    QPointF collisionStartPoint = collisionBegin;
    qreal pathY = collisionStartPoint.y();

    QMultiMapIterator<qreal, QPair<QPointF, QPointF> > collisionIterator(collisionPoints);
    while (collisionIterator.hasNext())
    {
        collisionIterator.next();
        QPair<QPointF, QPointF> singleCollisionPoints = collisionIterator.value();
        erasedPath.moveTo(collisionStartPoint);
        erasedPath.lineTo(singleCollisionPoints.first.x(), pathY);

        QPointF collisionEndPoint(singleCollisionPoints.second.x(), pathY);
        collisionStartPoint = collisionEndPoint;
    }

    erasedPath.moveTo(collisionStartPoint);

    return erasedPath;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::repositionLabels()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::repositionLabels()
{
    QFont labelFont = firstItemStartLabel_->font();
//     labelFont.setWeight(QFont::Bold);

    firstItemStartLabel_->setFont(labelFont);
    firstItemEndLabel_->setFont(labelFont);
    secondItemStartLabel_->setFont(labelFont);
    secondItemEndLabel_->setFont(labelFont);

    QRectF connectionRect = boundingRect();

    QPointF topLeft = connectionRect.topLeft();
    QPointF topRight = connectionRect.topRight();
    QPointF lowerLeft = connectionRect.bottomLeft();
    QPointF lowerRight = connectionRect.bottomRight();

    firstItemStartLabel_->setPos(topLeft);
    firstItemEndLabel_->setPos(lowerLeft.x(), lowerLeft.y() - firstItemEndLabel_->boundingRect().height());

    secondItemStartLabel_->setPos(topRight.x() - secondItemStartLabel_->boundingRect().width(), topRight.y());
    secondItemEndLabel_->setPos(lowerRight.x() - secondItemStartLabel_->boundingRect().width(), lowerRight.y() - secondItemEndLabel_->boundingRect().height());
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::setupLabels()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::setupLabels(quint64 const& leftStartValue, quint64 const& leftEndValue, quint64 const& rightStartValue, quint64 const& rightEndValue)
{
    QString leftStartValueInHexa = QString::number(leftStartValue, 16).toUpper();
    QString leftEndValueInHexa = QString::number(leftEndValue, 16).toUpper();
    QString rightStartValueInHexa = QString::number(rightStartValue, 16).toUpper();
    QString rightEndValueInHexa = QString::number(rightEndValue, 16).toUpper();

    int amountOfNumbersLeft = MemoryDesignerConstants::getAmountOfNumbersInRange(leftStartValueInHexa, leftEndValueInHexa);
    leftStartValueInHexa = MemoryDesignerConstants::getValueWithZerosAdded(leftStartValueInHexa, amountOfNumbersLeft);
    leftEndValueInHexa = MemoryDesignerConstants::getValueWithZerosAdded(leftEndValueInHexa, amountOfNumbersLeft);
    int amountOfNumbersRight = MemoryDesignerConstants::getAmountOfNumbersInRange(rightStartValueInHexa, rightEndValueInHexa);
    rightStartValueInHexa = MemoryDesignerConstants::getValueWithZerosAdded(rightStartValueInHexa, amountOfNumbersRight);
    rightEndValueInHexa = MemoryDesignerConstants::getValueWithZerosAdded(rightEndValueInHexa, amountOfNumbersRight);

    firstItemStartLabel_->setPlainText(leftStartValueInHexa);
    firstItemEndLabel_->setPlainText(leftEndValueInHexa);
    secondItemStartLabel_->setPlainText(rightStartValueInHexa);
    secondItemEndLabel_->setPlainText(rightEndValueInHexa);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::moveItemBy()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::moveItemBy(qreal const& movementInY)
{
    qreal newPosition = sceneBoundingRect().top() + movementInY;
    yTransfer_ = newPosition - startItem_->sceneBoundingRect().top();

    createPath();
    repositionLabels();
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
    createPath();
    repositionLabels();
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
        auto collidingLabel = dynamic_cast<QGraphicsTextItem*>(collidingItem);
        if (collidingLabel && collidingLabel != rangeLabel)
        {
            QRectF collidingRectangle = collidingLabel->sceneBoundingRect();
            collidingRectangle.setTop(collidingRectangle.top() + RECTANGLEMODIFIER);
            collidingRectangle.setBottom(collidingRectangle.bottom() - RECTANGLEMODIFIER);

            if (MemoryDesignerConstants::itemOverlapsAnotherItem(labelRectangle, 0, collidingRectangle, 0))
            {
                quint64 itemValue = rangeLabel->toPlainText().toULongLong(0, 16);
                if (quint64 collidingValue = collidingLabel->toPlainText().toULongLong(0, 16); itemValue < collidingValue)
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
    if (quint64 endItemSceneEndPoint = endItem_->getSceneRectangleWithSubItems().bottom(); endItemSceneEndPoint > sceneEndPoint)
    {
        sceneEndPoint = endItemSceneEndPoint;
    }

    return sceneEndPoint;
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
// Function: MemoryConnectionItem::labelCollidesWithRanges()
//-----------------------------------------------------------------------------
bool MemoryConnectionItem::labelCollidesWithRanges(QGraphicsTextItem* label, qreal fontHeight,
    const MainMemoryGraphicsItem* connectedItem) const
{
    QGraphicsTextItem const* comparedStartLabel = firstItemStartLabel_;
    QGraphicsTextItem const* comparedEndLabel = firstItemEndLabel_;

    if (connectedItem == endItem_)
    {
        comparedStartLabel = secondItemStartLabel_;
        comparedEndLabel = secondItemEndLabel_;
    }

    QRectF labelRectangle = label->sceneBoundingRect();
    QRectF comparedStartRectangle = comparedStartLabel->sceneBoundingRect();
    QRectF comparedEndRectangle = comparedEndLabel->sceneBoundingRect();

    labelRectangle.setHeight(fontHeight);
    comparedStartRectangle.setHeight(fontHeight);
    comparedEndRectangle.setHeight(fontHeight);

    return MemoryDesignerConstants::itemOverlapsAnotherItem(labelRectangle, 0, comparedStartRectangle, 0) ||
        MemoryDesignerConstants::itemOverlapsAnotherItem(labelRectangle, 0, comparedEndRectangle, 0);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::getConnectionLowPoint()
//-----------------------------------------------------------------------------
qreal MemoryConnectionItem::getConnectionLowPoint(MainMemoryGraphicsItem const* originItem) const
{
    qreal lowestPoint = sceneBoundingRect().bottom();

    if (MainMemoryGraphicsItem const* connectionStartItem = getConnectionStartItem(); !originItem || connectionStartItem != originItem)
    {
        qreal startItemLow = connectionStartItem->sceneBoundingRect().bottom();
        if (startItemLow > lowestPoint)
        {
            lowestPoint = startItemLow;
        }
    }

    if (MainMemoryGraphicsItem const* connectionEndItem = getConnectionEndItem(); !originItem || connectionEndItem != originItem)
    {
        qreal endItemLow = connectionEndItem->sceneBoundingRect().bottom();
        if (endItemLow > lowestPoint)
        {
            lowestPoint = endItemLow;
        }
    }

    return lowestPoint;
}
