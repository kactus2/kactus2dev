//-----------------------------------------------------------------------------
// File: BusInterfaceEndPoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.02.2018
//
// Description:
// Parent interface class for HW bus interfaces.
//-----------------------------------------------------------------------------

#include "BusInterfaceEndPoint.h"

#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>

#include <designEditors/HWDesign/OffPageConnectorItem.h>
#include <designEditors/HWDesign/InterfaceGraphics.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::BusInterfaceEndPoint()
//-----------------------------------------------------------------------------
BusInterfaceEndPoint::BusInterfaceEndPoint(QSharedPointer<BusInterface> busIf, QGraphicsItem *parent,
    QVector2D const& dir):
HWConnectionEndpoint(parent, dir),
nameLabel_(new QGraphicsTextItem("", this)),
busInterface_(busIf),
oldPos_(),
offPageConnector_()
{
    setType(ENDPOINT_TYPE_BUS);

    QFont font = nameLabel_->font();
    font.setPointSize(8);
    nameLabel_->setFont(font);
    nameLabel_->setFlag(ItemStacksBehindParent);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
    nameLabel_->setGraphicsEffect(shadow);

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    offPageConnector_ = new OffPageConnectorItem(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::~BusInterfaceEndPoint()
//-----------------------------------------------------------------------------
BusInterfaceEndPoint::~BusInterfaceEndPoint()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> BusInterfaceEndPoint::getBusInterface() const
{
    return busInterface_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getNameLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem* BusInterfaceEndPoint::getNameLabel() const
{
    return nameLabel_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::updateInterface()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::updateInterface()
{
    HWConnectionEndpoint::updateInterface(); 

    DirectionTypes::Direction direction =
        InterfaceGraphics::getInterfaceDirection(busInterface_, getOwnerComponent());

    QPolygonF shape = getInterfaceShape(direction);
    setPolygon(shape);

    nameLabel_->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + name() + "</div>");

    setLabelPosition();

    offPageConnector_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getInterfaceShape()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceEndPoint::getInterfaceShape(DirectionTypes::Direction direction) const
{
    if (direction == DirectionTypes::IN)
    {
        return getDirectionInShape();
    }
    else if (direction == DirectionTypes::OUT)
    {
        return getDirectionOutShape();
    }
    else
    {
        return getDirectionInOutShape();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::name()
//-----------------------------------------------------------------------------
QString BusInterfaceEndPoint::name() const
{
    return busInterface_->name();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setName()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setName(QString const& name)
{
    beginUpdateConnectionNames();

    QString previousName = busInterface_->name();

    busInterface_->setName(name);

    updateName(previousName, name);

    updateInterface();
    emit contentChanged();

    endUpdateConnectionNames();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setBusInterface()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setBusInterface(QSharedPointer<BusInterface> newBus)
{
    busInterface_ = newBus;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::description()
//-----------------------------------------------------------------------------
QString BusInterfaceEndPoint::description() const
{
    if (busInterface_)
    {
        return busInterface_->description();
    }
    else
    {
        return QString("");
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setDescription()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setDescription(QString const& description)
{
    if (busInterface_)
    {
        busInterface_->setDescription(description);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::isExclusive()
//-----------------------------------------------------------------------------
bool BusInterfaceEndPoint::isExclusive() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> BusInterfaceEndPoint::getPort() const
{
    return QSharedPointer<Port>();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::isBus()
//-----------------------------------------------------------------------------
bool BusInterfaceEndPoint::isBus() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setInterfaceMode()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setInterfaceMode(General::InterfaceMode mode)
{
    if (busInterface_)
    {
        busInterface_->setInterfaceMode(mode);
        updateInterface();
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceEndPoint::getInterfaceMode() const
{
    return busInterface_->getInterfaceMode();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* BusInterfaceEndPoint::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getOldPosition()
//-----------------------------------------------------------------------------
QPointF BusInterfaceEndPoint::getOldPosition() const
{
    return oldPos_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::setOldPosition()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::setOldPosition(QPointF const& newPosition)
{
    oldPos_ = newPosition;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::saveOldPortPositions()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::saveOldPortPositions(QList<QGraphicsItem*> items)
{
    foreach (QGraphicsItem* item, items)
    {
        ConnectionEndpoint* endPointItem = dynamic_cast<ConnectionEndpoint*>(item);
        if (endPointItem && item != this && endPointItem->isHierarchical() == isHierarchical())
        {
            ConnectionEndpoint* port = static_cast<ConnectionEndpoint*>(item);
            oldPortPositions_.insert(port, port->pos());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::getOldPortPositions()
//-----------------------------------------------------------------------------
QMap<ConnectionEndpoint*, QPointF> BusInterfaceEndPoint::getOldPortPositions() const
{
    return oldPortPositions_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::clearOldPortPositions()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::clearOldPortPositions()
{
    oldPortPositions_.clear();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::beginUpdateConnectionPositions()
//-----------------------------------------------------------------------------
void BusInterfaceEndPoint::beginUpdateConnectionPositions()
{
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0)
        {
            conn->beginUpdatePosition();
        }
    }
}


//-----------------------------------------------------------------------------
// Function: BusInterfaceEndPoint::sceneIsLocked()
//-----------------------------------------------------------------------------
bool BusInterfaceEndPoint::sceneIsLocked() const
{
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram != 0 && diagram->isProtected())
    {
        return true;
    }

    return false;
}
