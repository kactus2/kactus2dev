/* 
 *
 * 		filename: diagraminterconnection.cpp
 */

#include "diagraminterconnection.h"
#include "diagramcomponent.h"
#include "DiagramConnectionEndpoint.h"
#include "DiagramMoveCommands.h"
#include "blockdiagram.h"
#include "designwidget.h"

#include <models/generaldeclarations.h>

#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>

#include <models/businterface.h>

#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>

#include <cmath>

qreal const DiagramInterconnection::MIN_LENGTH = 10;

//-----------------------------------------------------------------------------
// Function: DiagramInterconnection()
//-----------------------------------------------------------------------------
DiagramInterconnection::DiagramInterconnection(DiagramConnectionEndPoint *endPoint1,
                                               DiagramConnectionEndPoint *endPoint2,
                                               bool autoConnect,
                                               const QString &displayName,
                                               const QString &description,
                                               BlockDiagram* parent)
    : QGraphicsPathItem(), 
      parent_(parent),
      name_(), 
      description_(description),
      endPoint1_(0),
      endPoint2_(0), 
      pathPoints_(), 
      selected_(-1), 
      selectionType_(NONE),
      routingMode_(ROUTING_MODE_NORMAL),
      widthLabel_(0)
{
    setItemSettings();
    createRoute(endPoint1, endPoint2);

    if (autoConnect)
    {
        endPoint1_ = endPoint1;
        endPoint2_ = endPoint2;

        endPoint1->addInterconnection(this);
        endPoint2->addInterconnection(this);
        updateWidthLabel();
        updateName();
    }
}

//-----------------------------------------------------------------------------
// Function: DiagramInterconnection()
//-----------------------------------------------------------------------------
DiagramInterconnection::DiagramInterconnection(QPointF p1, QVector2D const& dir1,
                                               QPointF p2, QVector2D const& dir2,
                                               const QString &displayName,
											   const QString &description,
											   BlockDiagram* parent)
    : QGraphicsPathItem(),
      parent_(parent),
      name_(),
      description_(),
      endPoint1_(0), 
      endPoint2_(0),
      pathPoints_(),
      selected_(-1),
      selectionType_(NONE),
      routingMode_(ROUTING_MODE_NORMAL),
      widthLabel_(0)
{
	setItemSettings();
    createRoute(p1, p2, dir1, dir2);
}

//-----------------------------------------------------------------------------
// Function: ~DiagramInterconnection()
//-----------------------------------------------------------------------------
DiagramInterconnection::~DiagramInterconnection()
{
	disconnectEnds();
	emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: connectEnds()
//-----------------------------------------------------------------------------
bool DiagramInterconnection::connectEnds()
{
    if (!scene())
        return false;

    prepareGeometryChange();

    // Disconnect the previous ends.
    disconnectEnds();
    
    // Find the new end points.
    endPoint1_ = DiagramUtil::snapToItem<DiagramConnectionEndPoint>(pathPoints_.first(), scene(), GridSize);
    Q_ASSERT(endPoint1_ != 0);
    
    endPoint2_ = DiagramUtil::snapToItem<DiagramConnectionEndPoint>(pathPoints_.last(), scene(), GridSize);
    Q_ASSERT(endPoint2_ != 0);

    // Swap the end points in a way that the first one at least has an encompassing component.
    if (endPoint1_->encompassingComp() == 0)
    {
        std::swap(endPoint1_, endPoint2_);
        
        // The path points have to be reversed.
        for (int i = 0; i < pathPoints_.size() / 2; ++i)
        {
            pathPoints_.swap(i, pathPoints_.size() - 1 - i);
        }
    }
    
    // Make the connections and check for errors.
    if (!endPoint1_->onConnect(endPoint2_))
    {
        endPoint1_ = 0;
        return false;
    }
    
    if (!endPoint2_->onConnect(endPoint1_))
    {
        endPoint1_->onDisconnect(endPoint2_);
        endPoint1_ = 0;
        endPoint2_ = 0;
        return false;
    }

    endPoint1_->addInterconnection(this);
    endPoint2_->addInterconnection(this);

    // Check if both end points were found.
    if (endPoint1_ && endPoint2_) {
        simplifyPath();
        setRoute(pathPoints_);
        updateName();
        return true;
    }

    foreach (QGraphicsItem *item, scene()->items(pathPoints_.first())) {
        if (item == this)
            continue;

        if (item->type() == DiagramInterconnection::Type) {
            DiagramInterconnection *segment
                    = qgraphicsitem_cast<DiagramInterconnection *>(item);

            if (segment->route().last() == pathPoints_.first()) {
                QList<QPointF> newPath = segment->route();
                newPath.append(pathPoints_);
                pathPoints_ = newPath;
                scene()->removeItem(segment);
                delete segment;

                return connectEnds();
            } else if (segment->route().first() == pathPoints_.first()) {
                QList<QPointF> newPath = segment->route();
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

        if (item->type() == DiagramInterconnection::Type) {
            DiagramInterconnection *segment
                    = qgraphicsitem_cast<DiagramInterconnection *>(item);

            if (segment->route().first() == pathPoints_.last()) {
                pathPoints_.append(segment->route());
                scene()->removeItem(segment);
                delete segment;

                return connectEnds();
            } else if (segment->route().last() == pathPoints_.last()) {
                QList<QPointF> newPath = segment->route();
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

void DiagramInterconnection::setRoute(QList<QPointF> path)
{
    if (path.size() < 2)
        return;

    if (endPoint1_)
    {
        QVector2D dir = QVector2D(path[1] - path[0]).normalized();

        // Switch the direction of the end point if it is not correct.
        if (!endPoint1_->isDirectionFixed() && QVector2D::dotProduct(dir, endPoint1_->getDirection()) < 0)
        {
            endPoint1_->setDirection(dir);
        }
    }

    if (endPoint2_)
    {
        QVector2D dir = QVector2D(path[path.size() - 2] - path[path.size() - 1]).normalized();

        // Switch the direction of the end point if it is not correct.
        if (!endPoint2_->isDirectionFixed() && QVector2D::dotProduct(dir, endPoint2_->getDirection()) < 0)
        {
            endPoint2_->setDirection(dir);
        }
    }

    QListIterator<QPointF> i(path);

    QPainterPath painterPath(i.next());

    while (i.hasNext()) {
        painterPath.lineTo(i.next());
    }

    QPainterPathStroker stroker;
    setPath(stroker.createStroke(painterPath));

    if (path != pathPoints_)
    {
        pathPoints_ = path;
        emit contentChanged();
    }

    updateWidthLabel();
}

QString DiagramInterconnection::name() const
{
    return name_;
}

void DiagramInterconnection::setName( const QString& name ) {
	name_ = name;
	emit contentChanged();
}

QString DiagramInterconnection::description() const {
	return description_;
}

void DiagramInterconnection::setDescription( const QString& description ) {
	description_ = description;
	emit contentChanged();
}

DiagramConnectionEndPoint *DiagramInterconnection::endPoint1() const
{
    return endPoint1_;
}

DiagramConnectionEndPoint *DiagramInterconnection::endPoint2() const
{
    return endPoint2_;
}

void DiagramInterconnection::updatePosition()
{
    if (routingMode_ == ROUTING_MODE_NORMAL)
    {
        QVector2D delta1 = QVector2D(endPoint1_->scenePos()) - QVector2D(pathPoints_.first());
        QVector2D delta2 = QVector2D(endPoint2_->scenePos()) - QVector2D(pathPoints_.last());
        QVector2D const& dir1 = endPoint1_->getDirection();
        QVector2D const& dir2 = endPoint2_->getDirection();

        // Recreate the route from scratch if there are not enough points in the path or
        // the route is too complicated when the position and direction of the endpoints is considered.
        if (pathPoints_.size() < 2 ||
            (pathPoints_.size() > 4 && qFuzzyCompare(QVector2D::dotProduct(dir1, dir2), -1.0) &&
            QVector2D::dotProduct(dir1, QVector2D(endPoint2_->scenePos() - endPoint1_->scenePos())) > 0.0))
        {
            createRoute(endPoint1_, endPoint2_);
            updateWidthLabel();
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
            DiagramConnectionEndPoint* endPoint = endPoint1_;
            int index0 = 0;
            int index1 = 1;
            int index2 = 2;
            int index3 = 3;

            if (!delta2.isNull())
            {
                delta = delta2;
                endPoint = endPoint2_;
                dir = dir2;
                index0 = pathPoints_.size() - 1;
                index1 = pathPoints_.size() - 2;
                index2 = pathPoints_.size() - 3;
                index3 = pathPoints_.size() - 4;
            }

            QVector2D seg1 = QVector2D(pathPoints_[index1] - pathPoints_[index0]).normalized();

            // Try to fix the first segment with perpendicular projection.
            if (pathPoints_.size() >= 4 && pathPoints_.size() < 7 && qFuzzyCompare(dir, seg1))
            {
                QVector2D perp = delta - QVector2D::dotProduct(delta, seg1) * seg1;
                pathPoints_[index1] += perp.toPointF();

                // The path is ok if the moved point is still in view (not behind the left edge).
                pathOk = pathPoints_[index1].x() >= 10.0;
            }

            // Handle the parallel part of the delta.
            pathPoints_[index0] = endPoint->scenePos();
            QVector2D newSeg1 = QVector2D(pathPoints_[index1] - pathPoints_[index0]);
            
            if (newSeg1.length() < MIN_LENGTH || !qFuzzyCompare(seg1, newSeg1.normalized()))
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
                createRoute(endPoint1_, endPoint2_);
                updateWidthLabel();
            }
            else
            {
                simplifyPath();
                setRoute(pathPoints_);
            }
        }
    }
    else if (routingMode_ == ROUTING_MODE_OFFPAGE)
    {
        // Make a straight line from begin to end.
        QList<QPointF> route;
        route.append(endPoint1()->scenePos());
        route.append(endPoint2()->scenePos());

        setRoute(route);
    }

    emit contentChanged();
}

const QList<QPointF> &DiagramInterconnection::route()
{
    return pathPoints_;
}

void DiagramInterconnection::simplifyPath()
{
    if (pathPoints_.size() < 3)
        return;

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

void DiagramInterconnection::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF pos = snapPointToGrid(mouseEvent->pos());

    oldRoute_ = route();

    if (pathPoints_.first() == pos) {
        selectionType_ = END;
        selected_ = 0;
    } else if (pathPoints_.last() == pos) {
        selectionType_ = END;
        selected_ = pathPoints_.size()-1;
    } else if (pathPoints_.size() > 1) {
        for (int i = 0; i < pathPoints_.size()-1; i++) {
            if ((i == 0 && endPoint1_) || (i == pathPoints_.size()-2 && endPoint2_))
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

void DiagramInterconnection::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Discard mouse move if the diagram is protected.
    if (parent_->isProtected())
    {
        return;
    }

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
        if (qFuzzyCompare(pathPoints_[selected_].x(), pathPoints_[selected_+1].x()))
        {
            // Clamp the movement delta.
            qreal delta = newPos.x() - pathPoints_[selected_].x();
            qreal prev = pathPoints_[selected_ - 1].x();
            qreal cur = pathPoints_[selected_].x();
            qreal next = pathPoints_[selected_ + 2].x();

            if (cur > next)
            {
                delta = std::max(delta, GridSize + next - cur);
            }
            else
            {
                delta = std::min(delta, -GridSize + next - cur);
            }

            if (cur > prev)
            {
                delta = std::max(delta, GridSize + prev - cur);
            }
            else
            {
                delta = std::min(delta, -GridSize + prev - cur);
            }

            pathPoints_[selected_].setX(pathPoints_[selected_].x() + delta);
            pathPoints_[selected_+1].setX(pathPoints_[selected_+1].x() + delta);
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
        emit contentChanged();
    }

    QGraphicsPathItem::mouseMoveEvent(mouseEvent);
}

void DiagramInterconnection::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (selectionType_ == END)
    {
        DiagramConnectionEndPoint* endPoint1 =
            DiagramUtil::snapToItem<DiagramConnectionEndPoint>(pathPoints_.first(), scene(), GridSize);
        DiagramConnectionEndPoint* endPoint2 =
            DiagramUtil::snapToItem<DiagramConnectionEndPoint>(pathPoints_.last(), scene(), GridSize);

        if (endPoint1 != 0 && endPoint2 != 0 &&
            endPoint1->canConnect(endPoint2) && endPoint2->canConnect(endPoint1))
        {
            connectEnds();
        }
    }
    else if (selectionType_ == SEGMENT)
    {
        simplifyPath();
        setRoute(pathPoints_);
    }

    if (route() != oldRoute_)
    {
        QSharedPointer<QUndoCommand> cmd(new ConnectionMoveCommand(this, oldRoute_));
        parent_->getEditProvider().addCommand(cmd);
    }

    QGraphicsPathItem::mouseReleaseEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: paint()
//-----------------------------------------------------------------------------
void DiagramInterconnection::paint(QPainter *painter,
                                   const QStyleOptionGraphicsItem *option,
                                   QWidget *widget)
{
    bool selected = option->state & QStyle::State_Selected;
    if (selected)
    {
        drawOverlapGraphics(painter);
    }

    QStyleOptionGraphicsItem myoption = (*option);
    myoption.state &= !QStyle::State_Selected;

    QGraphicsPathItem::paint(painter, &myoption, widget);

    if (!selected)
    {
        drawOverlapGraphics(painter);
    }


    if (!endPoint1_)
        painter->fillRect(QRectF(pathPoints_.first()-QPointF(2,2),
                                 pathPoints_.first()+QPointF(2,2)),
                          QBrush(Qt::red));

    if (!endPoint2_)
        painter->fillRect(QRectF(pathPoints_.last()-QPointF(2,2),
                                 pathPoints_.last()+QPointF(2,2)),
                          QBrush(Qt::red));
}

//-----------------------------------------------------------------------------
// Function: setItemSettings()
//-----------------------------------------------------------------------------
void DiagramInterconnection::setItemSettings()
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
void DiagramInterconnection::createRoute(DiagramConnectionEndPoint* endPoint1,
                                         DiagramConnectionEndPoint* endPoint2)
{
    Q_ASSERT(endPoint1 != 0);
    Q_ASSERT(endPoint2 != 0);
    
    QPointF p1 = endPoint1->scenePos();
    QPointF p2 = endPoint2->scenePos();
    
    if (!endPoint1->isDirectionFixed())
    {
        if (p1.x() <= p2.x())
        {
            endPoint1->setDirection(QVector2D(1.0f, 0.0f));
        }
        else
        {
            endPoint1->setDirection(QVector2D(-1.0f, 0.0f));
        }
    }

    if (!endPoint2->isDirectionFixed())
    {
        if (p1.x() <= p2.x())
        {
            endPoint2->setDirection(QVector2D(-1.0f, 0.0f));
        }
        else
        {
            endPoint2->setDirection(QVector2D(1.0f, 0.0f));
        }
    }

    createRoute(p1, p2, endPoint1->getDirection(), endPoint2->getDirection());
}

//-----------------------------------------------------------------------------
// Function: createRoute()
//-----------------------------------------------------------------------------
void DiagramInterconnection::createRoute(QPointF p1, QPointF p2,
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
    QVector2D targetPos = QVector2D(p2) + dir2 * MIN_LENGTH;

    // Add the start position to the list of path points.
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
                        // or at least the minimum length.
                        curPos = curPos + curDir * std::max(MIN_LENGTH, proj.length());
                    }
                    else
                    {
                        // Otherwise we just draw the minimum length thub.
                        curPos = curPos + curDir * MIN_LENGTH;
                    }
                }
                // Check if the target is in the opposite direction compared to the current
                // direction and we previously draw the starting thub.
                else if (dot < 0 && qFuzzyCompare(curPos, startPos + curDir * MIN_LENGTH))
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
            pathPoints_ << curPos.toPointF();
        }

        // Add the last segment if the target position was not the end position.
        if (!qFuzzyCompare(targetPos, endPos))
        {
            pathPoints_ << endPos.toPointF();
        }
    }

    simplifyPath();

    QListIterator<QPointF> i(pathPoints_);

    QPainterPath path(i.next());

    while (i.hasNext()) {
        path.lineTo(i.next());
    }

    QPainterPathStroker stroker;
    setPath(stroker.createStroke(path));
}

//-----------------------------------------------------------------------------
// Function: updateName()
//-----------------------------------------------------------------------------
void DiagramInterconnection::updateName()
{
    Q_ASSERT(endPoint1_ != 0);
    Q_ASSERT(endPoint2_ != 0);

    // Determine one of the end points as the starting point in a way that its
    // encompassing component is defined.
    DiagramConnectionEndPoint* start = endPoint1_;
    DiagramConnectionEndPoint* end = endPoint2_;

    if (start->encompassingComp() == 0)
    {
        std::swap(start, end);
        Q_ASSERT(start->encompassingComp() != 0);
    }

    // Update the name string.
    QString endCompName = "";
    
    if (end->encompassingComp() != 0)
    {
        endCompName = end->encompassingComp()->name() + "_";
    }

    // TODO: Problems?
    if (start->getBusInterface() != 0 && end->getBusInterface() != 0)
    {
        name_ = start->encompassingComp()->name() + "_" + start->getBusInterface()->getName() + "_to_" +
                endCompName + end->getBusInterface()->getName();
    }
    else
    {
    }
}

//-----------------------------------------------------------------------------
// Function: disconnectEnds()
//-----------------------------------------------------------------------------
void DiagramInterconnection::disconnectEnds()
{
    // Discard existing connections.
    if (endPoint1_)
    {
        endPoint1_->removeInterconnection(this);
        endPoint1_->onDisconnect(endPoint2_);
    }

    if (endPoint2_)
    {
        endPoint2_->removeInterconnection(this);
        endPoint2_->onDisconnect(endPoint1_);
    }

    emit contentChanged();
    endPoint1_ = 0;
    endPoint2_ = 0;
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant DiagramInterconnection::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
    case ItemSelectedHasChanged:
        {
            bool selected = value.toBool();
            
            if (selected)
            {
                QPen curPen = pen();
                curPen.setColor(Qt::red);
                setPen(curPen);
            }
            else
            {
                setDefaultColor();
            }

            return value;
        }
    }

    return QGraphicsPathItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: beginUpdatePosition()
//-----------------------------------------------------------------------------
void DiagramInterconnection::beginUpdatePosition()
{
    oldRoute_ = route();
}

//-----------------------------------------------------------------------------
// Function: endUpdatePosition()
//-----------------------------------------------------------------------------
QUndoCommand* DiagramInterconnection::endUpdatePosition(QUndoCommand* parent)
{
    if (route() != oldRoute_)
    {
        return new ConnectionMoveCommand(this, oldRoute_, parent);
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: setRoutingMode()
//-----------------------------------------------------------------------------
void DiagramInterconnection::setRoutingMode(RoutingMode style)
{
    if (routingMode_ != style)
    {
        routingMode_ = style;

        setDefaultColor();
        updatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: setDefaultColor()
//-----------------------------------------------------------------------------
void DiagramInterconnection::setDefaultColor()
{
    QPen newPen = pen();

    if (routingMode_ == ROUTING_MODE_NORMAL)
    {
        newPen.setColor(Qt::black);
    }
    else
    {
        newPen.setColor(QColor(180, 180, 180));
    }

    setPen(newPen);
}

//-----------------------------------------------------------------------------
// Function: updateWidthLabel()
//-----------------------------------------------------------------------------
void DiagramInterconnection::updateWidthLabel()
{
    //simplifyPath();

    // Calculate the total width over all port maps.
    int totalWidth = 0;

    foreach (QSharedPointer<General::PortMap> portMap1, endPoint1_->getBusInterface()->getPortMaps())
    {
        // Find the port map with the same logical port name from the other end point's port map.
        QSharedPointer<General::PortMap> portMap2;

        foreach (QSharedPointer<General::PortMap> portMap, endPoint2_->getBusInterface()->getPortMaps())
        {
            if (portMap->logicalPort_ == portMap1->logicalPort_)
            {
                portMap2 = portMap;
            }
        }

        if (portMap2 == 0)
        {
            continue;
        }

        Port* port1 = endPoint1_->ownerComponent()->getPort(portMap1->physicalPort_);
        Port* port2 = endPoint2_->ownerComponent()->getPort(portMap2->physicalPort_);

        if (port1 == 0 || port2 == 0)
        {
            // TODO: Error handling.
            continue;
        }

        // Calculate the intersection of the port bounds and add it to the total width.
        General::PortAlignment align =
            General::calculatePortAlignment(portMap1.data(), port1->getLeftBound(), port1->getRightBound(),
                                            portMap2.data(), port2->getLeftBound(), port2->getRightBound());

        int width = std::min(align.port1Left_, align.port2Left_) -
                    std::max(align.port1Right_, align.port2Right_) + 1;
        totalWidth += width;
    }

    if (widthLabel_ == 0)
    {
        widthLabel_ = new QGraphicsTextItem(QString::number(totalWidth), this);

        QFont font = widthLabel_->font();
        font.setPointSize(8);
        font.setBold(true);
        widthLabel_->setFont(font);

        DesignWidget* designWidget = parent_->parent();
        widthLabel_->setVisible(designWidget->getVisibilityControls().value("Bus Widths"));
    }
    else
    {
        widthLabel_->setPlainText(QString::number(totalWidth));
    }

    QPointF textPos = boundingRect().center();

    if (routingMode_ == ROUTING_MODE_OFFPAGE)
    {
        textPos.setX(textPos.x() - widthLabel_->boundingRect().width() * 0.5);
        textPos.setY(textPos.y() - 2 * GridSize);
    }
    else
    {
        // If the end points are move apart in horizontal direction, place the text above
        // the longest horizontal segment.
        if (std::abs(endPoint1_->scenePos().x() - endPoint2_->scenePos().x()) >=
            std::abs(endPoint1_->scenePos().y() - endPoint2_->scenePos().y()))
        {
            // Determine the longest horizontal segment.
            int longestIndex = 0;
            qreal longestLength = 0.0;

            for (int i = 0; i < pathPoints_.size() - 1; ++i)
            {
                QPointF const& pt1 = pathPoints_[i];
                QPointF const& pt2 = pathPoints_[i + 1];
                qreal length = QVector2D(pt2 - pt1).length();

                if (qFuzzyCompare(pt1.y(), pt2.y()) && length > longestLength)
                {
                    longestLength = length;
                    longestIndex = i;
                }
            }

            QPointF const& pt1 = pathPoints_[longestIndex];
            QPointF const& pt2 = pathPoints_[longestIndex + 1];

            textPos.setX((pt1.x() + pt2.x()) * 0.5 - widthLabel_->boundingRect().width() * 0.5);
            textPos.setY(pt1.y() - 2 * GridSize);
        }
        // Otherwise place the text beside the longest vertical segment.
        else
        {
            // Determine the longest vertical segment.
            int longestIndex = 0;
            qreal longestLength = 0.0;

            for (int i = 0; i < pathPoints_.size() - 1; ++i)
            {
                QPointF const& pt1 = pathPoints_[i];
                QPointF const& pt2 = pathPoints_[i + 1];
                qreal length = QVector2D(pt2 - pt1).length();

                if (qFuzzyCompare(pt1.x(), pt2.x()) && length > longestLength)
                {
                    longestLength = length;
                    longestIndex = i;
                }
            }

            Q_ASSERT(longestIndex > 0 && longestIndex + 2 < pathPoints_.size());
            QPointF const& pt1 = pathPoints_[longestIndex];
            QPointF const& pt2 = pathPoints_[longestIndex + 1];

            // Place the text on the right side if the connection is fully on the left.
            if (pathPoints_[longestIndex - 1].x() < pt1.x() && pathPoints_[longestIndex + 2].x() < pt1.x())
            {
                textPos.setX(pt1.x() + GridSize);
            }
            // Place the text on the left side if the connection is fully on the right.
            else if (pathPoints_[longestIndex - 1].x() > pt1.x() && pathPoints_[longestIndex + 2].x() > pt1.x())
            {
                textPos.setX(pt1.x() - widthLabel_->boundingRect().width());
            }
            else
            {
                // Otherwise we have to determine on which side there is more space.
                if (QVector2D(pathPoints_[longestIndex - 1] - pt1).lengthSquared() <
                    QVector2D(pathPoints_[longestIndex + 2] - pt2).lengthSquared())
                {
                    textPos.setX(pt1.x() + GridSize);
                }
                else
                {
                    textPos.setX(pt1.x() - widthLabel_->boundingRect().width());
                }
            }

            textPos.setY((pt1.y() + pt2.y()) * 0.5 - widthLabel_->boundingRect().height() * 0.5);
        }
    }

    widthLabel_->setPos(textPos);
}

//-----------------------------------------------------------------------------
// Function: setBusWidthVisible()
//-----------------------------------------------------------------------------
void DiagramInterconnection::setBusWidthVisible(bool visible)
{
    widthLabel_->setVisible(visible);
}

//-----------------------------------------------------------------------------
// Function: drawLineGap()
//-----------------------------------------------------------------------------
void DiagramInterconnection::drawLineGap(QPainter* painter, QLineF const& line1, QPointF const& pt)
{
    QVector2D dir(line1.dx(), line1.dy());
    dir.normalize();

    qreal length1 = QVector2D(pt - line1.p1()).length();
    qreal length2 = QVector2D(pt - line1.p2()).length();

    QPointF pt1 = (QVector2D(pt) + dir * std::min(length2, (qreal)GridSize)).toPointF();
    QPointF pt2 = (QVector2D(pt) - dir * std::min(length1, (qreal)GridSize)).toPointF();
    painter->drawLine(pt1, pt2);
}

//-----------------------------------------------------------------------------
// Function: drawOverlapGraphics()
//-----------------------------------------------------------------------------
void DiagramInterconnection::drawOverlapGraphics(QPainter* painter)
{
    // Determine all items that collide with this item.
    foreach (QGraphicsItem* item, collidingItems())
    {
        QList<QPointF> const& route1 = pathPoints_;

        // Paint junction marks to those parts that go vertically and cross another connection.
        if (item->type() == DiagramInterconnection::Type)
        {
            DiagramInterconnection* conn = static_cast<DiagramInterconnection*>(item);
            QList<QPointF> const& route2 = conn->route();

            for (int i = 0; i < route1.size() - 1; ++i)
            {
                // Discard horizontal segments.
                if (qFuzzyCompare(route1[i].y(), route1[i + 1].y()))
                {
                    continue;
                }

                QLineF line1(route1[i], route1[i + 1]);

                for (int j = 0; j < route2.size() - 1; ++j)
                {
                    // Discard vertical segments from the intersecting connections.
                    if (qFuzzyCompare(route2[j].x(), route2[j + 1].x()))
                    {
                        continue;
                    }

                    QLineF line2(route2[j], route2[j + 1]);

                    QPointF pt;
                    QLineF::IntersectType type = line1.intersect(line2, &pt);

                    if (type == QLineF::BoundedIntersection)
                    {
                        // If the connections share an endpoint, draw a black junction circle.
                        if (endPoint1() == conn->endPoint1() || endPoint2() == conn->endPoint2() ||
                            endPoint1() == conn->endPoint2() || endPoint2() == conn->endPoint1())
                        {
                            painter->setPen(QPen(Qt::black, 0));

                            QPainterPath circlePath;
                            circlePath.addEllipse(pt, 5.0, 5.0);

                            painter->fillPath(circlePath, QBrush(Qt::black));

                        }
                        else
                        {
                            // Otherwise draw a gray "tunnel" line close to the intersection point.
                            // Drawing is performed using two lines, excluding the area close to
                            // the intersection point. This way the drawing is done correctly even though
                            // the connection is above the other connection.
                            QVector2D dir(line1.dx(), line1.dy());
                            dir.normalize();

                            qreal length1 = QVector2D(pt - line1.p1()).length();
                            qreal length2 = QVector2D(pt - line1.p2()).length();

                            QPointF seg1Pt1 = (QVector2D(pt) + dir * std::min(length2, 4.0)).toPointF();
                            QPointF seg1Pt2 = (QVector2D(pt) + dir * std::min(length2, (qreal)GridSize)).toPointF();

                            QPointF seg2Pt1 = (QVector2D(pt) - dir * std::min(length1, 4.0)).toPointF();
                            QPointF seg2Pt2 = (QVector2D(pt) - dir * std::min(length1, (qreal)GridSize)).toPointF();

                            painter->setPen(QPen(QColor(160, 160, 160), 4));
                            painter->drawLine(seg1Pt1, seg1Pt2);
                            painter->drawLine(seg2Pt1, seg2Pt2);
                        }
                    }
                }
            }
        }
        else if (item->type() == DiagramComponent::Type)
        {
            DiagramComponent* comp = static_cast<DiagramComponent*>(item);

            // Create the line object for each edge of the diagram component rectangle.
            QLineF leftEdge(comp->rect().topLeft() + comp->scenePos(),
                comp->rect().bottomLeft() + comp->scenePos());
            QLineF rightEdge(comp->rect().topRight() + comp->scenePos(),
                comp->rect().bottomRight() + comp->scenePos());
            QLineF topEdge(comp->rect().topLeft() + comp->scenePos(),
                comp->rect().topRight() + comp->scenePos());
            QLineF bottomEdge(comp->rect().bottomLeft() + comp->scenePos(),
                comp->rect().bottomRight() + comp->scenePos());

            for (int i = 0; i < route1.size() - 1; ++i)
            {
                QLineF line1(route1[i], route1[i + 1]);

                // Check if the line segment intersects both parallel lines (either vertical or horizontal).
                QPointF pt, pt2;
                QLineF::IntersectType type1 = line1.intersect(leftEdge, &pt);
                QLineF::IntersectType type2 = line1.intersect(rightEdge, &pt2);

                if (type1 == QLineF::BoundedIntersection && type2 == QLineF::BoundedIntersection)
                {
                    painter->drawLine(pt, pt2);
                    drawLineGap(painter, line1, pt);
                    drawLineGap(painter, line1, pt2);
                    continue;
                }

                type1 = line1.intersect(topEdge, &pt);
                type2 = line1.intersect(bottomEdge, &pt2);

                if (type1 == QLineF::BoundedIntersection && type2 == QLineF::BoundedIntersection)
                {
                    painter->setPen(QPen(QColor(160, 160, 160), 4));
                    painter->drawLine(pt, pt2);
                    drawLineGap(painter, line1, pt);
                    drawLineGap(painter, line1, pt2);
                }
            }
        }
    }
}
