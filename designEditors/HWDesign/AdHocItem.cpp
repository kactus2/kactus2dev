//-----------------------------------------------------------------------------
// File: AdHocItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.05.2016
//
// Description:
// Parent graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "AdHocItem.h"

#include <designEditors/HWDesign/OffPageConnectorItem.h>
#include <designEditors/common/diagramgrid.h>

#include <QPen>
#include <QFont>
#include <QGraphicsDropShadowEffect>

const int TIEOFFITEM_DISTANCE = 15;

//-----------------------------------------------------------------------------
// Function: AdHocItem::AdHocItem()
//-----------------------------------------------------------------------------
AdHocItem::AdHocItem(QSharedPointer<Port> port, QGraphicsItem* parent, QVector2D const& dir):
HWConnectionEndpoint(parent, dir),
nameLabel_("", this),
port_(port),
offPageConnector_(),
tieoffItem_()
{
    Q_ASSERT_X(port, "AdHocPortItem constructor", "Null Port pointer given as parameter");

    setType(ENDPOINT_TYPE_ADHOC);

    QFont font = nameLabel_.font();
    font.setPointSize(8);
    nameLabel_.setFont(font);
    nameLabel_.setFlag(ItemStacksBehindParent);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
    nameLabel_.setGraphicsEffect(shadow);

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
// Function: AdHocItem::getPortShape()
//-----------------------------------------------------------------------------
QPolygonF AdHocItem::getPortShape()
{
    int squareSize = GridSize - 4;

    QPolygonF shape;
    if (port_->getDirection() == DirectionTypes::IN)
    {
        shape = getInPortShape(squareSize);
    }
    else if (port_->getDirection() == DirectionTypes::OUT)
    {
        shape = getOutPortShape(squareSize);
    }
    else
    {
        shape << QPointF(-squareSize / 2, squareSize / 2)
            << QPointF(-squareSize / 2, -squareSize / 2)
            << QPointF(0, -squareSize)
            << QPointF(squareSize / 2, -squareSize / 2)
            << QPointF(squareSize / 2, squareSize / 2)
            << QPointF(0, squareSize);
    }

    return shape;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::~AdHocItem()
//-----------------------------------------------------------------------------
AdHocItem::~AdHocItem()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocItem::setTypes()
//-----------------------------------------------------------------------------
void AdHocItem::setTypes(VLNV const& , VLNV const& , General::InterfaceMode )
{

}

//-----------------------------------------------------------------------------
// Function: AdHocItem::name()
//-----------------------------------------------------------------------------
QString AdHocItem::name() const
{
    return port_->name();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::setName()
//-----------------------------------------------------------------------------
void AdHocItem::setName(const QString& name)
{
    beginUpdateConnectionNames();

    port_->setName(name);
    updateInterface();

    emit contentChanged();

    endUpdateConnectionNames();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::description()
//-----------------------------------------------------------------------------
QString AdHocItem::description() const
{
    Q_ASSERT(port_);
    return port_->description();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::setDescription()
//-----------------------------------------------------------------------------
void AdHocItem::setDescription(const QString& description)
{
    Q_ASSERT(port_);
    port_->setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::onConnect()
//-----------------------------------------------------------------------------
bool AdHocItem::onConnect(ConnectionEndpoint const* )
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::onDisconnect()
//-----------------------------------------------------------------------------
void AdHocItem::onDisconnect(ConnectionEndpoint const* )
{

}

//-----------------------------------------------------------------------------
// Function: AdHocItem::isExclusive()
//-----------------------------------------------------------------------------
bool AdHocItem::isExclusive() const
{
    // Ad-hoc ports are always non-exclusive.
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> AdHocItem::getBusInterface() const
{
    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::()
//-----------------------------------------------------------------------------
bool AdHocItem::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> AdHocItem::getPort() const
{
    return port_;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::setInterfaceMode()
//-----------------------------------------------------------------------------
void AdHocItem::setInterfaceMode(General::InterfaceMode )
{

}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* AdHocItem::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::createNonResolvableTieOff()
//-----------------------------------------------------------------------------
void AdHocItem::createNonResolvableTieOff()
{
    removeTieOffItem();

    QPainterPath path;

    path.addRect(0, -TIEOFFITEM_DISTANCE, 1, TIEOFFITEM_DISTANCE);

    tieoffItem_ = new QGraphicsPathItem(path, this);

    QRectF rectangle(-4, -(TIEOFFITEM_DISTANCE + 4), 8, 8);

    QPen usedPen;
    usedPen.setWidth(2);

    QBrush usedBrush(Qt::white);

    QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(rectangle, tieoffItem_);
    ellipse->setPen(usedPen);
    ellipse->setBrush(usedBrush);
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::createNumberedTieOff()
//-----------------------------------------------------------------------------
void AdHocItem::createNumberedTieOff()
{
    removeTieOffItem();

    QPainterPath path;

    path.addRect(0, -TIEOFFITEM_DISTANCE, 1, TIEOFFITEM_DISTANCE);

    tieoffItem_ = new QGraphicsPathItem(path, this);

    QPolygonF blackArrow;
    blackArrow << QPointF(6, -TIEOFFITEM_DISTANCE)
        << QPointF(0, -(TIEOFFITEM_DISTANCE + 6))
        << QPointF(-6, -TIEOFFITEM_DISTANCE);

    QBrush arrowBrush (Qt::black);

    QGraphicsPolygonItem* arrowItem =  new QGraphicsPolygonItem(blackArrow, tieoffItem_);
    arrowItem->setBrush(arrowBrush);
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::createHighTieOff()
//-----------------------------------------------------------------------------
void AdHocItem::createHighTieOff()
{
    removeTieOffItem();

    QPainterPath path;

    path.addRect(0, -TIEOFFITEM_DISTANCE, 1, TIEOFFITEM_DISTANCE);

    tieoffItem_ = new QGraphicsPathItem(path, this);

    QPolygonF arrow;
    arrow << QPointF(6, -TIEOFFITEM_DISTANCE)
        << QPointF(0, -(TIEOFFITEM_DISTANCE + 6))
        << QPointF(-6, -TIEOFFITEM_DISTANCE);

    QBrush arrowBrush (Qt::black);

    QGraphicsPolygonItem* arrowItem =  new QGraphicsPolygonItem(arrow, tieoffItem_);
    arrowItem->setBrush(arrowBrush);
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::createLowTieOff()
//-----------------------------------------------------------------------------
void AdHocItem::createLowTieOff()
{
    removeTieOffItem();

    QPainterPath path;

    path.addRect(9, -TIEOFFITEM_DISTANCE, -18, 1);
    path.addRect(6, -(TIEOFFITEM_DISTANCE + 3), -12, 1);
    path.addRect(3, -(TIEOFFITEM_DISTANCE + 6), -6, 1);
    path.addRect(0, -TIEOFFITEM_DISTANCE, 1, TIEOFFITEM_DISTANCE);

    tieoffItem_ = new QGraphicsPathItem(path, this);
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::removeTieOffItem()
//-----------------------------------------------------------------------------
void AdHocItem::removeTieOffItem()
{
    if (tieoffItem_)
    {
        childItems().removeAll(tieoffItem_);

        delete tieoffItem_;
        tieoffItem_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getNameLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem& AdHocItem::getNameLabel()
{
    return nameLabel_;
}
