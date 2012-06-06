//-----------------------------------------------------------------------------
// File: SWPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.5.2012
//
// Description:
// Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------

#include "SWPortItem.h"

#include "SWComponentItem.h"
#include "HWMappingItem.h"

#include "SystemMoveCommands.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>
#include <common/DesignDiagram.h>
#include <common/KactusColors.h>

#include <models/ApiInterface.h>
#include <models/ComInterface.h>
#include <models/component.h>
#include <models/ApiDefinition.h>
#include <models/ComDefinition.h>

#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: SWPortItem::SWPortItem()
//-----------------------------------------------------------------------------
SWPortItem::SWPortItem(QString const& name, QGraphicsItem *parent)
    : SWConnectionEndpoint(parent),
      nameLabel_(name, this),
      apiInterface_(),
      comInterface_(),
      temp_(true),
      oldPos_(),
      oldPortPositions_(),
      stubLine_(0, 0, 0, -GridSize, this)
{
    setType(ENDPOINT_TYPE_UNDEFINED);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::SWPortItem()
//-----------------------------------------------------------------------------
SWPortItem::SWPortItem(QSharedPointer<ApiInterface> apiIf, QGraphicsItem *parent)
    : SWConnectionEndpoint(parent),
      nameLabel_(this),
      apiInterface_(apiIf),
      comInterface_(),
      temp_(false),
      oldPos_(),
      oldPortPositions_(),
      stubLine_(0, 0, 0, -GridSize, this)
{
    Q_ASSERT(apiIf != 0);
    setType(ENDPOINT_TYPE_API);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::SWPortItem()
//-----------------------------------------------------------------------------
SWPortItem::SWPortItem(QSharedPointer<ComInterface> comIf, QGraphicsItem *parent)
    : SWConnectionEndpoint(parent),
      nameLabel_(this),
      apiInterface_(),
      comInterface_(comIf),
      temp_(false),
      oldPos_(),
      oldPortPositions_(),
      stubLine_(0, 0, 0, -GridSize, this)
{
    Q_ASSERT(comIf != 0);
    setType(ENDPOINT_TYPE_COM);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::~SWPortItem()
//-----------------------------------------------------------------------------
SWPortItem::~SWPortItem()
{
}

//-----------------------------------------------------------------------------
// Function: setTemporary()
//-----------------------------------------------------------------------------
void SWPortItem::setTemporary(bool temp)
{
    temp_ = temp;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::name()
//-----------------------------------------------------------------------------
QString SWPortItem::name() const
{
    if (isCom())
    {
        return comInterface_->getName();
    }
    else if (isApi())
    {
        return apiInterface_->getName();
    }
    else
    {
        return nameLabel_.toPlainText();
    }
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void SWPortItem::setName(const QString& name)
{
    if (isCom())
    {
        comInterface_->setName(name);
        encompassingComp()->componentModel()->updateComInterface(comInterface_.data());
    }
    else if (isApi())
    {
        apiInterface_->setName(name);
        encompassingComp()->componentModel()->updateApiInteface(apiInterface_.data());
    }
    else
    {
        nameLabel_.setPlainText(name);
    }

	updateInterface();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::getComInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterface> SWPortItem::getComInterface() const
{
    return comInterface_;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::getApiInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterface> SWPortItem::getApiInterface() const
{
    return apiInterface_;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::updateInterface()
//-----------------------------------------------------------------------------
void SWPortItem::updateInterface()
{
    SWConnectionEndpoint::updateInterface();

    // Update the polygon shape based on the direction.
    int squareSize = GridSize;
    QPolygonF shape;

    if (isApi())
    {
        if (apiInterface_->getDependencyDirection() == DEPENDENCY_PROVIDER)
        {
            /*  /\
             *  ||
             */
            shape << QPointF(-squareSize/2, squareSize/2)
                  << QPointF(-squareSize/2, 0)
                  << QPointF(0, -squareSize/2)
                  << QPointF(squareSize/2, 0)
                  << QPointF(squareSize/2, squareSize/2);
        }
        else
        {
            /*  \/
             *  ||
             */
            shape << QPointF(-squareSize/2, squareSize/2)
                  << QPointF(-squareSize/2, -squareSize/2)
                  << QPointF(0, 0)
                  << QPointF(squareSize/2, -squareSize/2)
                  << QPointF(squareSize/2, squareSize/2);
        }
    }
    else if (isCom())
    {
        switch (comInterface_->getDirection())
        {
        case General::IN:
            {
                /*  ||
                 *  \/
                 */
                shape << QPointF(-squareSize/2, 0)
                      << QPointF(-squareSize/2, -squareSize/2)
                      << QPointF(squareSize/2, -squareSize/2)
                      << QPointF(squareSize/2, 0)
                      << QPointF(0, squareSize/2);
                break;
            }

        case General::OUT:
            {
                /*  /\
                 *  ||
                 */
                shape << QPointF(-squareSize/2, squareSize/2)
                      << QPointF(-squareSize/2, 0)
                      << QPointF(0, -squareSize/2)
                      << QPointF(squareSize/2, 0)
                      << QPointF(squareSize/2, squareSize/2);
                break;
            }

        case General::INOUT:
            {
                /*  /\
                 *  ||
                 *  \/
                 */
                shape << QPointF(-squareSize/2, squareSize/2)
                      << QPointF(-squareSize/2, 0)
                      << QPointF(0, -squareSize/2)
                      << QPointF(squareSize/2, 0)
                      << QPointF(squareSize/2, squareSize/2)
                      << QPointF(0, squareSize);
                break;
            }
        }
    }
    else
    {
        /*  /\
         *  ||
         */
        shape << QPointF(-squareSize/2, squareSize/2)
              << QPointF(-squareSize/2, 0)
              << QPointF(0, -squareSize/2)
              << QPointF(squareSize/2, 0)
              << QPointF(squareSize/2, squareSize/2);
    }

    setPolygon(shape);

    // Update the name label.
    nameLabel_.setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + name() + "</div>");

    qreal nameWidth = nameLabel_.boundingRect().width();
    qreal nameHeight = nameLabel_.boundingRect().height();

    if (pos().x() < 0)
    {
        nameLabel_.setPos(nameHeight / 2, GridSize);
    }
    else
    {
        nameLabel_.setPos(-nameHeight / 2, GridSize + nameWidth);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::isHierachical()
//-----------------------------------------------------------------------------
bool SWPortItem::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::onConnect()
//-----------------------------------------------------------------------------
bool SWPortItem::onConnect(ConnectionEndpoint const* other)
{
    // If the port is undefined, try to copy the configuration from the other end point.
    if (getType() == ENDPOINT_TYPE_UNDEFINED)
    {
        if (other->getType() == ENDPOINT_TYPE_API)
        {
            apiInterface_ = QSharedPointer<ApiInterface>(new ApiInterface());
            apiInterface_->setName(nameLabel_.toPlainText());
            apiInterface_->setApiType(other->getApiInterface()->getApiType());
            
            if (other->getApiInterface()->getDependencyDirection() == DEPENDENCY_PROVIDER)
            {
                apiInterface_->setDependencyDirection(DEPENDENCY_REQUESTER);
            }
            else
            {
                apiInterface_->setDependencyDirection(DEPENDENCY_PROVIDER);
            }

            getOwnerComponent()->addApiInterface(apiInterface_);
        }
        else if (other->getType() == ENDPOINT_TYPE_COM)
        {
            comInterface_ = QSharedPointer<ComInterface>(new ComInterface());
            comInterface_->setName(nameLabel_.toPlainText());
            comInterface_->setComType(other->getComInterface()->getComType());
            comInterface_->setDataType(other->getComInterface()->getDataType());

            switch (other->getComInterface()->getDirection())
            {
            case General::IN:
                {
                    comInterface_->setDirection(General::OUT);
                    break;
                }
                
            case General::OUT:
                {
                    comInterface_->setDirection(General::IN);
                    break;
                }

            case General::INOUT:
                {
                    comInterface_->setDirection(General::INOUT);
                    break;
                }
            }

            getOwnerComponent()->addComInterface(comInterface_);
        }

        setType(other->getType());
        updateInterface();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::onDisconnect()
//-----------------------------------------------------------------------------
void SWPortItem::onDisconnect(ConnectionEndpoint const*)
{
    // Undefine the interface if it is temporary.
    if (temp_ && !isConnected())
    {
        setTypeDefinition(VLNV());
        updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::canConnect()
//-----------------------------------------------------------------------------
bool SWPortItem::canConnect(ConnectionEndpoint const* other) const
{
    if (!SWConnectionEndpoint::canConnect(other))
    {
        return false;
    }

    // Disallow normal API connections across different HW.
    if ((getType() == ENDPOINT_TYPE_API || other->getType() == ENDPOINT_TYPE_API) &&
        !other->isHierarchical() &&
        static_cast<SWComponentItem*>(encompassingComp())->getLinkedHW() !=
        static_cast<SWComponentItem*>(other->encompassingComp())->getLinkedHW())
    {
        return false;
    }

    // Check compatibility of the interfaces based on the connection type.
    bool fullyDefined = getType() != ENDPOINT_TYPE_UNDEFINED && other->getType() != ENDPOINT_TYPE_UNDEFINED;

    if (fullyDefined)
    {
        if (getType() == ENDPOINT_TYPE_API)
        {
            QSharedPointer<ApiInterface> apiIf1 = getApiInterface();
            QSharedPointer<ApiInterface> apiIf2 = other->getApiInterface();

            // Provider can have only one connection.
            if (isConnected() && apiIf1->getDependencyDirection() == DEPENDENCY_PROVIDER)
            {
                return false;
            }

            // Check if the API types are not compatible.
            if (!apiIf1->getApiType().isEmpty() && !apiIf2->getApiType().isEmpty() &&
                apiIf1->getApiType() != apiIf2->getApiType())
            {
                return false;
            }

            // If the other one is a hierarchical, then the dependency directions must be the same.
            // Otherwise they must be different (provider <-> requester).
            return ((other->isHierarchical() && apiIf1->getDependencyDirection() == apiIf2->getDependencyDirection()) ||
                    (!other->isHierarchical() && apiIf1->getDependencyDirection() != apiIf2->getDependencyDirection()));
        }
        else if (getType() == ENDPOINT_TYPE_COM)
        {
            QSharedPointer<ComInterface> comIf1 = getComInterface();
            QSharedPointer<ComInterface> comIf2 = other->getComInterface();

            // Allow only one connection per endpoint.
            if (isConnected())
            {
                return false;
            }

            // Check if the COM types are not compatible.
            if (!comIf1->getComType().isEmpty() && !comIf2->getComType().isEmpty() &&
                comIf1->getComType() != comIf2->getComType())
            {
                return false;
            }

            // Check if the data types are not compatible.
            if (!comIf1->getDataType().isEmpty() && !comIf2->getDataType().isEmpty() &&
                comIf1->getDataType() != comIf2->getDataType())
            {
                return false;
            }

            // If the other one is a hierarchical, then the direction must be the same.
            // Otherwise they must be just compatible (in <-> out or any <-> inout).
            return ((other->isHierarchical() && comIf1->getDirection() == comIf2->getDirection()) ||
                    (!other->isHierarchical() && (comIf1->getDirection() == General::INOUT ||
                                                  comIf2->getDirection() == General::INOUT ||
                                                  comIf1->getDirection() != comIf2->getDirection())));
        }
        else
        {
            Q_ASSERT(false);
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* SWPortItem::encompassingComp() const
{
    return static_cast<ComponentItem*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> SWPortItem::getOwnerComponent() const
{
	ComponentItem* comp = encompassingComp();
	Q_ASSERT(comp);

	QSharedPointer<Component> compModel = comp->componentModel();
	Q_ASSERT(compModel);
	return compModel;
}

//-----------------------------------------------------------------------------
// Function: addConnection()
//-----------------------------------------------------------------------------
void SWPortItem::addConnection(GraphicsConnection* connection)
{
    SWConnectionEndpoint::addConnection(connection);
    stubLine_.setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: removeInterconnection()
//-----------------------------------------------------------------------------
void SWPortItem::removeConnection(GraphicsConnection* connection)
{
    SWConnectionEndpoint::removeConnection(connection);

    if (getConnections().size() == 0)
    {
        stubLine_.setVisible(false);
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::itemChange()
//-----------------------------------------------------------------------------
QVariant SWPortItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    switch (change)
    {
    case ItemPositionChange:
        {
            if (!parentItem())
            {
                return snapPointToGrid(value.toPointF());
            }

            QPointF pos = value.toPointF();
            QRectF parentRect = static_cast<SWComponentItem*>(parentItem())->rect();

            /*if (pos.y() - parentRect.bottom() >= -35.0 && std::abs(pos.x()) < 50.0)
            {
                pos.setY(parentRect.bottom());
            }
            else*/ if (pos.x() < 0)
            {
                pos.setX(parentRect.left());
            }
            else
            {
                pos.setX(parentRect.right());
            }

            return snapPointToGrid(pos);
        }

    case ItemPositionHasChanged:
        {
            if (!parentItem())
                break;

            qreal nameWidth = nameLabel_.boundingRect().width();
            qreal nameHeight = nameLabel_.boundingRect().height();
            QPointF newPos = pos();

            QRectF parentRect = static_cast<SWComponentItem*>(parentItem())->rect();

            // Check if the port is directed to the bottom.
            /*if (pos().y() - parentRect.bottom() >= -35.0 && std::abs(pos().x()) < 50.0)
            {
                setDirection(QVector2D(0.0f, 1.0f));
                nameLabel_.setPos(nameWidth / 2, GridSize * 3);
            }
            // Check if the port is directed to the left.
            else*/ if (pos().x() < 0)
            {
                setDirection(QVector2D(-1.0f, 0.0f));
                nameLabel_.setPos(nameHeight/2, GridSize);
            }
            // Otherwise the port is directed to the right.
            else
            {
                setDirection(QVector2D(1.0f, 0.0f));
                nameLabel_.setPos(-nameHeight/2, GridSize + nameWidth);
            }

            emit contentChanged();
            break;
        }

    case ItemScenePositionHasChanged:
        // Check if the updates are not disabled.
        if (!static_cast<SWComponentItem*>(parentItem())->isConnectionUpdateDisabled())
        {
            // Update the connections.
            foreach (GraphicsConnection* connection, getConnections())
            {
                connection->updatePosition();
            }

            // Update the stub length if the parent's parent is a HW mapping item.
            HWMappingItem* mappingItem = dynamic_cast<HWMappingItem*>(parentItem()->parentItem());

            if (mappingItem != 0)
            {
                if (pos().x() < 0)
                {
                    stubLine_.setLine(0, 0, 0, std::min(0.0, mappingItem->sceneBoundingRect().left() - scenePos().x()));
                }
                else
                {
                    stubLine_.setLine(0, 0, 0, std::min(0.0, scenePos().x() - mappingItem->sceneBoundingRect().right()));
                }
            }
            else
            {
                stubLine_.setLine(0, 0, 0, -GridSize);
            }
        }

        break;

    default:
        break;
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool SWPortItem::isDirectionFixed() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void SWPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    SWConnectionEndpoint::mouseMoveEvent(event);
    static_cast<SWComponentItem*>(parentItem())->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::mousePressEvent()
//-----------------------------------------------------------------------------
void SWPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    SWConnectionEndpoint::mousePressEvent(event);    
    oldPos_ = pos();

    // Save old port positions for all ports in the parent component.
    foreach (QGraphicsItem* item, parentItem()->childItems())
    {
        if (dynamic_cast<SWPortItem*>(item) != 0 && item != this)
        {
            SWPortItem* port = static_cast<SWPortItem*>(item);
            oldPortPositions_.insert(port, port->pos());
        }
    }

    // Begin the position update for the connections.
    foreach (GraphicsConnection* conn, getConnections())
    {
        conn->beginUpdatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SWPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    SWConnectionEndpoint::mouseReleaseEvent(event);

    QSharedPointer<QUndoCommand> cmd;

    // Check if the port position was really changed.
    if (oldPos_ != pos())
    {
        cmd = QSharedPointer<QUndoCommand>(new SWPortMoveCommand(this, oldPos_));
    }
    else
    {
        cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    // Determine if the other ports changed their position and create undo commands for them.
    QMap<SWPortItem*, QPointF>::iterator cur = oldPortPositions_.begin();

    while (cur != oldPortPositions_.end())
    {
        if (cur.key()->pos() != cur.value())
        {
            QUndoCommand* childCmd = new SWPortMoveCommand(cur.key(), cur.value(), cmd.data());
        }

        ++cur;
    }

    oldPortPositions_.clear();
    
    // End the position update of the connections.
    foreach (GraphicsConnection* conn, getConnections())
    {
        conn->endUpdatePosition(cmd.data());
    }

    // Add the undo command to the edit stack only if it has changes.
    if (cmd->childCount() > 0 || oldPos_ != pos())
    {
        static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::description()
//-----------------------------------------------------------------------------
QString SWPortItem::description() const
{
	if (isCom())
    {
	    return comInterface_->getDescription();
    }
    else if (isApi())
    {
        return apiInterface_->getDescription();
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::setDescription()
//-----------------------------------------------------------------------------
void SWPortItem::setDescription(QString const& description)
{
	if (isCom())
    {
	    comInterface_->setDescription(description);
    }
    else if (isApi())
    {
        apiInterface_->setDescription(description);
    }

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::initialize()
//-----------------------------------------------------------------------------
void SWPortItem::initialize()
{
    stubLine_.setFlag(ItemStacksBehindParent);
    stubLine_.setVisible(false);

    QPen newPen = stubLine_.pen();
    newPen.setWidth(3);
    stubLine_.setPen(newPen);

    QFont font = nameLabel_.font();
    font.setPointSize(8);
    nameLabel_.setFont(font);
    nameLabel_.setFlag(ItemIgnoresTransformations);
    nameLabel_.setFlag(ItemStacksBehindParent);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
    nameLabel_.setGraphicsEffect(shadow);

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::onBeginConnect()
//-----------------------------------------------------------------------------
void SWPortItem::onBeginConnect()
{
    stubLine_.setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::onEndConnect()
//-----------------------------------------------------------------------------
void SWPortItem::onEndConnect()
{
    if (!isConnected())
    {
        stubLine_.setVisible(false);
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::setSelectionHighlight()
//-----------------------------------------------------------------------------
void SWPortItem::setSelectionHighlight(bool on)
{
    SWConnectionEndpoint::setSelectionHighlight(on);

    QPen curPen = stubLine_.pen();
    
    if (on)
    {
        curPen.setColor(KactusColors::DIAGRAM_SELECTION);
    }
    else
    {
        curPen.setColor(Qt::black);
    }

    stubLine_.setPen(curPen);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::setTypeDefinition()
//-----------------------------------------------------------------------------
void SWPortItem::setTypeDefinition(VLNV const& type)
{
    // Disconnect existing connections before setting the type.
    foreach(GraphicsConnection* conn, getConnections())
    {
        if (conn->endpoint1() != this)
        {
            conn->endpoint1()->removeConnection(conn);
            conn->endpoint1()->onDisconnect(this);
            conn->endpoint1()->addConnection(conn);
        }
        else
        {
            conn->endpoint2()->removeConnection(conn);
            conn->endpoint2()->onDisconnect(this);
            conn->endpoint2()->addConnection(conn);
        }
    }

    // Check if the type is valid.
    if (type.isValid())
    {
        if (type.getType() == VLNV::APIDEFINITION)
        {
            apiInterface_ = QSharedPointer<ApiInterface>(new ApiInterface());
            apiInterface_->setName(nameLabel_.toPlainText());
            apiInterface_->setApiType(type);
            getOwnerComponent()->addApiInterface(apiInterface_);

            setType(ENDPOINT_TYPE_API);
            setTemporary(false);
        }
        else if (type.getType() == VLNV::COMDEFINITION)
        {
            comInterface_ = QSharedPointer<ComInterface>(new ComInterface());
            comInterface_->setName(nameLabel_.toPlainText());
            comInterface_->setComType(type);
            getOwnerComponent()->addComInterface(comInterface_);

            setType(ENDPOINT_TYPE_COM);
            setTemporary(false);
        }
    }
    else
    {
        if (apiInterface_ != 0)
        {
            getOwnerComponent()->removeApiInterface(apiInterface_.data());
            apiInterface_.clear();
        }

        if (comInterface_ != 0)
        {
            getOwnerComponent()->removeComInterface(comInterface_.data());
            comInterface_.clear();
        }

        setType(ENDPOINT_TYPE_UNDEFINED);
        setTemporary(true);
    }

    updateInterface();

    if (getType() != ENDPOINT_TYPE_UNDEFINED)
    {
        // Undefined endpoints of the connections can now be defined.
        foreach(GraphicsConnection* conn, getConnections())
        {
            if (conn->endpoint1() != this)
            {
                conn->endpoint1()->onConnect(this);
                conn->endpoint2()->onConnect(conn->endpoint1());
            }
            else
            {
                conn->endpoint2()->onConnect(this);
                conn->endpoint1()->onConnect(conn->endpoint2());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::getTypeDefinition()
//-----------------------------------------------------------------------------
VLNV SWPortItem::getTypeDefinition() const
{
    if (isCom())
    {
        return comInterface_->getComType();
    }
    else if (isApi())
    {
        return apiInterface_->getApiType();
    }
    else
    {
        return VLNV();
    }
}