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
#include "EndpointDesignDiagram.h"
#include "SystemMoveCommands.h"

#include <common/DiagramUtil.h>
#include <common/GenericEditProvider.h>

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
        QVector2D dir = QVector2D(path[1] - path[0]).normalized();

        // Switch the direction of the end point if it is not correct.
        if (QVector2D::dotProduct(dir, endpoint1_->getDirectionVector()) < 0)
        {
            if (dir.x() > 0)
            {
                endpoint1_->setDirection(EndpointItem::DIR_RIGHT);
            }
            else
            {
                endpoint1_->setDirection(EndpointItem::DIR_LEFT);
            }
        }
    }

    if (endpoint2_)
    {
        QVector2D dir = QVector2D(path[path.size() - 2] - path[path.size() - 1]).normalized();

        // Switch the direction of the end point if it is not correct.
        if (QVector2D::dotProduct(dir, endpoint2_->getDirectionVector()) < 0)
        {
            if (dir.x() > 0)
            {
                endpoint2_->setDirection(EndpointItem::DIR_RIGHT);
            }
            else
            {
                endpoint2_->setDirection(EndpointItem::DIR_LEFT);
            }
        }
    }

    QListIterator<QPointF> i(path);

    QPainterPath painterPath(i.next());

    while (i.hasNext()) {
        painterPath.lineTo(i.next());
    }

    QPainterPathStroker stroker;
    setPath(stroker.createStroke(painterPath));

    pathPoints_ = path;
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: updatePosition()
//-----------------------------------------------------------------------------
void EndpointConnection::updatePosition()
{
    QVector2D delta1 = QVector2D(endpoint1_->scenePos()) - QVector2D(pathPoints_.first());
    QVector2D delta2 = QVector2D(endpoint2_->scenePos()) - QVector2D(pathPoints_.last());
    QVector2D const& dir1 = endpoint1_->getDirectionVector();
    QVector2D const& dir2 = endpoint2_->getDirectionVector();

    // Recreate the route from scratch if there are not enough points in the path or
    // the route is too complicated when the position and direction of the endpoints is considered.
    if (pathPoints_.size() < 2 ||
        (pathPoints_.size() > 4 && qFuzzyCompare(QVector2D::dotProduct(dir1, dir2), -1.0) &&
        QVector2D::dotProduct(dir1, QVector2D(endpoint2_->scenePos() - endpoint1_->scenePos())) > 0.0))
    {
        createRoute(endpoint1_, endpoint2_);
        return;
    }

    // If the delta movement of both endpoints was the same, we can just
    // move all route points by the delta1.
    if (qFuzzyCompare(delta1, delta2))
    {
        if (!delta1.isNull())
        {
            for (int i = 0; i < pathPoints_.size(); ++i)
            {
                pathPoints_[i] += delta1.toPointF();
            }

            setRoute(pathPoints_);
        }
    }
    // Otherwise check if either the first or the last point was moved.
    else if (!delta1.isNull() || !delta2.isNull())
    {
        bool pathOk = false;
        QVector2D delta = delta1;
        QVector2D dir = dir1;
        EndpointItem* endpoint = endpoint1_;
        int index0 = 0;
        int index1 = 1;
        int index2 = 2;
        int index3 = 3;

        if (!delta2.isNull())
        {
            delta = delta2;
            endpoint = endpoint2_;
            dir = dir2;
            index0 = pathPoints_.size() - 1;
            index1 = pathPoints_.size() - 2;
            index2 = pathPoints_.size() - 3;
            index3 = pathPoints_.size() - 4;
        }

        QVector2D seg1 = QVector2D(pathPoints_[index1] - pathPoints_[index0]).normalized();

        // Check if the endpoints should be directed in another way.
        if (dir1.x() * (endpoint2_->scenePos().x() - endpoint1_->scenePos().x()) < 0.0 ||
            dir2.x() * (endpoint1_->scenePos().x() - endpoint2_->scenePos().x()) < 0.0)
        {
            pathOk = false;
        }
        // Try to fix the first segment with perpendicular projection.
        else if (pathPoints_.size() >= 4 && pathPoints_.size() < 7 && qFuzzyCompare(dir, seg1))
        {
            QVector2D perp = delta - QVector2D::dotProduct(delta, seg1) * seg1;
            pathPoints_[index1] += perp.toPointF();

            // The path is ok if the moved point is still in view (not behind the left edge).
            pathOk = pathPoints_[index1].x() >= 10.0;
        }

        // Handle the parallel part of the delta.
        pathPoints_[index0] = endpoint->scenePos();
        QVector2D newSeg1 = QVector2D(pathPoints_[index1] - pathPoints_[index0]);

        if (newSeg1.length() < MIN_START_LENGTH || !qFuzzyCompare(seg1, newSeg1.normalized()))
        {
            pathOk = false;
        }

        // Check for a special case when there would be intersecting parallel lines.
        if (pathOk && pathPoints_.size() >= 4)
        {
            QVector2D seg2 = QVector2D(pathPoints_[index2] - pathPoints_[index1]).normalized();
            QVector2D seg3 = QVector2D(pathPoints_[index3] - pathPoints_[index2]).normalized();

            if (QVector2D::dotProduct(seg1, seg2) < 0.0f ||
                (seg2.isNull() && QVector2D::dotProduct(seg1, seg3) < 0.0f))
            {
                pathOk = false;
            }
        }

        // Snap the middle path points to grid.
        for (int i = 1; i < pathPoints_.size() - 1; ++i)
        {
            pathPoints_[i] = snapPointToGrid(pathPoints_[i]);
        }

        // If the simple fix didn't result in a solution, just recreate the route.
        if (!pathOk)
        {
            createRoute(endpoint1_, endpoint2_);
        }
        else
        {
            //simplifyPath();
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
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void EndpointConnection::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    oldRoute_ = getRoute();
    QPointF pos = snapPointToGrid(mouseEvent->pos());

    if (pathPoints_.size() > 1)
    {
        for (int i = 0; i < pathPoints_.size()-1; i++)
        {
            if ((i == 0 && endpoint1_) || (i == pathPoints_.size()-2 && endpoint2_))
            {
                continue;
            }

            if ((qFuzzyCompare(pathPoints_[i].x(), pos.x()) && qFuzzyCompare(pathPoints_[i+1].x(), pos.x())) ||
                (qFuzzyCompare(pathPoints_[i].y(), pos.y()) && qFuzzyCompare(pathPoints_[i+1].y(), pos.y())))
            {
                    selected_ = i;
                    selectionType_ = SEGMENT;
            }
        }
    } else
    {
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

    if (selectionType_ == SEGMENT)
    {
        if (qFuzzyCompare(pathPoints_[selected_].x(), pathPoints_[selected_+1].x()))
        {
            qreal prev = pathPoints_[selected_ - 1].x();
            qreal next = pathPoints_[selected_ + 2].x();

            // If the connection is on the same side of both the endpoints, try to change
            // the path in a way that even the side can be changed from left-to-right or right-to-left.
            if (endpoint1_->getDirection() == endpoint2_->getDirection() &&
                abs(newPos.x() - prev) >= MIN_START_LENGTH && abs(newPos.x() - next) >= MIN_START_LENGTH)
            {
                pathPoints_[selected_].setX(std::max(newPos.x(), 10.0));
                pathPoints_[selected_ + 1].setX(std::max(newPos.x(), 10.0));
            }
            // Otherwise clamp the movement delta.
            else
            {
                qreal delta = newPos.x() - pathPoints_[selected_].x();
                qreal cur = pathPoints_[selected_].x();                

                if (cur > next)
                {
                    delta = std::max(delta, MIN_START_LENGTH + next - cur);
                }
                else
                {
                    delta = std::min(delta, -MIN_START_LENGTH + next - cur);
                }

                if (cur > prev)
                {
                    delta = std::max(delta, MIN_START_LENGTH + prev - cur);
                }
                else
                {
                    delta = std::min(delta, -MIN_START_LENGTH + prev - cur);
                }

                pathPoints_[selected_].setX(pathPoints_[selected_].x() + delta);
                pathPoints_[selected_+1].setX(pathPoints_[selected_+1].x() + delta);
            }
        }
        else if (qFuzzyCompare(pathPoints_[selected_].y(), pathPoints_[selected_+1].y()))
        {
            qreal prev = pathPoints_[selected_ - 1].y();
            qreal next = pathPoints_[selected_ + 2].y();

            // Change the route only if the next and previous segments would not be too short.
            if (abs(newPos.y() - prev) >= MIN_LENGTH && abs(newPos.y() - next) >= MIN_LENGTH)
            {
                pathPoints_[selected_].setY(newPos.y());
                pathPoints_[selected_+1].setY(newPos.y());
            }
        }

        setRoute(pathPoints_);
    }

    QGraphicsPathItem::mouseMoveEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void EndpointConnection::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (selectionType_ == SEGMENT)
    {
        simplifyPath();
        setRoute(pathPoints_);

        if (getRoute() != oldRoute_)
        {
            QSharedPointer<QUndoCommand> cmd(new EndpointConnectionMoveCommand(this, oldRoute_));
            static_cast<EndpointDesignDiagram*>(scene())->getEditProvider().addCommand(cmd);
            
            emit contentChanged();
        }
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

//-----------------------------------------------------------------------------
// Function: beginUpdatePosition()
//-----------------------------------------------------------------------------
void EndpointConnection::beginUpdatePosition()
{
    oldRoute_ = getRoute();
}

//-----------------------------------------------------------------------------
// Function: endUpdatePosition()
//-----------------------------------------------------------------------------
QUndoCommand* EndpointConnection::endUpdatePosition(QUndoCommand* parent)
{
    if (getRoute() != oldRoute_)
    {
        return new EndpointConnectionMoveCommand(this, oldRoute_, parent);
    }
    else
    {
        return 0;
    }
}