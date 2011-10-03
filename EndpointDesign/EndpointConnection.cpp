//-----------------------------------------------------------------------------
// File: EndpointConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.6.2011
//
// Description:
// Endpoint connection class.
//-----------------------------------------------------------------------------

#include "EndpointConnection.h"

#include "EndpointItem.h"

#include <common/DiagramUtil.h>

#include <QGraphicsScene>
#include <QVector2D>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

qreal const EndpointConnection::MIN_LENGTH = 10;
qreal const EndpointConnection::MIN_START_LENGTH = EndpointItem::WIDTH / 2 + GridSize;

//-----------------------------------------------------------------------------
// Function: EndpointConnection()
//-----------------------------------------------------------------------------
EndpointConnection::EndpointConnection(EndpointItem* endpoint1, EndpointItem* endpoint2,
                                       bool autoConnect, QGraphicsItem* parent) : QGraphicsPathItem(parent),
                                                                                  endpoint1_(0), endpoint2_(0),
                                                                                  pathPoints_(), selected_(-1),
                                                                                  selectionType_(NONE)
{
    setItemSettings();
    createRoute(endpoint1, endpoint2);

    if (autoConnect)
    {
        endpoint1_ = endpoint1;
        endpoint2_ = endpoint2;

        endpoint1->addConnection(this);
        endpoint2->addConnection(this);
        updateName();
    }
}

//-----------------------------------------------------------------------------
// Function: EndpointConnection()
//-----------------------------------------------------------------------------
EndpointConnection::EndpointConnection(QPointF p1, QVector2D const& dir1,
                                       QPointF p2, QVector2D const& dir2,
                                       QGraphicsItem* parent) : QGraphicsPathItem(parent),
                                                                endpoint1_(0), endpoint2_(0),
                                                                pathPoints_(), selected_(-1),
                                                                selectionType_(NONE)
{
    setItemSettings();
    createRoute(p1, p2, dir1, dir2);
}

//-----------------------------------------------------------------------------
// Function: ~EndpointConnection()
//-----------------------------------------------------------------------------
EndpointConnection::~EndpointConnection()
{
    disconnectEnds();
}

//-----------------------------------------------------------------------------
// Function: setRoute()
//-----------------------------------------------------------------------------
void EndpointConnection::setRoute(QList<QPointF> path)
{
    if (path.size() < 2)
        return;

    if (endpoint1_)
    {
        if (endpoint1_->parentItem() != 0)
        {
            endpoint1_->setPos(endpoint1_->parentItem()->mapFromScene(path.first()));
        }
        else
        {
            endpoint1_->setPos(path.first());
        }

        path.replace(0, endpoint1_->scenePos());
    }

    if (endpoint2_)
    {
        if (endpoint2_->parentItem() != 0)
        {
            endpoint2_->setPos(endpoint2_->parentItem()->mapFromScene(path.last()));
        }
        else
        {
            endpoint2_->setPos(path.last());
        }

        path.replace(path.size() - 1, endpoint2_->scenePos());
    }

    QListIterator<QPointF> i(path);

    QPainterPath painterPath(i.next());

    while (i.hasNext()) {
        painterPath.lineTo(i.next());
    }

    QPainterPathStroker stroker;
    setPath(stroker.createStroke(painterPath));

    pathPoints_ = path;
}

//-----------------------------------------------------------------------------
// Function: updatePosition()
//-----------------------------------------------------------------------------
void EndpointConnection::updatePosition()
{
    if (pathPoints_.size() < 2)
    {
        createRoute(endpoint1_, endpoint2_);
        return;
    }

    QVector2D delta1 = QVector2D(endpoint1_->scenePos()) - QVector2D(pathPoints_.first());
    QVector2D delta2 = QVector2D(endpoint2_->scenePos()) - QVector2D(pathPoints_.last());
    QVector2D const& dir1 = endpoint1_->getDirectionVector();
    QVector2D const& dir2 = endpoint2_->getDirectionVector();

    // If the delta movement of both endpoints was the same, we can just
    // move all route points by the delta1.
    if (qFuzzyCompare(delta1, delta2))
    {
        for (int i = 0; i < pathPoints_.size(); ++i)
        {
            pathPoints_[i] += delta1.toPointF();
        }

        setRoute(pathPoints_);
    }
    // Otherwise check if either the first or the last point was moved.
    else if (!delta1.isNull() || !delta2.isNull())
    {
        QVector2D delta = delta1;
        QVector2D dir = dir1;
        EndpointItem* endpoint = endpoint1_;
        int index0 = 0;
        int index1 = 1;
        
        if (!delta2.isNull())
        {
            delta = delta2;
            endpoint = endpoint2_;
            dir = dir2;
            index0 = pathPoints_.size() - 1;
            index1 = pathPoints_.size() - 2;
        }

        QVector2D seg1 = QVector2D(pathPoints_[index1] - pathPoints_[index0]).normalized();

        bool pathOk = false;

        // Fix the first segment with perpendicular projection.
        if (pathPoints_.size() >= 4)
        {
            QVector2D perp = delta - QVector2D::dotProduct(delta, seg1) * seg1;
            pathPoints_[index1] += perp.toPointF();
            pathOk = true;
        }

        // Handle the parallel part of the delta.
        pathPoints_[index0] = endpoint->scenePos();
        QVector2D newSeg1 = QVector2D(pathPoints_[index1] - pathPoints_[index0]);
        
        if (!pathOk || newSeg1.length() < MIN_START_LENGTH ||
            newSeg1.length() > 320 || pathPoints_.size() > 4)
        {
            createRoute(endpoint1_, endpoint2_);
        }
        else
        {
            setRoute(pathPoints_);
        }
    }     

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: connectEnds()
//-----------------------------------------------------------------------------
bool EndpointConnection::connectEnds()
{
    if (!scene())
        return false;

    prepareGeometryChange();

    // Disconnect the previous ends.
    disconnectEnds();

    // Find the new end points.
    endpoint1_ = DiagramUtil::snapToItem<EndpointItem>(pathPoints_.first(), scene(), GridSize);
    Q_ASSERT(endpoint1_ != 0);

    endpoint2_ = DiagramUtil::snapToItem<EndpointItem>(pathPoints_.last(), scene(), GridSize);
    Q_ASSERT(endpoint2_ != 0);

    // Make the connections and check for errors.
    if (!endpoint1_->onConnect(endpoint2_))
    {
        endpoint1_ = 0;
        return false;
    }

    if (!endpoint2_->onConnect(endpoint1_))
    {
        endpoint1_->onDisconnect(endpoint2_);
        endpoint1_ = 0;
        endpoint2_ = 0;
        return false;
    }

    endpoint1_->addConnection(this);
    endpoint2_->addConnection(this);

    // Check if both end points were found.
    if (endpoint1_ && endpoint2_) {
        simplifyPath();
        setRoute(pathPoints_);
        updateName();
        return true;
    }

    foreach (QGraphicsItem *item, scene()->items(pathPoints_.first())) {
        if (item == this)
            continue;

        if (item->type() == EndpointConnection::Type) {
            EndpointConnection *segment
                = qgraphicsitem_cast<EndpointConnection *>(item);

            if (segment->getRoute().last() == pathPoints_.first()) {
                QList<QPointF> newPath = segment->getRoute();
                newPath.append(pathPoints_);
                pathPoints_ = newPath;
                scene()->removeItem(segment);
                delete segment;

                return connectEnds();
            } else if (segment->getRoute().first() == pathPoints_.first()) {
                QList<QPointF> newPath = segment->getRoute();
                for(int i = 0; i < (newPath.size()/2); i++)
                    newPath.swap(i, newPath.size()-(1+i));
                newPath.append(pathPoints_);
                pathPoints_ = newPath;
                scene()->removeItem(segment);
                delete segment;

                return connectEnds();
            }
        }
    }

    foreach (QGraphicsItem *item, scene()->items(pathPoints_.last())) {
        if (item == this)
            continue;

        if (item->type() == EndpointConnection::Type) {
            EndpointConnection *segment
                = qgraphicsitem_cast<EndpointConnection *>(item);

            if (segment->getRoute().first() == pathPoints_.last()) {
                pathPoints_.append(segment->getRoute());
                scene()->removeItem(segment);
                delete segment;

                return connectEnds();
            } else if (segment->getRoute().last() == pathPoints_.last()) {
                QList<QPointF> newPath = segment->getRoute();
                for(int i = 0; i < (newPath.size()/2); i++)
                    newPath.swap(i, newPath.size()-(1+i));
                pathPoints_.append(newPath);
                scene()->removeItem(segment);
                delete segment;

                return connectEnds();
            }
        }
    }

    simplifyPath();
    setRoute(pathPoints_);
    updateName();
    return true;
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& EndpointConnection::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> const& EndpointConnection::getRoute() const
{
    return pathPoints_;
}

//-----------------------------------------------------------------------------
// Function: getEndpoint1()
//-----------------------------------------------------------------------------
EndpointItem* EndpointConnection::getEndpoint1()
{
    return endpoint1_;
}

//-----------------------------------------------------------------------------
// Function: getEndpoint2()
//-----------------------------------------------------------------------------
EndpointItem* EndpointConnection::getEndpoint2()
{
    return endpoint2_;
}

//-----------------------------------------------------------------------------
// Function: setItemSettings()
//-----------------------------------------------------------------------------
void EndpointConnection::setItemSettings()
{
    setZValue(-1000);
    QPen newPen = pen();
    newPen.setWidth(3);
    setPen(newPen);

    setFlag(ItemIsSelectable);
}

//-----------------------------------------------------------------------------
// Function: createRoute()
//-----------------------------------------------------------------------------
void EndpointConnection::createRoute(QPointF p1, QPointF p2,
                                     QVector2D const& dir1, QVector2D const& dir2)
{
    pathPoints_.clear();

    // Convert the points to vectors.
    QVector2D startPos = QVector2D(p1);
    QVector2D endPos = QVector2D(p2);

    // Sets the start position as the current position.
    QVector2D curPos = startPos;
    QVector2D curDir = dir1;

    // Set the target position based on the end point's direction.
    QVector2D targetPos = QVector2D(p2) + dir2 * MIN_START_LENGTH;

    // Add the start position to the painter path.
    QPainterPath path(curPos.toPointF());
    pathPoints_ << curPos.toPointF();

    if (startPos != endPos)
    {
        // Find a route to the target.
        while (curPos != targetPos)
        {
            // Calculate the delta.
            QVector2D delta = targetPos - curPos;
            qreal dot = QVector2D::dotProduct(delta, curDir);
            qreal endDot = QVector2D::dotProduct(delta, dir2);

            // Calculate the projection onto the current direction and the perpendicular part.
            QVector2D proj = dot * curDir;
            QVector2D perp = delta - proj;

            // Check if we can draw a direct line to the target from the current position.
            if (dot > 0.0 && qFuzzyCompare(delta, proj) && endDot <= 0.0)
            {
                curPos = targetPos;
            }
            else 
            {
                // Otherwise draw at least some distance to the current direction.
                // Check if the target is not behind the current position/direction.
                if (qFuzzyCompare(curPos, startPos))
                {
                    if (dot > 0.0 && !(endDot > 0.0 && qFuzzyCompare(delta, endDot * dir2)))
                    {
                        // Draw the length of the projection to the current direction
                        // or at least the minimum start length.
                        curPos = curPos + curDir * std::max(MIN_START_LENGTH, proj.length());
                    }
                    else
                    {
                        // Otherwise we just draw the minimum start length thub.
                        curPos = curPos + curDir * MIN_START_LENGTH;
                    }
                }
                // Check if the target is in the opposite direction compared to the current
                // direction and we previously drew the starting thub.
                else if (dot < 0 && qFuzzyCompare(curPos, startPos + curDir * MIN_START_LENGTH))
                {
                    // Draw to the perpendicular direction at least the minimum length.
                    qreal length = std::max(perp.length(), MIN_LENGTH);
                    QVector2D dir = perp.normalized();

                    // Special case when the perpendicular vector would be zero-length.
                    if (dir.isNull())
                    {
                        // Rotate 90 degrees.
                        dir = QVector2D(curDir.y(), -curDir.x());
                    }

                    curPos = curPos + dir * length;
                    curDir = dir;
                }
                else
                {
                    // Otherwise we just draw to the perpendicular direction as much as we can.
                    if (!perp.isNull())
                    {
                        curPos = curPos + perp;
                        curDir = perp.normalized();
                    }
                    else
                    {
                        // If the perpendicular vector was zero-length, we rotate the current
                        // direction 90 degrees and draw a minimum length thub to the new direction.
                        curDir = QVector2D(curDir.y(), -curDir.x());
                        curPos = curPos + curDir * MIN_LENGTH;
                    }
                }

                // Check if we would end up in a position where we are right behind the target.
                QVector2D newDelta = targetPos - curPos;
                qreal endDot = QVector2D::dotProduct(newDelta, dir2);

                if (endDot > 0.0 && qFuzzyCompare(newDelta, endDot * dir2))
                {
                    // Make an adjustment to the current position.
                    curPos += curDir * MIN_LENGTH;
                }
            }

            // Add the newly calculated current position to the path.
            path.lineTo(curPos.toPointF());
            pathPoints_ << curPos.toPointF();
        }

        // Add the last segment if the target position was not the end position.
        if (!qFuzzyCompare(targetPos, endPos))
        {
            path.lineTo(endPos.toPointF());
            pathPoints_ << endPos.toPointF();
        }
    }

    simplifyPath();

    QPainterPathStroker stroker;
    setPath(stroker.createStroke(path));
}

//-----------------------------------------------------------------------------
// Function: createRoute()
//-----------------------------------------------------------------------------
void EndpointConnection::createRoute(EndpointItem* endpoint1, EndpointItem* endpoint2)
{
    Q_ASSERT(endpoint1 != 0);
    Q_ASSERT(endpoint2 != 0);

    QPointF p1 = endpoint1->scenePos();
    QPointF p2 = endpoint2->scenePos();

    // Auto-rotate the endpoints to get simpler paths.
    if (p2.x() - p1.x() >= EndpointItem::WIDTH)
    {
        endpoint1->setDirection(EndpointItem::DIR_RIGHT);
        endpoint2->setDirection(EndpointItem::DIR_LEFT);
    }
    else if (p1.x() - p2.x() >= EndpointItem::WIDTH)
    {
        endpoint1->setDirection(EndpointItem::DIR_LEFT);
        endpoint2->setDirection(EndpointItem::DIR_RIGHT);
    }
    else
    {
        // Otherwise inherit the second endpoint's direction from the first endpoint.
        endpoint2->setDirection(endpoint1->getDirection());
    }

    // Create the route.
    createRoute(p1, p2, endpoint1->getDirectionVector(), endpoint2->getDirectionVector());
}

//-----------------------------------------------------------------------------
// Function: simplifyPath()
//-----------------------------------------------------------------------------
void EndpointConnection::simplifyPath()
{
    for (int i = 0; i < pathPoints_.size() - 2; ++i)
    {
        QVector2D pt0 = QVector2D(pathPoints_[i]);
        QVector2D pt1 = QVector2D(pathPoints_[i + 1]);
        QVector2D pt2 = QVector2D(pathPoints_[i + 2]);

        QVector2D delta1 = pt1 - pt0;
        QVector2D delta2 = pt2 - pt1;

        QVector2D deltaProj = QVector2D::dotProduct(delta2, delta1.normalized()) * delta1.normalized();

        // If the path was otherwise ok, just remove parallel lines.
        if (qFuzzyCompare(deltaProj, delta2))
        {
            pathPoints_.removeAt(i + 1);
            --i;
        }                                
    }

    selected_ = -1;
    selectionType_ = NONE;
}

//-----------------------------------------------------------------------------
// Function: disconnectEnds()
//-----------------------------------------------------------------------------
void EndpointConnection::disconnectEnds()
{
    // Discard existing connections.
    if (endpoint1_)
    {
        endpoint1_->removeConnection(this);
        endpoint1_->onDisconnect(endpoint2_);
    }

    if (endpoint2_)
    {
        endpoint2_->removeConnection(this);
        endpoint2_->onDisconnect(endpoint1_);
    }

    endpoint1_ = 0;
    endpoint2_ = 0;
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void EndpointConnection::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF pos = snapPointToGrid(mouseEvent->pos());

    if (pathPoints_.first() == pos) {
        selectionType_ = END;
        selected_ = 0;
    } else if (pathPoints_.last() == pos) {
        selectionType_ = END;
        selected_ = pathPoints_.size()-1;
    } else if (pathPoints_.size() > 1) {
        for (int i = 0; i < pathPoints_.size()-1; i++) {
            if ((i == 0 && endpoint1_) || (i == pathPoints_.size()-2 && endpoint2_))
                continue;
            if ((qFuzzyCompare(pathPoints_[i].x(), pos.x())
                && qFuzzyCompare(pathPoints_[i+1].x(), pos.x()))
                || (qFuzzyCompare(pathPoints_[i].y(), pos.y())
                && qFuzzyCompare(pathPoints_[i+1].y(), pos.y()))) {
                    selected_ = i;
                    selectionType_ = SEGMENT;
            }
        }
    } else {
        selected_ = -1;
        selectionType_ = NONE;
    }

    QGraphicsPathItem::mousePressEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void EndpointConnection::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF newPos = snapPointToGrid(mouseEvent->pos());

    if (selectionType_ == END)
    {
        // Disconnect the ends.
        disconnectEnds();

        if (pathPoints_.size() > 2) {
            if (selected_ == pathPoints_.size()-1) {
                if (qFuzzyCompare(pathPoints_[selected_-1].x(), pathPoints_[selected_-2].x())) {
                    pathPoints_[selected_-1].setY(newPos.y());
                } else {
                    pathPoints_[selected_-1].setX(newPos.x());
                }
            } else if (selected_ == 0) {
                if (qFuzzyCompare(pathPoints_[selected_+1].x(), pathPoints_[selected_+2].x())) {
                    pathPoints_[selected_+1].setY(newPos.y());
                } else {
                    pathPoints_[selected_+1].setX(newPos.x());
                }
            }
            pathPoints_[selected_] = newPos;
            setRoute(pathPoints_);
        } else {
            QPointF oldPos = pathPoints_[selected_];

            if (qFuzzyCompare(pathPoints_.first().x(), pathPoints_.last().x())) {
                pathPoints_[selected_].setY(newPos.y());
            } else {
                pathPoints_[selected_].setX(newPos.x());
            }
            if (pathPoints_.first() == pathPoints_.last())
                pathPoints_[selected_] = oldPos;
            else
                setRoute(pathPoints_);
        }

        emit contentChanged();
    }
    else if (selectionType_ == SEGMENT)
    {
        if (qFuzzyCompare(pathPoints_[selected_].x(), pathPoints_[selected_+1].x())) {
            pathPoints_[selected_].setX(newPos.x());
            pathPoints_[selected_+1].setX(newPos.x());
        } else if (qFuzzyCompare(pathPoints_[selected_].y(), pathPoints_[selected_+1].y())) {
            pathPoints_[selected_].setY(newPos.y());
            pathPoints_[selected_+1].setY(newPos.y());
        }

        setRoute(pathPoints_);
        emit contentChanged();
    }

    QGraphicsPathItem::mouseMoveEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void EndpointConnection::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (selectionType_ == END)
    {
        EndpointItem* endpoint1 = DiagramUtil::snapToItem<EndpointItem>(pathPoints_.first(), scene(), GridSize);
        EndpointItem* endpoint2 = DiagramUtil::snapToItem<EndpointItem>(pathPoints_.last(), scene(), GridSize);

        if (endpoint1 != 0 && endpoint2 != 0 &&
            endpoint1->canConnect(endpoint2) && endpoint2->canConnect(endpoint1))
        {
            connectEnds();
        }
    }
    else if (selectionType_ == SEGMENT)
    {
        simplifyPath();
        setRoute(pathPoints_);
    }

    QGraphicsPathItem::mouseReleaseEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: paint()
//-----------------------------------------------------------------------------
void EndpointConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    QStyleOptionGraphicsItem myoption = (*option);
    myoption.state &= !QStyle::State_Selected;

    QPen curPen = pen();

    if (option->state & QStyle::State_Selected)
    {
        curPen.setColor(Qt::red);
    }
    else
    {
        curPen.setColor(Qt::black);
    }

    setPen(curPen);
    QGraphicsPathItem::paint(painter, &myoption, widget);

    if (!endpoint1_)
        painter->fillRect(QRectF(pathPoints_.first()-QPointF(2,2),
        pathPoints_.first()+QPointF(2,2)),
        QBrush(Qt::red));

    if (!endpoint2_)
        painter->fillRect(QRectF(pathPoints_.last()-QPointF(2,2),
        pathPoints_.last()+QPointF(2,2)),
        QBrush(Qt::red));
}

//-----------------------------------------------------------------------------
// Function: updateName()
//-----------------------------------------------------------------------------
void EndpointConnection::updateName()
{
    Q_ASSERT(endpoint1_ != 0);
    Q_ASSERT(endpoint2_ != 0);
    name_ = endpoint1_->getFullName() + "_to_" + endpoint2_->getFullName();
}
