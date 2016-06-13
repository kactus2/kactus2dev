//-----------------------------------------------------------------------------
// File: HWConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT interconnection.
//-----------------------------------------------------------------------------

#include "HWConnection.h"
#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "HWDesignWidget.h"

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>

#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>

//-----------------------------------------------------------------------------
// Function: HWConnection::HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2, bool autoConnect,
    QString const& name, QString const& displayName, QString const& description, DesignDiagram* parent):
GraphicsConnection(endpoint1, endpoint2, autoConnect, name, displayName, description, parent), 
    widthLabel_(new QGraphicsTextItem(this)), 
    interconnection_(),
    route_()
{
    setWidthLabelDefaultFont();

    if (autoConnect)
    {
        updateWidthLabel();
    }
}

//-----------------------------------------------------------------------------
// Function: HWConnection::HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2, 
    QSharedPointer<Interconnection> interconnection, QSharedPointer<ConnectionRoute> route, DesignDiagram* parent):
GraphicsConnection(endpoint1, endpoint2, false, interconnection->name(), interconnection->displayName(),
    interconnection->description(), parent),
    widthLabel_(new QGraphicsTextItem(this)),
    interconnection_(interconnection),
    route_(route)
{
    GraphicsConnection::setRoute(route_->getRoute());

    setWidthLabelDefaultFont();
    updateWidthLabel();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
    QString const& displayName, QString const& description, DesignDiagram* parent):
GraphicsConnection(p1, dir1, p2, dir2, displayName, description, parent),
    widthLabel_(new QGraphicsTextItem(this))
{
    setWidthLabelDefaultFont();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::~HWConnection()
//-----------------------------------------------------------------------------
HWConnection::~HWConnection()
{
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setName()
//-----------------------------------------------------------------------------
void HWConnection::setName(QString const& name)
{
    GraphicsConnection::setName(name);

    interconnection_->setName(name);
    route_->setName(name);
    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::name()
//-----------------------------------------------------------------------------
QString HWConnection::name() const
{
    return interconnection_->name();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setDescription()
//-----------------------------------------------------------------------------
void HWConnection::setDescription(QString const& description)
{
    interconnection_->setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::description()
//-----------------------------------------------------------------------------
QString HWConnection::description() const
{
    return interconnection_->description();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::getInterconnection()
//-----------------------------------------------------------------------------
QSharedPointer<Interconnection> HWConnection::getInterconnection()
{
    return interconnection_;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::getRouteExtension()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> HWConnection::getRouteExtension() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::toggleOffPage()
//-----------------------------------------------------------------------------
void HWConnection::toggleOffPage()
{
    route_->setOffpage(!route_->isOffpage());
    GraphicsConnection::toggleOffPage();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setRoute()
//-----------------------------------------------------------------------------
void HWConnection::setRoute(QList<QPointF> path)
{
    route_->setRoute(path);

    GraphicsConnection::setRoute(path);
    updateWidthLabel();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::updatePosition()
//-----------------------------------------------------------------------------
void HWConnection::updatePosition()
{
    GraphicsConnection::updatePosition();
    updateWidthLabel();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::updateWidthLabel()
//-----------------------------------------------------------------------------
void HWConnection::updateWidthLabel()
{
    if (route().size() < 2)
    {
        return;
    }

    int totalWidth = 0;

    // for bus connections, the bus width must be calculated from the port maps.
    if (isBus())
    {
        totalWidth = calculateBusWidth();
    }
    else
    {
        // Otherwise the connection is for only one signal (i.e. ad-hoc).
        totalWidth = 1;
    }

    widthLabel_->setPlainText(QString::number(totalWidth));

    QPointF textPos = findWidthLabelPosition();
    widthLabel_->setPos(textPos);
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setBusWidthVisible()
//-----------------------------------------------------------------------------
void HWConnection::setBusWidthVisible(bool visible)
{
    widthLabel_->setVisible(visible);
}

//-----------------------------------------------------------------------------
// Function: HWConnection::calculateBusWidth()
//-----------------------------------------------------------------------------
int HWConnection::calculateBusWidth() const
{
    int totalWidth = 0;

    /*foreach (QSharedPointer<PortMap> portMap1, *endpoint1()->getBusInterface()->getPortMaps())
    {
        // Find the port map with the same logical port name from the other end point's port map.
        QSharedPointer<PortMap> portMap2;
        foreach (QSharedPointer<PortMap> portMap, *endpoint2()->getBusInterface()->getPortMaps())
        {
            if (portMap->logicalPort() == portMap1->logicalPort())
            {
                portMap2 = portMap;
            }
        }

        if (portMap2)
        {
            QSharedPointer<Port> port1 = endpoint1()->getOwnerComponent()->getPort(portMap1->getPhysicalPort()->name_);
            QSharedPointer<Port> port2 = endpoint2()->getOwnerComponent()->getPort(portMap2->getPhysicalPort()->name_);

            if (port1 && port2)
            {
                // TODO: Calculate the intersection of the port bounds and add it to the total width.
                /*General::PortAlignment align =
                General::calculatePortAlignment(portMap1.data(), port1->getLeftBound(), port1->getRightBound(),
                portMap2.data(), port2->getLeftBound(), port2->getRightBound());

                int width = qMin(align.port1Left_, align.port2Left_) - qMax(align.port1Right_, align.port2Right_) + 1;
                totalWidth += width;
            }
        }
    }*/

    return totalWidth;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::isBus()
//-----------------------------------------------------------------------------
bool HWConnection::isBus() const
{
    return endpoint1()->isBus();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::findWidthLabelPosition()
//-----------------------------------------------------------------------------
QPointF HWConnection::findWidthLabelPosition() const
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
// Function: HWConnection::findWidthLabelHorizontally()
//-----------------------------------------------------------------------------
QPointF HWConnection::findWidthLabelHorizontally() const
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
// Function: HWConnection::findWidthLabelVertically()
//-----------------------------------------------------------------------------
QPointF HWConnection::findWidthLabelVertically() const
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
    if (pathPoints.size() == 2)
    {
        position.setX(pt1.x() + GridSize);
    }
    // Place the text on the right side if the connection is fully on the left.
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
// Function: HWConnection::setWidthLabelFont()
//-----------------------------------------------------------------------------
void HWConnection::setWidthLabelDefaultFont()
{
    QFont font = widthLabel_->font();
    font.setPointSize(8);
    font.setBold(true);
    widthLabel_->setFont(font);
}

//-----------------------------------------------------------------------------
// Function: HWConnection::changeConnectionComponentReference()
//-----------------------------------------------------------------------------
void HWConnection::changeConnectionComponentReference(QString const& oldName, QString const& newName)
{
    interconnection_->changetInterfaceComponentReferences(oldName, newName);
}
