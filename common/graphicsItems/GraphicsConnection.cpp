//-----------------------------------------------------------------------------
// File: GraphicsConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 5.6.2012
//
// Description:
// Base class for graphical connections.
//-----------------------------------------------------------------------------

#include "GraphicsConnection.h"

#include "ComponentItem.h"
#include "ConnectionUndoCommands.h"

#include <common/GenericEditProvider.h>
#include <common/KactusColors.h>

#include <editors/HWDesign/OffPageConnectorItem.h>
#include <editors/common/DesignDiagram.h>
#include <editors/common/DiagramUtil.h>
#include <editors/common/DefaultRouting.h>

#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::GraphicsConnection()
//-----------------------------------------------------------------------------
GraphicsConnection::GraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
    QSharedPointer<ConnectionRoute> route, bool autoConnect, DesignDiagram* parent) :
    QGraphicsPathItem(),
    Associable(),
    parent_(parent),
    endpoint1_(endpoint1),
    endpoint2_(endpoint2),
    route_(route)
{
    setItemSettings();
    setAcceptHoverEvents(true);

    if (autoConnect)
    {
        endpoint1_->onConnect(endpoint2_);
        endpoint2_->onConnect(endpoint1_);

        validate();

        endpoint1_->addConnection(this);
        endpoint2_->addConnection(this);

        if (route_->name().isEmpty())
        {
            route_->setName(createDefaultName());
        }
    }

    if (route_->getRoute().isEmpty())
    {
        createRoute(endpoint1_, endpoint2_);
    }
    else
    {
        setRoute(route_->getRoute());
    }

    setDefaultColor();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::GraphicsConnection()
//-----------------------------------------------------------------------------
GraphicsConnection::GraphicsConnection(QPointF const& p1, QVector2D const& dir1,
                                       QPointF const& p2, QVector2D const& dir2,
                                       DesignDiagram* parent):
    QGraphicsPathItem(), parent_(parent)
{
    setItemSettings();
    pathPoints_ = DefaultRouting::createRoute(p1, p2, dir1, dir2);
    setRoute(pathPoints_);        
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::~GraphicsConnection()
//-----------------------------------------------------------------------------
GraphicsConnection::~GraphicsConnection()
{
    disconnectEnds();
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::connectEnds()
//-----------------------------------------------------------------------------
bool GraphicsConnection::connectEnds()
{
    if (!scene())
    {
        return false;
    }

    prepareGeometryChange();

    // Disconnect the previous ends.
    disconnectEnds();

    // Find the new end points.
    endpoint1_ = DiagramUtil::snapToItem<ConnectionEndpoint>(pathPoints_.first(), scene(), GridSize);
    Q_ASSERT(endpoint1_ != nullptr);

    endpoint2_ = DiagramUtil::snapToItem<ConnectionEndpoint>(pathPoints_.last(), scene(), GridSize);
    Q_ASSERT(endpoint2_ != nullptr);

    // Swap the end points in a way that the first one at least has an encompassing component.
    if (endpoint1_->encompassingComp() == nullptr && endpoint2_->encompassingComp() != nullptr)
    {
        std::swap(endpoint1_, endpoint2_);

        // The path points have to be reversed.
        for (int i = 0; i < pathPoints_.size() / 2; ++i)
        {
            pathPoints_.swapItemsAt(i, pathPoints_.size() - 1 - i);
        }
    }

    // Make the connections and check for errors.
    if (!endpoint1_->onConnect(endpoint2_))
    {
        endpoint1_ = nullptr;
        return false;
    }

    if (!endpoint2_->onConnect(endpoint1_))
    {
        endpoint1_->onDisconnect();
        endpoint1_ = nullptr;
        endpoint2_ = nullptr;
        return false;
    }

    validate();

    endpoint1_->addConnection(this);
    endpoint2_->addConnection(this);

    setRoute(pathPoints_);

    updatePosition();

    route_->setName(createDefaultName());

    return true;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setRoute()
//-----------------------------------------------------------------------------
void GraphicsConnection::setRoute(QList<QPointF> path)
{
    if (path.size() < 2)
    {
        return;
    }

    QVector2D dir1 = QVector2D(path.at(1) - path.first()).normalized();
    updateEndpointDirection(endpoint1_, dir1);

    QVector2D dir2 = QVector2D(path.at(path.size() - 2) - path.last()).normalized();
    updateEndpointDirection(endpoint2_, dir2);

    pathPoints_ = path;
    pathLines_ = pointsToLines(pathPoints_);

    route_->setRoute(path);

    paintConnectionPath();
    positionUpdated();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setName()
//-----------------------------------------------------------------------------
void GraphicsConnection::setName(QString const& name)
{
    route_->setName(name);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::endpoint1()
//-----------------------------------------------------------------------------
ConnectionEndpoint* GraphicsConnection::endpoint1() const
{
    return endpoint1_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::endpoint2()
//-----------------------------------------------------------------------------
ConnectionEndpoint* GraphicsConnection::endpoint2() const
{
    return endpoint2_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::updatePosition()
//-----------------------------------------------------------------------------
void GraphicsConnection::updatePosition()
{
    if (routingMode_ == ROUTING_MODE_NORMAL)
    {
        QVector2D startDelta = QVector2D(endpoint1_->scenePos()) - QVector2D(pathPoints_.first());
        QVector2D endDelta = QVector2D(endpoint2_->scenePos()) - QVector2D(pathPoints_.last());
        QVector2D const& startDirection = endpoint1_->getDirection();
        QVector2D const& endDirection = endpoint2_->getDirection();

        // Recreate the route from scratch if there are not enough points in the path, the route is too 
        // complicated considering the position and direction of the endpoints, or the delta is away from the ends.
        if (pathPoints_.size() < 2 ||
            (pathPoints_.size() > 4 && qFuzzyCompare(QVector2D::dotProduct(startDirection, endDirection), -1.0f) &&
            QVector2D::dotProduct(startDirection, QVector2D(endpoint2_->scenePos() - endpoint1_->scenePos())) > 0.0) ||
            startDelta.x() * startDirection.x() < 0.0f || endDelta.x() * endDirection.x() < 0.0f)
        {
            createRoute(endpoint1_, endpoint2_);
            return;
        }

        // If the delta movement of both endpoints was the same, we can just move all route points by the delta1.
        if (!startDelta.isNull() && qFuzzyCompare(startDelta, endDelta))
        {
            for (int i = 0; i < pathPoints_.size(); ++i)
            {
                pathPoints_[i] += startDelta.toPointF();
            }

            setRoute(pathPoints_);
        }
        // Otherwise check if either the first or the last point was moved.
        else if (!startDelta.isNull() || !endDelta.isNull())
        {
            QVector2D delta = startDelta;
            QVector2D dir = startDirection;
            QPointF endpointPosition = endpoint1_->scenePos();
            int index0 = 0;
            int index1 = 1;
            int index2 = 2;
            int index3 = 3;

            if (!endDelta.isNull())
            {
                delta = endDelta;
                endpointPosition = endpoint2_->scenePos();
                dir = endDirection;
                index0 = pathPoints_.size() - 1;
                index1 = pathPoints_.size() - 2;
                index2 = pathPoints_.size() - 3;
                index3 = pathPoints_.size() - 4;
            }

            QVector2D seg1 = QVector2D(pathPoints_.at(index1) - pathPoints_.at(index0)).normalized();

            bool pathOk = false;
            // Try to fix the first segment with perpendicular projection.
            // The path is ok if the moved point is still in view (not behind the left edge).
            if (pathPoints_.size() >= 4 && pathPoints_.size() < 7 && qFuzzyCompare(dir, seg1))
            {
                QVector2D perp = delta - QVector2D::dotProduct(delta, seg1) * seg1;
                pathPoints_[index1] += perp.toPointF();

                pathOk = pathPoints_.at(index1).x() >= 10.0;
            }

            // Handle the parallel part of the delta.
            pathPoints_[index0] = endpointPosition;
            QVector2D newSeg1 = QVector2D(pathPoints_.at(index1) - pathPoints_.at(index0));

            if (newSeg1.length() < DefaultRouting::MIN_START_LENGTH || !qFuzzyCompare(seg1, newSeg1.normalized()))
            {
                pathOk = false;
            }

            // Check for a special case when there would be intersecting parallel lines.
            if (pathOk && pathPoints_.size() >= 4 ) 
            {
                QVector2D seg2 = QVector2D(pathPoints_.at(index2) - pathPoints_.at(index1)).normalized();
                QVector2D seg3 = QVector2D(pathPoints_.at(index3) - pathPoints_.at(index2)).normalized();
              
                if (QVector2D::dotProduct(seg1, seg2) < 0.0f ||
                    (seg2.isNull() && QVector2D::dotProduct(seg1, seg3) < 0.0f))
                {
                     pathOk = false;
                }               
            }

            if (pathOk)
            {                
                for (int i = 1; i < pathPoints_.size() - 1; i++)
                {
                    pathPoints_[i] = snapPointToGrid(pathPoints_.at(i));
                }

                setRoute(pathPoints_);
            }
            else //<! If the simple fix didn't result in a solution, just recreate the route.
            {
                createRoute(endpoint1_, endpoint2_);
            }
        }
    }
    else if (routingMode_ == ROUTING_MODE_OFFPAGE)
    {
        // Make a straight line from begin to end.
        QList<QPointF> route;
        route.append(endpoint1()->scenePos());
        route.append(endpoint2()->scenePos());

        setRoute(route);
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::route()
//-----------------------------------------------------------------------------
QList<QPointF> GraphicsConnection::route() const
{
    return pathPoints_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::mousePressEvent()
//-----------------------------------------------------------------------------
void GraphicsConnection::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    beginUpdatePosition();
    
    selected_ = -1;
    selectionType_ = NONE;

    QPointF pos = snapPointToGrid(mouseEvent->pos());    
    if (pathPoints_.first() == pos)
    {
        selectionType_ = END;
        selected_ = 0;
    }
    else if (pathPoints_.last() == pos)
    {
        selectionType_ = END;
        selected_ = pathPoints_.size()-1;
    }
    else if (pathPoints_.size() > 1)
    {
        for (int i = 0; i < pathPoints_.size()-1; i++)
        {
            if ((i == 0 && endpoint1_) || (i == pathPoints_.size()-2 && endpoint2_))
            {
                continue;
            }

            if ((qFuzzyCompare(pathPoints_.at(i).x(), pos.x()) && qFuzzyCompare(pathPoints_.at(i+1).x(), pos.x())) ||
                (qFuzzyCompare(pathPoints_.at(i).y(), pos.y()) && qFuzzyCompare(pathPoints_.at(i+1).y(), pos.y())))
            {
                selected_ = i;
                selectionType_ = SEGMENT;
            }
        }
    }

    QGraphicsPathItem::mousePressEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::mouseMoveEvent()
//-----------------------------------------------------------------------------
void GraphicsConnection::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{   
    // Discard mouse move if the diagram is protected.
    if (parent_->isProtected())
    {
        return;
    }

    QPointF newPos = snapPointToGrid(mouseEvent->pos());
    if (selectionType_ == END)
    {
        disconnectEnds();

        if (pathPoints_.first() != newPos || newPos != pathPoints_.last())
        {
            pathPoints_[selected_] = newPos;       

            if (pathPoints_.size() > 2)
            {
                int adjacent = selected_ + 1;
                int next = selected_ + 2;

                if (selected_ == pathPoints_.size() - 1)
                {
                    adjacent = selected_ - 1;
                    next = selected_ - 2;
                }

                // Adjust also the adjacent point to the same x- or y-axis.
                if (qFuzzyCompare(pathPoints_.at(adjacent).x(), pathPoints_.at(next).x()))
                {
                    pathPoints_[adjacent].setY(newPos.y());
                }
                else
                {
                    pathPoints_[adjacent].setX(newPos.x());
                }
            }

            setRoute(pathPoints_);
        }
    }
    else if (selectionType_ == SEGMENT)
    {
        if (qFuzzyCompare(pathPoints_.at(selected_).x(), pathPoints_.at(selected_ + 1).x()))
        {
            // Clamp the movement delta.
            qreal delta = newPos.x() - pathPoints_.at(selected_).x();
            qreal prev = pathPoints_.at(selected_ - 1).x();
            qreal cur = pathPoints_.at(selected_).x();
            qreal next = pathPoints_.at(selected_ + 2).x();

            if (cur > next)
            {
                delta = qMax(delta, DefaultRouting::MIN_START_LENGTH + next - cur);
            }
            else
            {
                delta = qMin(delta, -DefaultRouting::MIN_START_LENGTH + next - cur);
            }

            if (cur > prev)
            {
                delta = qMax(delta, DefaultRouting::MIN_START_LENGTH + prev - cur);
            }
            else
            {
                delta = qMin(delta, -DefaultRouting::MIN_START_LENGTH + prev - cur);
            }

            pathPoints_[selected_].setX(pathPoints_.at(selected_).x() + delta);
            pathPoints_[selected_+1].setX(pathPoints_.at(selected_+1).x() + delta);
        }
        else if (qFuzzyCompare(pathPoints_.at(selected_).y(), pathPoints_.at(selected_+1).y()))
        {
            qreal prev = pathPoints_.at(selected_ - 1).y();
            qreal next = pathPoints_.at(selected_ + 2).y();

            // Change the route only if the next and previous segments would not be too short.
            if (qAbs(newPos.y() - prev) >= DefaultRouting::MIN_LENGTH &&
                qAbs(newPos.y() - next) >= DefaultRouting::MIN_LENGTH)
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
// Function: GraphicsConnection::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void GraphicsConnection::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (selectionType_ == END)
    {
        auto endpoint1 = DiagramUtil::snapToItem<ConnectionEndpoint>(pathPoints_.first(), scene(), GridSize);
        auto endpoint2 = DiagramUtil::snapToItem<ConnectionEndpoint>(pathPoints_.last(), scene(), GridSize);

        if (endpoint1 != nullptr && endpoint2 != nullptr &&
            endpoint1->canConnect(endpoint2) && endpoint2->canConnect(endpoint1))
        {
            connectEnds();
        }
    }
    else if (selectionType_ == SEGMENT && !parent_->isProtected())
    {
        fixOverlap();
        setRoute(pathPoints_);
    }

    if (route() != oldRoute_)
    {
        QSharedPointer<QUndoCommand> cmd(new ConnectionMoveCommand(this, oldRoute_));
        parent_->getEditProvider()->addCommand(cmd);
        cmd->redo();
    }

    QGraphicsPathItem::mouseReleaseEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::paint()
//-----------------------------------------------------------------------------
void GraphicsConnection::paint(QPainter* painter, QStyleOptionGraphicsItem const* option, QWidget* widget)
{
    bool selected = option->state & QStyle::State_Selected;

    QStyleOptionGraphicsItem myoption = *option;
    myoption.state &= !QStyle::State_Selected;

    QGraphicsPathItem::paint(painter, &myoption, widget);

    if (!selected && !positionUpdateInProcess_ && routingMode_ == ROUTING_MODE_NORMAL)
    {
        drawOverlapGraphics(painter);
    }

    if (!endpoint1_)
    {
        painter->fillRect(QRectF(pathPoints_.first() - QPointF(2,2), pathPoints_.first() + QPointF(2,2)),
            QBrush(Qt::red));
    }

    if (!endpoint2_)
    {
        painter->fillRect(QRectF(pathPoints_.last() - QPointF(2,2), pathPoints_.last() + QPointF(2,2)),
            QBrush(Qt::red));
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::hoverEnterEvent()
//-----------------------------------------------------------------------------
void GraphicsConnection::hoverEnterEvent(QGraphicsSceneHoverEvent* hoverEvent)
{
    hoveredAbove_ = true;
    setDefaultColor();
    setZValue(-500); // Move current connection up.

    auto hoverPosX = hoverEvent->pos().x();
    auto hoverPosY = hoverEvent->pos().y();

    // Look for intersections at and 2x2 pixels around hover position. Helps with highlighting 
    // items on top of each other.
    auto const& intersectingItems = parent_->items(hoverPosX, hoverPosY, static_cast<qreal>(2), static_cast<qreal>(2), 
        Qt::IntersectsItemShape, Qt::DescendingOrder);

    for (auto intersectingItem : intersectingItems)
    {
        // Highlight item and set topmost if graphics connection.
        if (auto graphicsConnection = dynamic_cast<GraphicsConnection*>(intersectingItem))
        {
            graphicsConnection->setZValue(-500);
            graphicsConnection->setHovered(true);
            graphicsConnection->setDefaultColor();
        }
    }

    QGraphicsPathItem::hoverEnterEvent(hoverEvent);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::hoverLeaveEvent()
//-----------------------------------------------------------------------------
void GraphicsConnection::hoverLeaveEvent(QGraphicsSceneHoverEvent* hoverEvent)
{
    hoveredAbove_ = false;
    setDefaultColor();
    setZValue(-1000); // Move current connection back to original level.

    for (auto intersectingItem : parent_->items(shape(), Qt::IntersectsItemShape, Qt::DescendingOrder))
    {
        if (auto graphicsConnection = dynamic_cast<GraphicsConnection*>(intersectingItem))
        {
            bool connectionIsSelected = graphicsConnection->isSelected();

            // Reset z-value, if not selected.
            graphicsConnection->setZValue(connectionIsSelected ? -500 : -1000);
            graphicsConnection->setHovered(false);
            graphicsConnection->setDefaultColor();
        }
    }

    QGraphicsPathItem::hoverLeaveEvent(hoverEvent);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setItemSettings()
//-----------------------------------------------------------------------------
void GraphicsConnection::setItemSettings()
{
    setZValue(-1000);
    setLineWidth(2);
    setFlag(ItemIsSelectable);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::createRoute()
//-----------------------------------------------------------------------------
void GraphicsConnection::createRoute(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2)
{
    if (!endpoint1 || !endpoint2)
    {
        return;
    }

    QPointF startPoint = endpoint1->scenePos();
    QPointF endPoint = endpoint2->scenePos();

    if (!endpoint1->isDirectionFixed())
    {
        if (startPoint.x() <= endPoint.x())
        {
            endpoint1->setDirection(QVector2D(1.0f, 0.0f));
        }
        else
        {
            endpoint1->setDirection(QVector2D(-1.0f, 0.0f));
        }
    }

    if (!endpoint2->isDirectionFixed())
    {
        if (startPoint.x() <= endPoint.x())
        {
            endpoint2->setDirection(QVector2D(-1.0f, 0.0f));
        }
        else
        {
            endpoint2->setDirection(QVector2D(1.0f, 0.0f));
        }
    }

    pathPoints_ = DefaultRouting::createRoute(startPoint, endPoint, 
        endpoint1->getDirection(), endpoint2->getDirection());
    
    setRoute(pathPoints_);

    paintConnectionPath();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::paintConnectionPath()
//-----------------------------------------------------------------------------
void GraphicsConnection::paintConnectionPath()
{
    QListIterator<QPointF> i(pathPoints_);

    QPainterPath path(i.next());

    while (i.hasNext())
    {
        path.lineTo(i.next());
    }

    QPainterPathStroker stroker;
    setPath(stroker.createStroke(path));
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::updateName()
//-----------------------------------------------------------------------------
QString GraphicsConnection::createDefaultName() const
{
    Q_ASSERT(endpoint1_ != 0);
    Q_ASSERT(endpoint2_ != 0);

    // Determine one of the end points as the starting point in a way that its encompassing component is defined.
    ConnectionEndpoint* start = endpoint1_;
    ConnectionEndpoint* end = endpoint2_;

    if (start->encompassingComp() == nullptr && end->encompassingComp() != nullptr)
    {
        std::swap(start, end);
    }

    auto startComponentName = QString();
    if (start->encompassingComp() != nullptr)
    {
        startComponentName = start->encompassingComp()->name() + "_";
    }

    auto endComponentName = QString();
    if (end->encompassingComp() != nullptr)
    {
        endComponentName = end->encompassingComp()->name() + "_";
    }

    return startComponentName + start->name() + "_to_" + endComponentName + end->name();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::disconnectEnds()
//-----------------------------------------------------------------------------
void GraphicsConnection::disconnectEnds()
{
    // Discard existing connections.
    if (endpoint1_)
    {
        endpoint1_->removeConnection(this);
        endpoint1_->onDisconnect();
        endpoint1_->setSelectionHighlight(false);
    }

    if (endpoint2_)
    {
        endpoint2_->removeConnection(this);
        endpoint2_->onDisconnect();
        endpoint2_->setSelectionHighlight(false);
    }

    endpoint1_ = nullptr;
    endpoint2_ = nullptr;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::itemChange()
//-----------------------------------------------------------------------------
QVariant GraphicsConnection::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedHasChanged)
    {
        bool selected = value.toBool();
        setZValue(selected ? -500 : -1000); // Move up, if selected, down if unselected.
        setDefaultColor();

        if (endpoint1_ != nullptr)
        {
            endpoint1_->setSelectionHighlight(selected);
        }

        if (endpoint2_ != nullptr)
        {
            endpoint2_->setSelectionHighlight(selected);
        }
    }

    return QGraphicsPathItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::beginUpdatePosition()
//-----------------------------------------------------------------------------
void GraphicsConnection::beginUpdatePosition()
{
    oldRoute_ = route();
    positionUpdateInProcess_ = true;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::endUpdatePosition()
//-----------------------------------------------------------------------------
QUndoCommand* GraphicsConnection::endUpdatePosition(QUndoCommand* parent)
{
    positionUpdateInProcess_ = false;

    if (!parent_->isProtected())
    {
        simplifyPath();
        fixOverlap();        
        setRoute(pathPoints_);
    }

    if (route() != oldRoute_)
    {        
        QUndoCommand* undoCommand = new ConnectionMoveCommand(this, oldRoute_, parent);
        oldRoute_.clear();
        return undoCommand;
    }
    else
    {
        return nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setRoutingMode()
//-----------------------------------------------------------------------------
void GraphicsConnection::setRoutingMode(RoutingMode style)
{
    if (routingMode_ != style)
    {
        routingMode_ = style;
        
        if (style == ROUTING_MODE_NORMAL)
        {
            setVisible(true);
        }

        setDefaultColor();
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setDefaultColor()
//-----------------------------------------------------------------------------
void GraphicsConnection::setDefaultColor()
{
    QPen newPen = pen();

    if (isSelected())
    {
        newPen.setColor(KactusColors::DIAGRAM_SELECTION);
    }
    else if (hoveredAbove_)
    {
        newPen.setColor(KactusColors::MASTER_INTERFACE);
    }
    else if (invalid_)
    {
        newPen.setColor(KactusColors::BROKEN_CONNECTION);
    }
    else if (routingMode_ == ROUTING_MODE_NORMAL)
    {
        if (getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_COM)
        {
            newPen.setColor(KactusColors::COM_CONNECTION);
        }
        else
        {
            newPen.setColor(KactusColors::REGULAR_CONNECTION);
        }
    }
    else
    {
        newPen.setColor(KactusColors::UNKNOWN_CONNECTION);
    }

    setPen(newPen);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::drawOverlapGraphics()
//-----------------------------------------------------------------------------
void GraphicsConnection::drawOverlapGraphics(QPainter* painter)
{
    for (QGraphicsItem* item : scene()->items())
    {
        if (item->isVisible() && item != this)
        {
            auto connection = dynamic_cast<GraphicsConnection*>(item);
            if (connection && collidesWithItem(item))
            {
                drawOverlapWithConnection(painter, connection);
            }
            else if(dynamic_cast<ComponentItem*>(item) && collidesWithItem(item))
            {
                drawOverlapWithComponent(painter, item);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::drawOverlapWithConnection()
//-----------------------------------------------------------------------------
void GraphicsConnection::drawOverlapWithConnection(QPainter* painter, GraphicsConnection* connection)
{
    QList<QLineF> connectionLines = pointsToLines(connection->route());

    for (QLineF const& pathLine : pathLines_)
    {                
        if (!qFuzzyIsNull(pathLine.dy())) //<! Discard horizontal segments of this connection line.
        {
            for (QLineF const& connectionLine : connectionLines)
            {             
                if (!qFuzzyIsNull(connectionLine.dx())) //<! Discard vertical segments of the intersecting line.
                {
                    QPointF intersectionPoint;
                    QLineF::IntersectType type = pathLine.intersects(connectionLine, &intersectionPoint);

                    if (type == QLineF::BoundedIntersection)
                    {
                        // If the connections share an endpoint, draw a black junction circle.
                        if (endpoint1() == connection->endpoint1() || endpoint2() == connection->endpoint2() ||
                            endpoint1() == connection->endpoint2() || endpoint2() == connection->endpoint1())
                        {
                            drawJunctionPoint(painter, intersectionPoint);
                        }
                        else
                        {
                            // Otherwise draw a gray undercrossing line close to the intersection point.
                            drawUndercrossing(painter, pathLine, intersectionPoint, connection->pen().width());
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::drawJunctionPoint()
//-----------------------------------------------------------------------------
void GraphicsConnection::drawJunctionPoint(QPainter* painter, QPointF const& intersectionPoint)
{
    painter->setPen(QPen(Qt::black, 0));

    QPainterPath circlePath;
    circlePath.addEllipse(intersectionPoint, 5.0, 5.0);

    painter->fillPath(circlePath, QBrush(Qt::black));
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::drawUndercrossing()
//-----------------------------------------------------------------------------
void GraphicsConnection::drawUndercrossing(QPainter* painter, QLineF const& path, QPointF const& crossingPoint, 
    int crossConnectionWidth)
{   
    // Drawing is performed using two lines, excluding the area close to
    // the intersection point. This way the drawing is done correctly even though
    // the connection is above the other connection.
    QVector2D direction(path.dx(), path.dy());
    direction.normalize();

    qreal length1 = QVector2D(crossingPoint - path.p1()).length();
    qreal length2 = QVector2D(crossingPoint - path.p2()).length();

    qreal crossingWidth = 3;

    // If both lines are thick, we have to use a thicker width.
    if (pen().width() >= 3 && crossConnectionWidth >= 3)
    {
        crossingWidth++;
    }

    painter->setPen(QPen(KactusColors::CONNECTION_UNDERCROSSING, pen().width() + 1));

    if (length1 > 0.5f)
    {
        QPointF seg2Pt1 = (QVector2D(crossingPoint) - direction * qMin(length1, crossingWidth)).toPointF();
        QPointF seg2Pt2 = (QVector2D(crossingPoint) - direction * qMin(length1, (qreal)GridSize/2)).toPointF();
        painter->drawLine(seg2Pt1, seg2Pt2);
    }

    if (length2 > 0.5f)
    {
        QPointF seg1Pt1 = (QVector2D(crossingPoint) + direction * qMin(length2, crossingWidth)).toPointF();
        QPointF seg1Pt2 = (QVector2D(crossingPoint) + direction * qMin(length2, (qreal)GridSize/2)).toPointF();
        painter->drawLine(seg1Pt1, seg1Pt2);
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::drawOverlapWithComponent()
//-----------------------------------------------------------------------------
void GraphicsConnection::drawOverlapWithComponent(QPainter* painter, QGraphicsItem* item)
{
    auto comp = static_cast<ComponentItem*>(item);
    QRectF componentRect = comp->rect();

    // Create the line objects for each edge of the diagram component rectangle.
    QLineF leftEdge(comp->mapToScene(componentRect.topLeft()), comp->mapToScene(componentRect.bottomLeft()));
    QLineF rightEdge(comp->mapToScene(componentRect.topRight()), comp->mapToScene(componentRect.bottomRight()));
    QLineF topEdge(comp->mapToScene(componentRect.topLeft()), comp->mapToScene(componentRect.topRight()));
    QLineF bottomEdge(comp->mapToScene(componentRect.bottomLeft()),
        comp->mapToScene(componentRect.bottomRight()));

    for (QLineF const& pathLine : pathLines_)
    {                
        // Check for intersections with the component rectangle's edges.
        QPointF leftPoint;
        QPointF rightPoint;
        QPointF topPoint;
        QPointF bottomPoint;

        QLineF::IntersectType leftIntersection = pathLine.intersects(leftEdge, &leftPoint);
        QLineF::IntersectType rightIntersection = pathLine.intersects(rightEdge, &rightPoint);
        QLineF::IntersectType topIntersection = pathLine.intersects(topEdge, &topPoint);
        QLineF::IntersectType bottomIntersection = pathLine.intersects(bottomEdge, &bottomPoint);

        painter->setPen(QPen(KactusColors::CONNECTION_UNDERCROSSING, pen().width() + 1));

        if (leftIntersection == QLineF::BoundedIntersection && leftPoint != pathPoints_.first() &&
            leftPoint != pathPoints_.last())
        {
            drawLineGap(painter, pathLine, leftPoint);
        }

        if (rightIntersection == QLineF::BoundedIntersection && rightPoint != pathPoints_.first() &&
            rightPoint != pathPoints_.last())
        {
            drawLineGap(painter, pathLine, rightPoint);

            // Fill in the whole line segment under the component if the segment goes across the component
            // horizontally.
            if (leftIntersection == QLineF::BoundedIntersection)
            {
                painter->drawLine(leftPoint, rightPoint);
            }
        }

        if (topIntersection == QLineF::BoundedIntersection)
        {
            drawLineGap(painter, pathLine, topPoint);
        }

        if (bottomIntersection == QLineF::BoundedIntersection)
        {
            drawLineGap(painter, pathLine, bottomPoint);

            // Fill in the whole line segment under the component if the segment goes across the component
            // vertically.
            if (topIntersection == QLineF::BoundedIntersection)
            {
                painter->drawLine(topPoint, bottomPoint);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::drawLineGap()
//-----------------------------------------------------------------------------
void GraphicsConnection::drawLineGap(QPainter* painter, QLineF const& line1, QPointF const& pt)
{
    QVector2D dir(line1.dx(), line1.dy());
    dir.normalize();

    qreal length1 = QVector2D(pt - line1.p1()).length();
    qreal length2 = QVector2D(pt - line1.p2()).length();

    QPointF pt1 = (QVector2D(pt) + dir * qMin(length2, (qreal)GridSize / 2)).toPointF();
    QPointF pt2 = (QVector2D(pt) - dir * qMin(length1, (qreal)GridSize) / 2).toPointF();
    painter->drawLine(pt1, pt2);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setLineWidth()
//-----------------------------------------------------------------------------
void GraphicsConnection::setLineWidth(int width)
{
    QPen newPen = pen();
    newPen.setWidth(width);
    setPen(newPen);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::getConnectionType()
//-----------------------------------------------------------------------------
ConnectionEndpoint::EndpointType GraphicsConnection::getConnectionType() const
{
    if (endpoint1() == nullptr && endpoint2() == nullptr)
    {
        return ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED;
    }

    if (endpoint1()->getType() != ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED)
    {
        return endpoint1()->getType();
    }
    else
    {
        return endpoint2()->getType();
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::getRoutingMode()
//-----------------------------------------------------------------------------
GraphicsConnection::RoutingMode GraphicsConnection::getRoutingMode() const
{
    return routingMode_;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::getRouteExtension()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> GraphicsConnection::getRouteExtension() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setImported()
//-----------------------------------------------------------------------------
void GraphicsConnection::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::isImported()
//-----------------------------------------------------------------------------
bool GraphicsConnection::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::validate()
//-----------------------------------------------------------------------------
void GraphicsConnection::validate()
{
    invalid_ = !endpoint1_->isConnectionValid(endpoint2_) || !endpoint2_->isConnectionValid(endpoint1_);
    setDefaultColor();
    endpoint1_->updateInterface();
    endpoint2_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setEndpoint1()
//-----------------------------------------------------------------------------
void GraphicsConnection::setEndpoint1(ConnectionEndpoint* endpoint1)
{
    if (endpoint1_ != nullptr)
    {
        // Disconnect from the previous endpoint.
        endpoint1_->removeConnection(this);
        endpoint1_->onDisconnect();
    }

    // Connect to the new endpoint.
    endpoint1_ = endpoint1;
    endpoint1_->onConnect(endpoint2_);

    validate();

    endpoint1->addConnection(this);

    updatePosition();
    setName(createDefaultName());
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setEndpoint2()
//-----------------------------------------------------------------------------
void GraphicsConnection::setEndpoint2(ConnectionEndpoint* endpoint2)
{
    if (endpoint2_ != nullptr)
    {
        // Disconnect from the previous endpoint.
        endpoint2_->removeConnection(this);
        endpoint2_->onDisconnect();
    }

    // Connect to the new endpoint.
    endpoint2_ = endpoint2;
    endpoint2_->onConnect(endpoint1_);

    validate();

    endpoint2->addConnection(this);

    updatePosition();
    setName(createDefaultName());
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::isInvalid()
//-----------------------------------------------------------------------------
bool GraphicsConnection::isInvalid() const
{
    return invalid_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setHovered()
//-----------------------------------------------------------------------------
void GraphicsConnection::setHovered(bool hovered)
{
    hoveredAbove_ = hovered;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::getHovered()
//-----------------------------------------------------------------------------
bool GraphicsConnection::getHovered() const
{
    return hoveredAbove_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::hasDefaultName()
//-----------------------------------------------------------------------------
bool GraphicsConnection::hasDefaultName() const
{
    return (createDefaultName() == route_->name());
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::toggleOffPage()
//-----------------------------------------------------------------------------
void GraphicsConnection::toggleOffPage()
{
    route_->setOffpage(!route_->isOffpage());

    // Determine the new end points for the connection.
    ConnectionEndpoint* endpoint1 = endpoint1_;
    ConnectionEndpoint* endpoint2 = endpoint2_;
    
    if (endpoint1->type() == OffPageConnectorItem::Type)
    {
        endpoint1 = static_cast<ConnectionEndpoint*>(endpoint1->parentItem());
        endpoint2 = static_cast<ConnectionEndpoint*>(endpoint2->parentItem());
    }
    else
    {
        endpoint1 = endpoint1->getOffPageConnector();
        endpoint2 = endpoint2->getOffPageConnector();
    }

    // Disconnect old endpoints.
    endpoint1_->removeConnection(this);
    endpoint2_->removeConnection(this);

    // Connect new endpoints.
    endpoint1_ = endpoint1;
    endpoint2_ = endpoint2;

    endpoint1_->addConnection(this);
    endpoint2_->addConnection(this);

    updatePosition();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::isOffPage()
//-----------------------------------------------------------------------------
bool GraphicsConnection::isOffPage() const
{
    return route_->isOffpage();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::fixOverlap()
//-----------------------------------------------------------------------------
void GraphicsConnection::fixOverlap()
{
    // Overlap fix is possible only for normally routed connections.
    if (routingMode_ != ROUTING_MODE_NORMAL)
    {
        return;
    }

    // Build segment bounding lists based on the other existing connections.
    QList<SegmentBound> horizontalBounds;
    QList<SegmentBound> verticalBounds;
    createSegmentBounds(verticalBounds, horizontalBounds);
    
    // Iteratively move segments until the solution does not change.
    bool solutionChanged = true;

    while (solutionChanged)
    {
        solutionChanged = false;

        for (int i = 1; i < pathPoints_.size() - 2; ++i)
        {
            // Check if the segment is vertical.
            if (qFuzzyCompare(pathPoints_.at(i).x(), pathPoints_.at(i + 1).x()))
            {
                if (fixVerticalSegmentClearance(verticalBounds, i))
                {
                    solutionChanged = true;
                }
            }
            // Otherwise check if the segment is horizontal.
            else if (qFuzzyCompare(pathPoints_.at(i).y(), pathPoints_.at(i + 1).y()))
            {
                if (fixHorizontalSegmentClearance(horizontalBounds, i))
                {
                    solutionChanged = true;
                }
            }
        }
    }

}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::createSegmentBounds()
//-----------------------------------------------------------------------------
void GraphicsConnection::createSegmentBounds(QList<SegmentBound>& verticalBounds,
                                             QList<SegmentBound>& horizontalBounds)
{
    for (QGraphicsItem* item : scene()->items())
    {
        auto const conn = dynamic_cast<GraphicsConnection const*>(item);

        // If the connections share an endpoint, discard it from the segment bounds.
        if (conn != 0 && conn != this &&
            endpoint1() != conn->endpoint1() && endpoint2() != conn->endpoint2() &&
            endpoint1() != conn->endpoint2() && endpoint2() != conn->endpoint1())
        {
            for (int i = 0; i < conn->route().size() - 1; ++i)
            {
                if (qFuzzyCompare(conn->route().at(i).x(), conn->route().at(i + 1).x()))
                {
                    verticalBounds.append(SegmentBound(conn->route().at(i), conn->route().at(i + 1)));
                }
                else
                {
                    horizontalBounds.append(SegmentBound(conn->route().at(i), conn->route().at(i + 1)));
                }
            }
        }
    }

    // Add start and end segments of this connection also to the list of horizontal bounds.
    if (pathPoints_.size() >= 2)
    {
        horizontalBounds.append(SegmentBound(pathPoints_.first(), pathPoints_.at(1)));
        horizontalBounds.append(SegmentBound(pathPoints_.at(pathPoints_.size() - 2), pathPoints_.last()));
    }

    std::sort(horizontalBounds.begin(), horizontalBounds.end(), sortBoundsByY);
    std::sort(verticalBounds.begin(), verticalBounds.end(), sortBoundsByX);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::sortBoundsByX()
//-----------------------------------------------------------------------------
bool GraphicsConnection::sortBoundsByX(SegmentBound const& lhs, SegmentBound const& rhs)
{
    return (lhs.minX < rhs.minX || (lhs.minX == rhs.minX && lhs.minY < rhs.minY));
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::sortBoundsByY()
//-----------------------------------------------------------------------------
bool GraphicsConnection::sortBoundsByY(SegmentBound const& lhs, SegmentBound const& rhs)
{
    return (lhs.minY < rhs.minY || (lhs.minY == rhs.minY && lhs.minX < rhs.minX));
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::simplifyPath()
//-----------------------------------------------------------------------------
void GraphicsConnection::simplifyPath()
{
    if (pathPoints_.size() < 3)
    {
        return;
    }

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
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::getSegmentLimitsX()
//-----------------------------------------------------------------------------
QPair<qreal, qreal> GraphicsConnection::getSegmentLimitsX(int i) const
{
    // Prevent connections going out of the left edge.
    qreal minX = 10.0;
    qreal maxX = 100000.0f;

    // Clamp the min and max.
    qreal prev = pathPoints_.at(i - 1).x();
    qreal cur = pathPoints_.at(i).x();
    qreal next = pathPoints_.at(i + 2).x();

    if (cur > next)
    {
        minX = qMax(minX, next + DefaultRouting::MIN_START_LENGTH);
    }
    else
    {
        maxX = qMin(maxX, next - DefaultRouting::MIN_START_LENGTH);
    }

    if (cur > prev)
    {
        minX = qMax(minX, DefaultRouting::MIN_START_LENGTH + prev);
    }
    else
    {
        maxX = qMin(maxX, prev - DefaultRouting::MIN_START_LENGTH);
    }

    return qMakePair(minX, maxX);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::getSegmentLimitsY()
//-----------------------------------------------------------------------------
QPair<qreal, qreal> GraphicsConnection::getSegmentLimitsY(int i) const
{
    // Use simple distance based min and max.
    qreal minY = qMax(30.0, pathPoints_.at(i).y() - 500.0);
    qreal maxY = pathPoints_.at(i).y() + 500.0;

    return qMakePair(minY, maxY);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::findVerticalSegmentOverlap()
//-----------------------------------------------------------------------------
int GraphicsConnection::findVerticalSegmentOverlap(QList<SegmentBound> const& verBounds, SegmentBound const& bounds)
{
    // Check all bounds that have the same X.
    for (int i = 0; i < verBounds.size(); i++)
    {
        if (qFuzzyCompare(verBounds.at(i).minX, bounds.minX) &&
            qFuzzyCompare(verBounds.at(i).maxX, bounds.minX))
        {
            if (testSegmentOverlapY(verBounds.at(i), bounds))
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::findHorizontalSegmentOverlap()
//-----------------------------------------------------------------------------
int GraphicsConnection::findHorizontalSegmentOverlap(QList<SegmentBound> const& horBounds, SegmentBound const& bounds)
{
    // Check all bounds that have the same Y.
    for (int i = 0; i < horBounds.size(); i++)
    {
        if (qFuzzyCompare(horBounds.at(i).minY, bounds.minY) &&
            qFuzzyCompare(horBounds.at(i).maxY, bounds.minY))
        {
            if (testSegmentOverlapX(horBounds.at(i), bounds))
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::fixVerticalSegmentClearance()
//-----------------------------------------------------------------------------
bool GraphicsConnection::fixVerticalSegmentClearance(QList<SegmentBound> const& verticalBounds, int i)
{
    // Create segment bounds for the inspected segment.
    SegmentBound bounds(pathPoints_.at(i), pathPoints_.at(i + 1));

    // Check if the segment does not violate the clearance rule.
    if (findVerticalSegmentOverlap(verticalBounds, bounds) == -1)
    {
        return false;
    }
    
    // Retrieve segment limits.
    QPair<qreal, qreal> limits = getSegmentLimitsX(i);
    qreal minX = limits.first;
    qreal maxX = limits.second;

    qreal currentX = bounds.minX;
    qreal newX = bounds.minX;

    for (int move = GridSize; minX <= currentX - move || currentX + move <= maxX; move += GridSize)
    {
        SegmentBound leftCandidate(QPointF(currentX - move, pathPoints_.at(i).y()),
            QPointF(currentX - move, pathPoints_.at(i + 1).y()));
        SegmentBound rightCandidate(QPointF(currentX + move, pathPoints_.at(i).y()),
            QPointF(currentX + move, pathPoints_.at(i + 1).y()));

        if (minX <= currentX - move && findVerticalSegmentOverlap(verticalBounds, leftCandidate) == -1)
        {
            newX = currentX - move;
            break;
        }
        else if (currentX + move <= maxX && findVerticalSegmentOverlap(verticalBounds, rightCandidate) == -1)
        {
            newX = currentX + move;
            break;
        }
    }

    // Check if we cannot change the bounds at all.
    if (newX < minX || newX > maxX || qFuzzyCompare(newX, bounds.minX))
    {
        return false;
    }

    pathPoints_[i].setX(newX);
    pathPoints_[i + 1].setX(newX);

    return true;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::fixHorizontalSegmentClearance()
//-----------------------------------------------------------------------------
bool GraphicsConnection::fixHorizontalSegmentClearance(QList<SegmentBound> const& horBounds, int i)
{
    // Create segment bounds for the inspected segment.
    SegmentBound bounds(pathPoints_.at(i), pathPoints_.at(i + 1));

    // Check if the segment does not violate the clearance rule.
    if (findHorizontalSegmentOverlap(horBounds, bounds) == -1)
    {
        return false;
    }

    // Retrieve segment limits.
    QPair<qreal, qreal> limits = getSegmentLimitsY(i);
    qreal minY = limits.first;
    qreal maxY = limits.second;
    
    qreal newY = bounds.minY;
    qreal currentY = bounds.minY;

    for (int move = GridSize; minY <= currentY - move || currentY + move <= maxY; move += GridSize)
    {
        SegmentBound upCandidate(QPointF(pathPoints_.at(i).x(), currentY - move),
            QPointF(pathPoints_.at(i + 1).x(), currentY - move));
        SegmentBound downCandidate(QPointF(pathPoints_.at(i).x(), currentY + move),
            QPointF(pathPoints_.at(i + 1).x(), currentY + move));

        if (minY <= currentY - move && findHorizontalSegmentOverlap(horBounds, upCandidate) == -1)
        {
            newY = currentY - move;
            break;
        }
        else if (currentY + move <= maxY && findHorizontalSegmentOverlap(horBounds, downCandidate) == -1)
        {
            newY = currentY + move;
            break;
        }
    }

    // Check if we cannot change the bounds at all.
    if (newY < minY || newY > maxY || qFuzzyCompare(newY, bounds.minY))
    {
        return false;
    }

    pathPoints_[i].setY(newY);
    pathPoints_[i + 1].setY(newY);
    return true;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::testSegmentOverlapX()
//-----------------------------------------------------------------------------
bool GraphicsConnection::testSegmentOverlapX(SegmentBound const& bound1, SegmentBound const& bound2) const
{
    return !(bound1.maxX < bound2.minX || bound1.minX > bound2.maxX);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::testSegmentOverlapY()
//-----------------------------------------------------------------------------
bool GraphicsConnection::testSegmentOverlapY(SegmentBound const& bound1, SegmentBound const& bound2) const
{
    return !(bound1.maxY < bound2.minY || bound1.minY > bound2.maxY);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::updateEndpointDirection()
//-----------------------------------------------------------------------------
void GraphicsConnection::updateEndpointDirection(ConnectionEndpoint* endpoint, QVector2D dir)
{
    if (endpoint)
    {
        // Switch the direction of the end point if it is not correct.
        if (!endpoint->isDirectionFixed() && QVector2D::dotProduct(dir, endpoint->getDirection()) < 0)
        {
            endpoint->setDirection(dir);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::connectionPoint()
//-----------------------------------------------------------------------------
QPointF GraphicsConnection::connectionPoint(QPointF const& otherEnd) const
{
    QList<QPointF> connectionPoints;
    
    if (otherEnd.isNull() || getRoutingMode() == ROUTING_MODE_OFFPAGE)
    {        
        connectionPoints.append(pathPoints_.first());
        connectionPoints.append(pathPoints_.last());           
    }
    else
    {
        int segmentCount = pathPoints_.size() - 1;
        for(int i = 0; i < segmentCount; i++)
        {
            QPointF segmentStart = pathPoints_.at(i);
            QPointF segmentEnd = pathPoints_.at(i + 1);     
            QPointF segmentCenter = segmentStart + (segmentEnd - segmentStart)/2;

            connectionPoints.append(segmentCenter);
        }
    }

    return findClosestPoint(connectionPoints, otherEnd);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::findClosestPoint()
//-----------------------------------------------------------------------------
QPointF GraphicsConnection::findClosestPoint(QList<QPointF> const& sourcePoints, QPointF const& destination) const
{
    QPointF closest = sourcePoints.first();
    qreal shortestDistance = QLineF(closest, destination).length();

    for (QPointF const& candidate : sourcePoints)
    {
        qreal distance = QLineF(candidate, destination).length();
        if (distance < shortestDistance)
        {
            shortestDistance = distance;
            closest = candidate;
        }
    }

    return closest;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::changeConnectionComponentReference()
//-----------------------------------------------------------------------------
void GraphicsConnection::changeConnectionComponentReference(QString const&, QString const& )
{
    return;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::pointsToLines()
//-----------------------------------------------------------------------------
QList<QLineF> GraphicsConnection::pointsToLines(QList<QPointF> const& points) const
{
    QList<QLineF> lines;
    for (int i = 0; i < points.size() - 1; i++)
    {
        lines.append(QLineF(points.at(i), points.at(i + 1)));
    }

    return lines;
}
