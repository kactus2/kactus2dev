//-----------------------------------------------------------------------------
// File: AdHocConnectionItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.10.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "AdHocConnectionItem.h"

#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "HWDesignWidget.h"

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>

#include <IPXACTmodels/Design/AdHocConnection.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::AdHocConnectionItem()
//-----------------------------------------------------------------------------
AdHocConnectionItem::AdHocConnectionItem(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2, 
    QSharedPointer<AdHocConnection> adhocConnection, QSharedPointer<ConnectionRoute> route,
    DesignDiagram* parent)
    : GraphicsConnection(endpoint1, endpoint2, true,
    adhocConnection->name(),
    adhocConnection->displayName(),
    adhocConnection->description(), parent),
    widthLabel_(new QGraphicsTextItem(this)),
    portBounds_(),
    adHocConnection_(adhocConnection),
    route_(route)
{
    GraphicsConnection::setRoute(route_->getRoute());

    setWidthLabelDefaultFont();
    setLineWidth(1);
    updateWidthLabel();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::AdHocConnectionItem()
//-----------------------------------------------------------------------------
AdHocConnectionItem::AdHocConnectionItem(QPointF const& p1, QVector2D const& dir1,
    QPointF const& p2, QVector2D const& dir2,
    QString const& displayName,
    QString const& description,
    DesignDiagram* parent):
GraphicsConnection(p1, dir1, p2, dir2, displayName, description, parent),
    widthLabel_(new QGraphicsTextItem(this)),
    portBounds_(),
    adHocConnection_(),
    route_()
{
    setWidthLabelDefaultFont();
    setLineWidth(1);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::~AdHocConnectionItem()
//-----------------------------------------------------------------------------
AdHocConnectionItem::~AdHocConnectionItem()
{
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setName()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setName(QString const& name)
{
    GraphicsConnection::setName(name);

    adHocConnection_->setName(name);
    route_->setName(name);
    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::name()
//-----------------------------------------------------------------------------
QString AdHocConnectionItem::name() const
{
    return adHocConnection_->name();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setDescription()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setDescription(QString const& description)
{
    adHocConnection_->setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::description()
//-----------------------------------------------------------------------------
QString AdHocConnectionItem::description() const
{
    return adHocConnection_->description();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::getAdHocConnection()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdHocConnectionItem::getAdHocConnection()
{
    return adHocConnection_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::getRouteExtension()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> AdHocConnectionItem::getRouteExtension() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::connectEnds()
//-----------------------------------------------------------------------------
bool AdHocConnectionItem::connectEnds()
{
    if (!GraphicsConnection::connectEnds())
    {
        return false;
    }

    if (getAdHocLeftBound(0).isEmpty())
    {
        setAdHocLeftBound(0, endpoint1()->getPort()->getLeftBound());
    }

    if (getAdHocRightBound(0).isEmpty())
    {
        setAdHocRightBound(0, endpoint1()->getPort()->getRightBound());
    }

    if (getAdHocLeftBound(1).isEmpty())
    {
        setAdHocLeftBound(1, endpoint2()->getPort()->getLeftBound());
    }

    if (getAdHocRightBound(1).isEmpty())
    {
        setAdHocRightBound(1, endpoint2()->getPort()->getRightBound());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::toggleOffPage()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::toggleOffPage()
{
    route_->setOffpage(!route_->isOffpage());
    GraphicsConnection::toggleOffPage();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setRoute()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setRoute(QList<QPointF> path)
{
    route_->setRoute(path);

    GraphicsConnection::setRoute(path);
    updateWidthLabel();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::updatePosition()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::updatePosition()
{
    GraphicsConnection::updatePosition();
    updateWidthLabel();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::updateWidthLabel()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::updateWidthLabel()
{
    if (route().size() < 2)
    {
        return;
    }

    widthLabel_->setPlainText("1");

    QPointF textPos = findWidthLabelPosition();
    widthLabel_->setPos(textPos);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setBusWidthVisible()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setBusWidthVisible(bool visible)
{
    widthLabel_->setVisible(visible);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::isBus()
//-----------------------------------------------------------------------------
bool AdHocConnectionItem::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::getAdHocLeftBound()
//-----------------------------------------------------------------------------
QString AdHocConnectionItem::getAdHocLeftBound(int endpointIndex) const
{
    return portBounds_[endpointIndex].left_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::getAdHocRightBound()
//-----------------------------------------------------------------------------
QString AdHocConnectionItem::getAdHocRightBound(int endpointIndex) const
{
    return portBounds_[endpointIndex].right_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setAdHocLeftBound()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setAdHocLeftBound(int endpointIndex, QString const& leftBound)
{
    portBounds_[endpointIndex].left_ = leftBound;
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setAdHocRightBound()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setAdHocRightBound(int endpointIndex, QString const& rightBound)
{
    portBounds_[endpointIndex].right_ = rightBound;
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::findWidthLabelPosition()
//-----------------------------------------------------------------------------
QPointF AdHocConnectionItem::findWidthLabelPosition() const
{
    QPointF textPos = boundingRect().center();
    if (getRoutingMode() == ROUTING_MODE_OFFPAGE)
    {
        textPos.setX(textPos.x() - widthLabel_->boundingRect().width() * 0.5);
        textPos.setY(textPos.y() - 2 * GridSize);
    }
    else
    {
        // If the end points are more apart in horizontal direction, place the text above
        // the longest horizontal segment.
        if (qAbs(endpoint1()->scenePos().x() - endpoint2()->scenePos().x()) >=
            qAbs(endpoint1()->scenePos().y() - endpoint2()->scenePos().y()))
        {
            textPos = findWidthLabelHorizontally();
        }
        // Otherwise place the text beside the longest vertical segment.
        else
        {
            textPos = findWidthLabelVertically();
        }
    }
    return textPos;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::findWidthLabelHorizontally()
//-----------------------------------------------------------------------------
QPointF AdHocConnectionItem::findWidthLabelHorizontally() const
{
    QList<QPointF> pathPoints = route();

    // Determine the longest horizontal segment.
    int longestIndex = 0;
    qreal longestLength = 0.0;

    for (int i = 0; i < route().size() - 1; i++)
    {
        QPointF const& pt1 = pathPoints[i];
        QPointF const& pt2 = pathPoints[i + 1];
        qreal length = QVector2D(pt2 - pt1).length();

        if (qFuzzyCompare(pt1.y(), pt2.y()) && length > longestLength)
        {
            longestLength = length;
            longestIndex = i;
        }
    }

    QPointF const& pt1 = pathPoints[longestIndex];
    QPointF const& pt2 = pathPoints[longestIndex + 1];

    QPointF position;
    position.setX((pt1.x() + pt2.x()) * 0.5 - widthLabel_->boundingRect().width() * 0.5);
    position.setY(pt1.y() - 2 * GridSize);
    return position;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::findWidthLabelVertically()
//-----------------------------------------------------------------------------
QPointF AdHocConnectionItem::findWidthLabelVertically() const
{
    QList<QPointF> pathPoints = route();

    // Determine the longest vertical segment.
    int longestIndex = 0;
    qreal longestLength = 0.0;

    for (int i = 0; i < pathPoints.size() - 1; ++i)
    {
        QPointF const& pt1 = pathPoints[i];
        QPointF const& pt2 = pathPoints[i + 1];
        qreal length = QVector2D(pt2 - pt1).length();

        if (qFuzzyCompare(pt1.x(), pt2.x()) && length > longestLength)
        {
            longestLength = length;
            longestIndex = i;
        }
    }

    Q_ASSERT(longestIndex > 0 && longestIndex + 2 < pathPoints.size());
    QPointF const& pt1 = pathPoints[longestIndex];
    QPointF const& pt2 = pathPoints[longestIndex + 1];

    QPointF position;
    // Place the text on the right side if the connection is fully on the left.
    if (pathPoints.size() == 2)
    {
        position.setX(pt1.x() + GridSize);
    }
    else if (pathPoints[longestIndex - 1].x() < pt1.x() && pathPoints[longestIndex + 2].x() < pt1.x())
    {
        position.setX(pt1.x() + GridSize);
    }
    // Place the text on the left side if the connection is fully on the right.
    else if (pathPoints[longestIndex - 1].x() > pt1.x() && pathPoints[longestIndex + 2].x() > pt1.x())
    {
        position.setX(pt1.x() - widthLabel_->boundingRect().width());
    }
    else
    {
        // Otherwise we have to determine on which side there is more space.
        if (QVector2D(pathPoints[longestIndex - 1] - pt1).lengthSquared() <
            QVector2D(pathPoints[longestIndex + 2] - pt2).lengthSquared())
        {
            position.setX(pt1.x() + GridSize);
        }
        else
        {
            position.setX(pt1.x() - widthLabel_->boundingRect().width());
        }
    }

    position.setY((pt1.y() + pt2.y()) * 0.5 - widthLabel_->boundingRect().height() * 0.5);
    return position;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setWidthLabelFont()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setWidthLabelDefaultFont()
{
    QFont font = widthLabel_->font();
    font.setPointSize(8);
    font.setBold(true);
    widthLabel_->setFont(font);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::changeConnectionComponentReference()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::changeConnectionComponentReference(QString const& oldName, QString const& newName)
{
    foreach (QSharedPointer<PortReference> portReference, *adHocConnection_->getInternalPortReferences())
    {
        if (portReference->getComponentRef() == oldName)
        {
            portReference->setComponentRef(newName);
        }
    }
}
