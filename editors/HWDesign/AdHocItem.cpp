//-----------------------------------------------------------------------------
// File: AdHocItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.05.2016
//
// Description:
// Parent graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "AdHocItem.h"

#include <common/graphicsItems/GraphicsConnection.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/DesignDiagram.h>
#include <editors/common/GraphicsItemLabel.h>

#include <QPen>
#include <QFont>
#include <QGraphicsDropShadowEffect>

const int TIEOFFITEM_DISTANCE = 20;

//-----------------------------------------------------------------------------
// Function: AdHocItem::AdHocItem()
//-----------------------------------------------------------------------------
AdHocItem::AdHocItem(QSharedPointer<Port> port, QSharedPointer<Component> containingComponent,
    QGraphicsItem* parent, QVector2D const& dir):
    HWConnectionEndpoint(port->name(), containingComponent, parent, dir),
    port_(port)
{
    Q_ASSERT_X(port, "AdHocPortItem constructor", "Null Port pointer given as parameter");
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getPortShape()
//-----------------------------------------------------------------------------
QPolygonF AdHocItem::getPortShape() const
{
    int squareSize = GridSize - 4;

    QPolygonF shape;
    if (port_->getDirection() == DirectionTypes::IN)
    {
        if (isHierarchical())
        {
            shape = getInPortShape(squareSize);
        }
        else
        {
            shape = getOutPortShape(squareSize);
        }
    }
    else if (port_->getDirection() == DirectionTypes::OUT)
    {
        if (isHierarchical())
        {
            shape = getOutPortShape(squareSize);
        }
        else
        {
            shape = getInPortShape(squareSize);
        }
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
// Function: AdHocItem::getInPortShape()
//-----------------------------------------------------------------------------
QPolygonF AdHocItem::getInPortShape(const int squareSize) const
{
    QPolygonF shape;

    int halfSquare = squareSize / 2;

    shape << QPointF(-halfSquare, halfSquare)
        << QPointF(-halfSquare, -halfSquare)
        << QPointF(0, -squareSize)
        << QPointF(halfSquare, -halfSquare)
        << QPointF(halfSquare, halfSquare);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getOutPortShape()
//-----------------------------------------------------------------------------
QPolygonF AdHocItem::getOutPortShape(const int squareSize) const
{
    QPolygonF shape;

    int halfSquare = squareSize / 2;

    shape << QPointF(-halfSquare, halfSquare)
        << QPointF(-halfSquare, -halfSquare)
        << QPointF(halfSquare, -halfSquare)
        << QPointF(halfSquare, halfSquare)
        << QPointF(0, squareSize);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::updateEndPointGraphics()
//-----------------------------------------------------------------------------
void AdHocItem::updateEndPointGraphics()
{
    if (adhocPortIsValid())
    {
        setBrush(QBrush(Qt::black));
    }
    else
    {
        setBrush(QBrush(Qt::red));
    }

    setTieOffLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::adhocPortIsValid()
//-----------------------------------------------------------------------------
bool AdHocItem::adhocPortIsValid() const
{
    bool portExists = (getOwnerComponent()->getPort(getPort()->name()).isNull() == false);
    return portExists;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::name()
//-----------------------------------------------------------------------------
std::string AdHocItem::name() const
{
    return port_->name().toStdString();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::description()
//-----------------------------------------------------------------------------
std::string AdHocItem::description() const
{
    Q_ASSERT(port_);
    if (port_)
    {
        return port_->description().toStdString();
    }
    else
    {
        return std::string();
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::setDescription()
//-----------------------------------------------------------------------------
void AdHocItem::setDescription(std::string_view description)
{
    Q_ASSERT(port_);
    if (port_)
    {
        port_->setDescription(description);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::onConnect()
//-----------------------------------------------------------------------------
bool AdHocItem::onConnect(ConnectionEndpoint const* )
{
    return true;
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
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::isAdHoc()
//-----------------------------------------------------------------------------
bool AdHocItem::isAdHoc() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getType()
//-----------------------------------------------------------------------------
ConnectionEndpoint::EndpointType AdHocItem::getType() const
{
    return ConnectionEndpoint::ENDPOINT_TYPE_ADHOC;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> AdHocItem::getPort() const
{
    return port_;
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

    tieOffLabel_->setText(tieOffText);
    tieOffLabel_->setToolTip(tieOffExpression);

    setTieOffLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::createTieOffLabel()
//-----------------------------------------------------------------------------
void AdHocItem::createTieOffLabel()
{
    tieOffLabel_ = new GraphicsItemLabel("", this);

    QFont font = tieOffLabel_->font();
    font.setPointSize(8);
    tieOffLabel_->setFont(font);
    tieOffLabel_->setRotation(-rotation());

    createTieOffPath();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::createTieOffPath()
//-----------------------------------------------------------------------------
void AdHocItem::createTieOffPath()
{
    if (tieOffPath_ == nullptr)
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
    if (tieOffLabel_ != nullptr)
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
    if (tieOffPath_ != nullptr)
    {
        delete tieOffPath_;
        tieOffPath_ = nullptr;
    }

    if (tieOffLabel_ != nullptr)
    {
        delete tieOffLabel_;
        tieOffLabel_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::getTieOffLabel()
//-----------------------------------------------------------------------------
GraphicsItemLabel* AdHocItem::getTieOffLabel()
{
    return tieOffLabel_;
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::isConnectionValid()
//-----------------------------------------------------------------------------
bool AdHocItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    return HWConnectionEndpoint::isConnectionValid(other) && !other->isBus();
}

//-----------------------------------------------------------------------------
// Function: AdHocItem::mousePressEvent()
//-----------------------------------------------------------------------------
void AdHocItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);

    saveOldPortPositions();

    beginUpdateConnectionPositions();
}
