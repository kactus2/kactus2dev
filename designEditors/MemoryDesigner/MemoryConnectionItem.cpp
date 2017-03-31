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
#include <designEditors/MemoryDesigner/MemoryItem.h>

#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::MemoryConnectionItem()
//-----------------------------------------------------------------------------
MemoryConnectionItem::MemoryConnectionItem(MainMemoryGraphicsItem* startItem, quint64 firstStartValue,
    quint64 firstEndValue, MainMemoryGraphicsItem* endItem, QGraphicsScene* containingScene, int yTransfer,
    QGraphicsItem* parent):
QGraphicsPathItem(parent),
firstItemStartLabel_(new QGraphicsTextItem(this)),
firstItemEndLabel_(new QGraphicsTextItem(this)),
secondItemStartLabel_(new QGraphicsTextItem(this)),
secondItemEndLabel_(new QGraphicsTextItem(this)),
startItem_(startItem),
endItem_(endItem),
yTransfer_(yTransfer),
connectionBaseAddress_(firstStartValue),
connectionLastAddress_(firstEndValue),
connectionWidth_(getConnectionWidth())
{
    setupLabels(firstStartValue, firstEndValue);

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

    QPointF startItemTopRight = startItem_->sceneBoundingRect().topRight();
    QPointF endItemTopLeft = endItem_->sceneBoundingRect().topLeft();

    QPointF highStartPoint(startItemTopRight.x(), startItemTopRight.y() + yTransfer_ + LINEWIDTH / 2);
    QPointF lowStartPoint(highStartPoint.x(), highStartPoint.y() + connectionWidth);
    QPointF highEndPoint(endItemTopLeft.x(), highStartPoint.y());
    QPointF lowEndPoint(highEndPoint.x(), lowStartPoint.y());

    QPainterPath path = createConnectionPath(highStartPoint, highEndPoint, lowStartPoint, lowEndPoint, LINEWIDTH);

    setPath(path);
    avoidCollisionsOnPath(highStartPoint, highEndPoint, lowStartPoint, lowEndPoint);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::createConnectionPath()
//-----------------------------------------------------------------------------
QPainterPath MemoryConnectionItem::createConnectionPath(QPointF highStartPoint, QPointF highEndPoint,
    QPointF lowStartPoint, QPointF lowEndPoint, const int LINEWIDTH)
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
        const int BRIDGEMODIFIER = 10;
        qreal bridgeHighLineY = highStartPoint.y();
        qreal bridgeLowLineY = lowStartPoint.y();
        qreal bridgeHighY = highStartPoint.y() + BRIDGEMODIFIER;
        qreal bridgeLowY = lowStartPoint.y() - BRIDGEMODIFIER;
        qreal bridgeStartX = highStartPoint.x() + BRIDGEMODIFIER;
        qreal bridgeFirstPillarX = bridgeStartX + BRIDGEMODIFIER;
        qreal bridgeEndX = highEndPoint.x() - BRIDGEMODIFIER;
        qreal bridgeLastPillarX = bridgeEndX - BRIDGEMODIFIER;

        if (isLocalMapConnection())
        {
            path.lineTo(bridgeFirstPillarX, bridgeHighLineY);
            path.lineTo(bridgeFirstPillarX, bridgeHighY);
            path.lineTo(bridgeLastPillarX, bridgeHighY);
            path.lineTo(bridgeLastPillarX, bridgeHighLineY);
            path.lineTo(highEndPoint);
            path.moveTo(lowStartPoint);
            path.lineTo(bridgeFirstPillarX, bridgeLowLineY);
            path.lineTo(bridgeFirstPillarX, bridgeLowY);
            path.lineTo(bridgeLastPillarX, bridgeLowY);
            path.lineTo(bridgeLastPillarX, bridgeLowLineY);
            path.lineTo(lowEndPoint);

            connectionPen.setColor(QColor(0, 128, 255));
        }

        else
        {
            path.lineTo(bridgeStartX, bridgeHighLineY);
            path.lineTo(bridgeFirstPillarX, bridgeHighY);
            path.lineTo(bridgeLastPillarX, bridgeHighY);
            path.lineTo(bridgeEndX, bridgeHighLineY);
            path.lineTo(highEndPoint);

            path.moveTo(lowStartPoint);
            path.lineTo(bridgeStartX, bridgeLowLineY);
            path.lineTo(bridgeFirstPillarX, bridgeLowY);
            path.lineTo(bridgeLastPillarX, bridgeLowY);
            path.lineTo(bridgeEndX, bridgeLowLineY);
            path.lineTo(lowEndPoint);

            connectionPen.setColor(QColor(60, 153, 60));
        }
    }

    setPen(connectionPen);

    return path;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::isLocalMapConnection()
//-----------------------------------------------------------------------------
bool MemoryConnectionItem::isLocalMapConnection() const
{
    QSharedPointer<MemoryItem> startMemory = startItem_->getMemoryItem();
    QSharedPointer<MemoryItem> endMemory = endItem_->getMemoryItem();

    foreach (QSharedPointer<MemoryItem> startSubMemory, startMemory->getChildItems())
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
// Function: MemoryConnectionItem::condenseToUnCutAddresses()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::condenseToUnCutAddresses(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > condensedItems, QVector<quint64> uncutAddresses,
    const int CUTMODIFIER)
{
    quint64 cutArea = 0;
    quint64 previousAddress = connectionBaseAddress_;
    foreach (quint64 address, uncutAddresses)
    {
        if (address > connectionBaseAddress_ && address <= connectionLastAddress_)
        {
            qint64 singleCut = address - previousAddress - CUTMODIFIER;
            if (singleCut > 0)
            {
                cutArea += singleCut;
            }

            previousAddress = address;
        }

        if (address > connectionLastAddress_)
        {
            break;
        }
    }

    if (cutArea > 0)
    {
        qreal condensedHeight = boundingRect().height() - (cutArea * MemoryDesignerConstants::RANGEINTERVAL);

        setCondensedHeight(condensedHeight);
    }

    compressEndItem(condensedItems, uncutAddresses, CUTMODIFIER);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::compressEndItem()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::compressEndItem(QSharedPointer<QVector<MainMemoryGraphicsItem*> > condensedItems,
    QVector<quint64> unCutAddresses, const int CUTMODIFIER)
{
    if (!condensedItems->contains(endItem_))
    {
        condensedItems->append(endItem_);
        endItem_->compressToUnCutAddresses(unCutAddresses, CUTMODIFIER);
    }

    if (!condensedItems->contains(startItem_))
    {
        condensedItems->append(startItem_);
        startItem_->compressToUnCutAddresses(unCutAddresses, CUTMODIFIER);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::moveConnectionAndConnectedItems()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::moveCutConnectionAndConnectedItems(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems, MainMemoryGraphicsItem* movementOrigin,
    quint64 cutAreaBegin, quint64 cutAreaEnd, qreal transferY)
{
    MainMemoryGraphicsItem* connectionStartItem = getConnectionStartItem();
    MainMemoryGraphicsItem* connectionEndItem = getConnectionEndItem();
    if (connectionStartItem && connectionEndItem)
    {
        quint64 startItemBaseAddress = connectionStartItem->getBaseAddress();
        quint64 endItemBaseAddress = connectionEndItem->getBaseAddress();
        
        bool needRedraw = false;

        if (getRangeStartValue() >= cutAreaEnd)
        {
            if (cutAreaBegin >= startItemBaseAddress)
            {
                moveConnectionWithoutConnectedItemsInY(transferY);
            }

            moveCutConnectedItemWithoutConnections(
                movedItems, movementOrigin, connectionStartItem, startItemBaseAddress, transferY, cutAreaEnd);
            moveCutConnectedItemWithoutConnections(
                movedItems, movementOrigin, connectionEndItem, endItemBaseAddress, transferY, cutAreaEnd);

            needRedraw = true;
        }
        else
        {
            if (startItemBaseAddress >= cutAreaBegin)
            {
                moveCutConnectedItemWithoutConnections(
                    movedItems, movementOrigin, connectionStartItem, startItemBaseAddress, transferY, cutAreaEnd);
                needRedraw = true;
            }
            if (endItemBaseAddress >= cutAreaEnd)
            {
                moveCutConnectedItemWithoutConnections(
                    movedItems, movementOrigin, connectionEndItem, endItemBaseAddress, transferY, cutAreaEnd);
                needRedraw = true;
            }
        }

        if (needRedraw)
        {
            reDrawConnection();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::moveCutConnectedItemWithoutConnections()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::moveCutConnectedItemWithoutConnections(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems, MainMemoryGraphicsItem* movementOrigin,
    MainMemoryGraphicsItem* connectedItem, quint64 itemBaseAddress, qreal transferY, quint64 cutAreaEnd)
{
    if (connectedItem != movementOrigin && !movedItems->contains(connectedItem))
    {
        movedItems->append(connectedItem);

        if (itemBaseAddress >= cutAreaEnd)
        {
            connectedItem->moveBy(0, transferY);
        }
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
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems, MainMemoryGraphicsItem* movementOrigin,
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
void MemoryConnectionItem::avoidCollisionsOnPath(QPointF highStartPoint, QPointF highEndPoint,
    QPointF lowStartPoint, QPointF lowEndPoint)
{
    QList<QGraphicsItem*> collisions = collidingItems(Qt::IntersectsItemBoundingRect);
    if (!collisions.isEmpty())
    {
        qreal connectionTop = highStartPoint.y();
        qreal connectionLow = lowStartPoint.y();

        QMap<qreal, QPair<QPointF, QPointF> > highCollisionPoints;
        QMap<qreal, QPair<QPointF, QPointF> > lowCollisionPoints;

        MemoryColumn* startColumn = dynamic_cast<MemoryColumn*>(startItem_->parentItem());
        MemoryColumn* endColumn = dynamic_cast<MemoryColumn*>(endItem_->parentItem());

        foreach (QGraphicsItem* collidingItem, collisions)
        {
            MemoryDesignerGraphicsItem* memoryItem = dynamic_cast<MemoryDesignerGraphicsItem*>(collidingItem);
            if (memoryItem)
            {
                bool isMainItem = dynamic_cast<MainMemoryGraphicsItem*>(memoryItem);
                bool isExtensionItem = dynamic_cast<MemoryExtensionGraphicsItem*>(memoryItem);
                bool isNotConnectedItem = memoryItem != startItem_ && memoryItem != endItem_;
                if (memoryItem && (isMainItem || isExtensionItem) && isNotConnectedItem)
                {
                    MemoryColumn* itemColumn = dynamic_cast<MemoryColumn*>(memoryItem->parentItem());
                    if (!itemColumn && isExtensionItem)
                    {
                        MainMemoryGraphicsItem* extensionParent =
                            dynamic_cast<MainMemoryGraphicsItem*>(memoryItem->parentItem());
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
                            highCollisionPoints.insertMulti(collisionLeft, collisionPoints);
                        }

                        if (collisionTop < connectionLow && collisionLow > connectionLow)
                        {
                            QPointF collisionStart (collisionLeft, connectionLow);
                            QPointF collisionEnd (collisionRight, connectionLow);

                            QPair<QPointF, QPointF> collisionPoints(collisionStart, collisionEnd);
                            lowCollisionPoints.insertMulti(collisionLeft, collisionPoints);
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
    QMap<qreal, QPair<QPointF, QPointF> > highCollisionPoints,
    QMap<qreal, QPair<QPointF, QPointF> > lowCollisionPoints)
{
    QPainterPath collisionPath;

    QPointF highCollisionStartPoint = highStartPoint;
    QMapIterator<qreal, QPair<QPointF, QPointF> > highIterator(highCollisionPoints);
    while (highIterator.hasNext())
    {
        highIterator.next();
        QPair<QPointF, QPointF> collisionPoint = highIterator.value();
        collisionPath.moveTo(highCollisionStartPoint);
        collisionPath.lineTo(collisionPoint.first);
        highCollisionStartPoint = collisionPoint.second;
    }

    collisionPath.moveTo(highCollisionStartPoint);
    collisionPath.lineTo(highEndPoint);

    QPointF lowCollisionStartPoint = lowStartPoint;
    QMapIterator<qreal, QPair<QPointF, QPointF> > lowIterator(lowCollisionPoints);
    while (lowIterator.hasNext())
    {
        lowIterator.next();
        QPair<QPointF, QPointF> collisionPoint = lowIterator.value();
        collisionPath.moveTo(lowCollisionStartPoint);
        collisionPath.lineTo(collisionPoint.first);
        lowCollisionStartPoint = collisionPoint.second;
    }

    collisionPath.moveTo(lowCollisionStartPoint);
    collisionPath.lineTo(lowEndPoint);

    setPath(collisionPath);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::repositionLabels()
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
// Function: MemoryConnectionItem::setupLabels()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::setupLabels(quint64 startValue, quint64 endValue)
{
    QString startValueInHexa = QString::number(startValue, 16).toUpper();
    QString endValueInHexa = QString::number(endValue, 16).toUpper();

    int amountOfNumbers = MemoryDesignerConstants::getAmountOfNumbersInRange(startValueInHexa, endValueInHexa);
    startValueInHexa = MemoryDesignerConstants::getValueWithZerosAdded(startValueInHexa, amountOfNumbers);
    endValueInHexa = MemoryDesignerConstants::getValueWithZerosAdded(endValueInHexa, amountOfNumbers);

    firstItemStartLabel_->setPlainText(startValueInHexa);
    firstItemEndLabel_->setPlainText(endValueInHexa);
    secondItemStartLabel_->setPlainText(startValueInHexa);
    secondItemEndLabel_->setPlainText(endValueInHexa);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionItem::moveConnectionWithoutConnectedItemsInY()
//-----------------------------------------------------------------------------
void MemoryConnectionItem::moveConnectionWithoutConnectedItemsInY(qreal ytransfer)
{
    yTransfer_ += ytransfer;
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

            if (MemoryDesignerConstants::itemOverlapsAnotherItem(labelRectangle, 0, collidingRectangle, 0))
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

    quint64 endItemSceneEndPoint = endItem_->getSceneRectangleWithSubItems().bottom();

    if (endItemSceneEndPoint > sceneEndPoint)
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
    QGraphicsTextItem* comparedStartLabel = firstItemStartLabel_;
    QGraphicsTextItem* comparedEndLabel = firstItemEndLabel_;

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
qreal MemoryConnectionItem::getConnectionLowPoint(MainMemoryGraphicsItem* originItem) const
{
    qreal lowestPoint = sceneBoundingRect().bottom();

    MainMemoryGraphicsItem* connectionStartItem = getConnectionStartItem();

    if (!originItem || connectionStartItem != originItem)
    {
        qreal startItemLow = connectionStartItem->sceneBoundingRect().bottom();
        if (startItemLow > lowestPoint)
        {
            lowestPoint = startItemLow;
        }
    }

    MainMemoryGraphicsItem* connectionEndItem = getConnectionEndItem();
    if (!originItem || connectionEndItem != originItem)
    {
        qreal endItemLow = connectionEndItem->sceneBoundingRect().bottom();
        if (endItemLow > lowestPoint)
        {
            lowestPoint = endItemLow;
        }
    }

    return lowestPoint;
}
