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
tieOffLabel_(0),
tieOffPath_(0)
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

    this->setHandlesChildEvents(true);
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
// Function: AdHocItem::changeTieOffLabel()
//-----------------------------------------------------------------------------
void AdHocItem::changeTieOffLabel(QString const& tieOffExpression, QString const& tieOffValue,
    bool tieOffIsSupported)
{
    if (tieOffLabel_ == 0)
    {
        createTieOffLabel();
    }

    QString textColour = "#000000";
    QString tieOffText = tieOffValue;
    if (!tieOffIsSupported && tieOffExpression.compare("open", Qt::CaseInsensitive) != 0)
    {
        tieOffText = "n/a";
        textColour = "#FF0000";
    }

    if (tieOffText.size() > 4)
    {
        tieOffText = tieOffValue.left(4) + "...";
    }

    tieOffLabel_->setPlainText(tieOffText);
    tieOffLabel_->setToolTip(tieOffExpression);

    setTieOffLabelPosition();

    tieOffLabel_->setHtml(
        "<div style=\"color:" + textColour + "; background-color:#eeeeee; border-style:solid; border-width: 1px\">"
        + tieOffText + "</div>");
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::createTieOffLabel()
//-----------------------------------------------------------------------------
void AdHocItem::createTieOffLabel()
{
    tieOffLabel_ = new QGraphicsTextItem("", this);

    QFont font = tieOffLabel_->font();
    font.setPointSize(8);
    tieOffLabel_->setFont(font);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
    tieOffLabel_->setGraphicsEffect(shadow);

    tieOffLabel_->setRotation(-rotation());

    createTieOffPath();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::createTieOffPath()
//-----------------------------------------------------------------------------
void AdHocItem::createTieOffPath()
{
    if (tieOffPath_ == 0)
    {
        QPainterPath pathToTieOff;
        pathToTieOff.addRect(0, -TIEOFFITEM_DISTANCE, 1, TIEOFFITEM_DISTANCE);
        tieOffPath_ = new QGraphicsPathItem(pathToTieOff, this);
        tieOffPath_->setFlag(ItemStacksBehindParent);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::setTieOffLabelPosition()
//-----------------------------------------------------------------------------
void AdHocItem::setTieOffLabelPosition()
{
    if (tieOffLabel_ != 0)
    {
        qreal labelHeight = tieOffLabel_->boundingRect().height();
        qreal labelWidth = tieOffLabel_->boundingRect().width();

        if (labelShouldBeDrawnLeft())
        {
            tieOffLabel_->setPos(labelHeight / 2, -(TIEOFFITEM_DISTANCE-5) - labelWidth);
        }
        else
        {
            tieOffLabel_->setPos( -labelHeight / 2, -(TIEOFFITEM_DISTANCE-5));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::removeTieOffItem()
//-----------------------------------------------------------------------------
void AdHocItem::removeTieOffItem()
{
    if (tieOffPath_ != 0)
    {
        delete tieOffPath_;
        tieOffPath_ = 0;
    }

    if (tieOffLabel_ != 0)
    {
        delete tieOffLabel_;
        tieOffLabel_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getNameLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem& AdHocItem::getNameLabel()
{
    return nameLabel_;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getTieOffLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem* AdHocItem::getTieOffLabel()
{
    return tieOffLabel_;
}
