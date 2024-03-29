//-----------------------------------------------------------------------------
// File: SWPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 2.5.2012
//
// Description:
// Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------

#include "SWPortItem.h"

#include "SystemComponentItem.h"
#include "HWMappingItem.h"

#include <common/KactusColors.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/GenericEditProvider.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/DesignDiagram.h>
#include <editors/common/NamelabelWidth.h>
#include <editors/common/GraphicsItemLabel.h>
#include <editors/SystemDesign/UndoCommands/SystemMoveCommands.h>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: SWPortItem::SWPortItem()
//-----------------------------------------------------------------------------
SWPortItem::SWPortItem(QString const& name, QSharedPointer<Component> containingComponent, QGraphicsItem *parent):
SWConnectionEndpoint(containingComponent, name, parent),
stubLine_(0, 0, 0, -GridSize, this)
{
    setType(ENDPOINT_TYPE_UNDEFINED);
    setTypeLocked(false);
    SWPortItem::setTemporary(true);
    SWPortItem::initialize();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::SWPortItem()
//-----------------------------------------------------------------------------
SWPortItem::SWPortItem(QSharedPointer<ApiInterface> apiIf, QSharedPointer<Component> containingComponent,
    QGraphicsItem *parent):
SWConnectionEndpoint(containingComponent, apiIf->name(), parent),
apiInterface_(apiIf),
stubLine_(0, 0, 0, -GridSize, this)
{
    Q_ASSERT(apiIf != 0);
    setType(ENDPOINT_TYPE_API);
    setTypeLocked(true);
    SWPortItem::initialize();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::SWPortItem()
//-----------------------------------------------------------------------------
SWPortItem::SWPortItem(QSharedPointer<ComInterface> comIf, QSharedPointer<Component> containingComponent,
    QGraphicsItem *parent):
SWConnectionEndpoint(containingComponent, comIf->name(), parent),
comInterface_(comIf),
stubLine_(0, 0, 0, -GridSize, this)
{
    Q_ASSERT(comIf != 0);
    setType(ENDPOINT_TYPE_COM);
    setTypeLocked(true);
    SWPortItem::initialize();
}


//-----------------------------------------------------------------------------
// Function: SWPortItem::name()
//-----------------------------------------------------------------------------
QString SWPortItem::name() const
{
    if (isCom())
    {
        return comInterface_->name();
    }
    else if (isApi())
    {
        return apiInterface_->name();
    }
    else
    {
        return getNameLabel()->getText();
    }
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void SWPortItem::setName(QString const& name)
{
    beginUpdateConnectionNames();


    if (isCom())
    {
        comInterface_->setName(name);
    }
    else if (isApi())
    {
        apiInterface_->setName(name);
    }
    else
    {
        getNameLabel()->setText(name);
    }

	updateInterface();
    endUpdateConnectionNames();
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

    if (isInvalid() || hasInvalidConnections())
    {
        stubLineDefaultPen_.setColor(Qt::red);
    }
    else
    {
        if (getType() == ENDPOINT_TYPE_COM)
        {
            stubLineDefaultPen_.setColor(KactusColors::COM_CONNECTION);
        }
        else
        {
            stubLineDefaultPen_.setColor(Qt::black);
        }
    }

    stubLine_.setPen(stubLineDefaultPen_);

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
        if (comInterface_->getDirection() == DirectionTypes::IN)
        {
            /*  ||
             *  \/
             */
            shape << QPointF(-squareSize/2, 0)
                << QPointF(-squareSize/2, -squareSize/2)
                << QPointF(squareSize/2, -squareSize/2)
                << QPointF(squareSize/2, 0)
                << QPointF(0, squareSize/2);
        }
        else if (comInterface_->getDirection() == DirectionTypes::OUT)
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
    getNameLabel()->updateLabelGeometry();

	setLabelPosition();

    getOffPageConnector()->updateInterface();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::isHierachical()
//-----------------------------------------------------------------------------
bool SWPortItem::isHierarchical() const noexcept
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::onConnect()
//-----------------------------------------------------------------------------
bool SWPortItem::onConnect(ConnectionEndpoint const* other)
{
    // If the port is undefined, try to copy the configuration from the other end point.
    if (!isInvalid() && getType() == ENDPOINT_TYPE_UNDEFINED)
    {
        if (other->getType() == ENDPOINT_TYPE_API)
        {
            apiInterface_ = QSharedPointer<ApiInterface>(new ApiInterface());
            apiInterface_->setName(getNameLabel()->getText());
            apiInterface_->setApiType(other->getApiInterface()->getApiType());
            
            if (other->isHierarchical())
            {
                apiInterface_->setDependencyDirection(other->getApiInterface()->getDependencyDirection());
            }
            else if (other->getApiInterface()->getDependencyDirection() == DEPENDENCY_PROVIDER)
            {
                apiInterface_->setDependencyDirection(DEPENDENCY_REQUESTER);
            }
            else
            {
                apiInterface_->setDependencyDirection(DEPENDENCY_PROVIDER);
            }

            if (!isInvalid())
            {
                getOwnerComponent()->getVendorExtensions()->append(apiInterface_);
            }
        }
        else if (other->getType() == ENDPOINT_TYPE_COM)
        {
            comInterface_ = QSharedPointer<ComInterface>(new ComInterface());
            comInterface_->setName(getNameLabel()->getText());
            comInterface_->setComType(other->getComInterface()->getComType());
            comInterface_->setTransferType(other->getComInterface()->getTransferType());

            if (other->isHierarchical())
            {
                comInterface_->setDirection(other->getComInterface()->getDirection());
            }
            else
            {
                DirectionTypes::Direction direction = other->getComInterface()->getDirection();
                if (direction == DirectionTypes::IN)
                {
                    comInterface_->setDirection(DirectionTypes::OUT);
                }
                else if (direction == DirectionTypes::OUT)
                {
                    comInterface_->setDirection(DirectionTypes::IN);
                }
                else if (direction == DirectionTypes::INOUT)
                {
                    comInterface_->setDirection(DirectionTypes::INOUT);
                }
                else
                {
                    Q_ASSERT(false);
                }
            }

            if (!isInvalid())
            {
                getOwnerComponent()->getVendorExtensions()->append(comInterface_);
            }
        }

        setType(other->getType());
        updateInterface();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::onDisconnect()
//-----------------------------------------------------------------------------
void SWPortItem::onDisconnect()
{
    // Undefine the interface if it is temporary.
    if (!isTypeLocked() && !isConnected())
    {
        setTypeDefinition(VLNV());
        updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::isConnectionValid()
//-----------------------------------------------------------------------------
bool SWPortItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!SWConnectionEndpoint::isConnectionValid(other))
    {
        return false;
    }

    // Disallow normal API connections across different HW.
    if ((getType() == ENDPOINT_TYPE_API || other->getType() == ENDPOINT_TYPE_API) &&
        !other->isHierarchical() &&
        static_cast<SystemComponentItem*>(encompassingComp())->getLinkedHW() !=
        static_cast<SystemComponentItem*>(other->encompassingComp())->getLinkedHW())
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

            // Check if the COM types are not compatible.
            if (!comIf1->getComType().isEmpty() && !comIf2->getComType().isEmpty() &&
                comIf1->getComType() != comIf2->getComType())
            {
                return false;
            }

            // Check if the data types are not compatible.
            if (!comIf1->getTransferType().isEmpty() && !comIf2->getTransferType().isEmpty() &&
                comIf1->getTransferType() != comIf2->getTransferType())
            {
                return false;
            }

            // If the other one is a hierarchical, then the direction must be the same.
            // Otherwise they must be just compatible (in <-> out or any <-> inout).
            return ((other->isHierarchical() && comIf1->getDirection() == comIf2->getDirection()) ||
                    (!other->isHierarchical() && (comIf1->getDirection() == DirectionTypes::INOUT ||
                                                  comIf2->getDirection() == DirectionTypes::INOUT ||
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
// Function: addConnection()
//-----------------------------------------------------------------------------
void SWPortItem::addConnection(GraphicsConnection* connection)
{
    SWConnectionEndpoint::addConnection(connection);
    stubLine_.setVisible(true);
    
    if (connection->isInvalid())
    {
        stubLineDefaultPen_.setColor(Qt::red);
        stubLine_.setPen(stubLineDefaultPen_);
    }
}

//-----------------------------------------------------------------------------
// Function: removeInterconnection()
//-----------------------------------------------------------------------------
void SWPortItem::removeConnection(GraphicsConnection* connection)
{
    SWConnectionEndpoint::removeConnection(connection);

    if (connection->isInvalid())
    {
        // Check if there no other invalid connections.
        if (!hasInvalidConnections())
        {
            if (getType() == ENDPOINT_TYPE_COM)
            {
                stubLineDefaultPen_.setColor(KactusColors::COM_CONNECTION);
            }
            else
            {
                stubLineDefaultPen_.setColor(Qt::black);
            }

            stubLine_.setPen(stubLineDefaultPen_);
        }
    }

    if (getConnections().isEmpty())
    {
        stubLine_.setVisible(false);
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::itemChange()
//-----------------------------------------------------------------------------
QVariant SWPortItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        if (!parentItem())
        {
            return snapPointToGrid(value.toPointF());
        }

        QPointF pos = value.toPointF();
        QRectF parentRect = static_cast<SystemComponentItem*>(parentItem())->rect();

        if (pos.x() < 0)
        {
            pos.setX(parentRect.left());
        }
        else
        {
            pos.setX(parentRect.right());
        }

        return snapPointToGrid(pos);
    }
    else if (change == ItemPositionHasChanged)
    {
        if (parentItem())
        {
            checkDirection();
            setLabelPosition();
        }
    }
    else if (change == ItemRotationHasChanged)
    {
        getNameLabel()->setRotation(-rotation());
    }
    else if (change == ItemScenePositionHasChanged)
    {
        // Check if the updates are not disabled.
        if (!static_cast<SystemComponentItem*>(parentItem())->isConnectionUpdateDisabled())
        {
            // Update the connections.
            updateConnectionPositions();
        }

        // Update the stub length if the parent's parent is a HW mapping item.
        auto mappingItem = dynamic_cast<HWMappingItem*>(parentItem()->parentItem());

        if (mappingItem != 0)
        {
            if (pos().x() < 0)
            {
                stubLine_.setLine(0, 0, 0, qMin(0.0, mappingItem->sceneBoundingRect().left() - scenePos().x()));
            }
            else
            {
                stubLine_.setLine(0, 0, 0, qMin(0.0, scenePos().x() - mappingItem->sceneBoundingRect().right()));
            }
        }
        else
        {
            stubLine_.setLine(0, 0, 0, -GridSize);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool SWPortItem::isDirectionFixed() const noexcept
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void SWPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the diagram is protected.
    auto diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram != nullptr && diagram->isProtected())
    {
        return;
    }

    SWConnectionEndpoint::mouseMoveEvent(event);
    static_cast<SystemComponentItem*>(parentItem())->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::mousePressEvent()
//-----------------------------------------------------------------------------
void SWPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    SWConnectionEndpoint::mousePressEvent(event);    
    oldPos_ = pos();

    // Save old port positions for all ports in the parent component.
    for (QGraphicsItem* item : parentItem()->childItems())
    {
        if (dynamic_cast<SWPortItem*>(item) != nullptr && item != this)
        {
            auto port = static_cast<SWPortItem*>(item);
            oldPortPositions_.insert(port, port->pos());
        }
    }

    beginUpdateConnectionPositions();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SWPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    SWConnectionEndpoint::mouseReleaseEvent(event);

    auto diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == nullptr)
    {
        // Update the default position in case the graphics are located in other scene than the designer.
        if (comInterface_ != 0)
        {
            comInterface_->setDefaultPos(pos());
        }
        else if (apiInterface_ != 0)
        {
            apiInterface_->setDefaultPos(pos());
        }

        if (oldPos_ != pos())
        {
            emit moved(this);
        }

        return;
    }

    QSharedPointer<QUndoCommand> cmd;

    // Check if the port position was really changed.
    if (oldPos_ != pos())
    {
        cmd = QSharedPointer<QUndoCommand>(new SWPortMoveCommand(this, oldPos_, diagram));
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
            new SWPortMoveCommand(cur.key(), cur.value(), diagram, cmd.data());
        }

        ++cur;
    }

    oldPortPositions_.clear();
    
    // End the position update for all connections.
    endUpdateConnectionPositions(cmd.data());

    // Add the undo command to the edit stack only if it has changes.
    if (cmd->childCount() > 0 || oldPos_ != pos())
    {
        diagram->getEditProvider()->addCommand(cmd);
        cmd->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::description()
//-----------------------------------------------------------------------------
QString SWPortItem::description() const
{
	if (isCom())
    {
	    return comInterface_->description();
    }
    else if (isApi())
    {
        return apiInterface_->description();
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

    stubLineDefaultPen_ = stubLine_.pen();
    stubLineDefaultPen_.setWidth(3);
    stubLine_.setPen(stubLineDefaultPen_);

    SWConnectionEndpoint::initialize();

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
        curPen = stubLineDefaultPen_;
    }

    stubLine_.setPen(curPen);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::setTypeDefinition()
//-----------------------------------------------------------------------------
void SWPortItem::setTypeDefinition(VLNV const& type)
{
    // Disconnect existing connections before setting the type.
    for (GraphicsConnection* conn : getConnections())
    {
        if (conn->endpoint1() != this)
        {
            conn->endpoint1()->removeConnection(conn);
            conn->endpoint1()->onDisconnect();
            conn->endpoint1()->addConnection(conn);
        }
        else
        {
            conn->endpoint2()->removeConnection(conn);
            conn->endpoint2()->onDisconnect();
            conn->endpoint2()->addConnection(conn);
        }
    }

    // Check if the type is valid.
    if (type.isValid())
    {
        if (type.getType() == VLNV::APIDEFINITION)
        {
            apiInterface_ = QSharedPointer<ApiInterface>(new ApiInterface());
            apiInterface_->setName(getNameLabel()->getText());
            apiInterface_->setApiType(type);

            getOwnerComponent()->getVendorExtensions()->append(apiInterface_);

            setType(ENDPOINT_TYPE_API);
            setTypeLocked(true);
        }
        else if (type.getType() == VLNV::COMDEFINITION)
        {
            comInterface_ = QSharedPointer<ComInterface>(new ComInterface());
            comInterface_->setName(getNameLabel()->getText());
            comInterface_->setComType(type);
            getOwnerComponent()->getVendorExtensions()->append(comInterface_);

            setType(ENDPOINT_TYPE_COM);
            setTypeLocked(true);
        }
    }
    else
    {
        if (apiInterface_ != 0)
        {
            if (!isInvalid())
            {
                getOwnerComponent()->getVendorExtensions()->removeOne(apiInterface_);
            }

            apiInterface_.clear();
        }

        if (comInterface_ != 0)
        {
            if (!isInvalid())
            {
                getOwnerComponent()->getVendorExtensions()->removeOne(comInterface_);
            }

            comInterface_.clear();
        }

        setType(ENDPOINT_TYPE_UNDEFINED);
        setTypeLocked(false);
    }

    updateInterface();

    if (getType() != ENDPOINT_TYPE_UNDEFINED)
    {
        // Undefined endpoints of the connections can now be defined.
        for (GraphicsConnection* conn : getConnections())
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

    for (GraphicsConnection* conn : getConnections())
    {
        conn->validate();
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

//-----------------------------------------------------------------------------
// Function: SWPortItem::isExclusive()
//-----------------------------------------------------------------------------
bool SWPortItem::isExclusive() const
{
    // COM interfaces are always exclusive.
    return (getType() == ENDPOINT_TYPE_COM);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::hasInvalidConnections()
//-----------------------------------------------------------------------------
bool SWPortItem::hasInvalidConnections()
{
    // Check if there are no other invalid connections.
    for (GraphicsConnection* otherConn : getConnections())
    {
        if (otherConn->isInvalid())
        {
            return true;
        }
    }

    return false;
}


//-----------------------------------------------------------------------------
// Function: SWPortItem::setLabelPosition()
//-----------------------------------------------------------------------------
void SWPortItem::setLabelPosition()
{
    qreal nameWidth = getNameLabel()->boundingRect().width();
    qreal nameHeight = getNameLabel()->boundingRect().height();

	if (pos().x() < 0)
	{
        getNameLabel()->setPos(nameHeight/2, GridSize);
	}
	// Otherwise the port is directed to the right.
	else
	{
        getNameLabel()->setPos(-nameHeight/2, GridSize + nameWidth);
	}
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::checkDirection()
//-----------------------------------------------------------------------------
void SWPortItem::checkDirection()
{
	if (pos().x() < 0)
	{
		setDirection(QVector2D(-1.0f, 0.0f));
	}
	// Otherwise the port is directed to the right.
	else
	{
		setDirection(QVector2D(1.0f, 0.0f));
	}
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::getNameLength()
//-----------------------------------------------------------------------------
qreal SWPortItem::getNameLength()
{
    QFont font = getNameLabel()->font();

	return NamelabelWidth::getTextLength(name(), font);
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::shortenNameLabel()
//-----------------------------------------------------------------------------
void SWPortItem::shortenNameLabel( qreal width )
{
    QFont font = getNameLabel()->font();
	QString nameLabelText = NamelabelWidth::setNameLabel(name(), font, width);
    getNameLabel()->setText(nameLabelText);

	setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::isCom()
//-----------------------------------------------------------------------------
bool SWPortItem::isCom() const noexcept
{
    return comInterface_ != nullptr;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::isApi()
//-----------------------------------------------------------------------------
bool SWPortItem::isApi() const noexcept
{
    return apiInterface_ != nullptr;
}
