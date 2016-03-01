//-----------------------------------------------------------------------------
// File: SWInterfaceItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.5.2012
//
// Description:
// Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------

#include "SWInterfaceItem.h"

#include "SWOffPageConnectorItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/GenericEditProvider.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>
#include <designEditors/SystemDesign/UndoCommands/SWInterfaceMoveCommand.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::SWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem::SWInterfaceItem(QSharedPointer<Component> component, QString const& name,
                                 QSharedPointer<InterfaceGraphicsData> interfaceGraphics, QGraphicsItem *parent):
SWConnectionEndpoint(parent, QVector2D(1.0f, 0.0f)),
nameLabel_(name, this),
component_(component),
comInterface_(),
apiInterface_(),
oldPos_(),
oldStack_(0),
oldInterfacePositions_(),
offPageConnector_(0),
graphicsData_(interfaceGraphics)
{
    setType(ENDPOINT_TYPE_UNDEFINED);
    setTypeLocked(false);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::SWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem::SWInterfaceItem(QSharedPointer<Component> component, QSharedPointer<ApiInterface> apiIf,
                                 QSharedPointer<InterfaceGraphicsData> interfaceGraphics, QGraphicsItem *parent):
SWConnectionEndpoint(parent, QVector2D(1.0f, 0.0f)),
nameLabel_(this),
component_(component),
comInterface_(),
apiInterface_(apiIf),
oldPos_(),
oldStack_(0),
oldInterfacePositions_(),
offPageConnector_(0),
graphicsData_(interfaceGraphics)
{
    Q_ASSERT(apiIf != 0);
    setType(ENDPOINT_TYPE_API);
    setTypeLocked(true);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::SWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem::SWInterfaceItem(QSharedPointer<Component> component, QSharedPointer<ComInterface> comIf,
                                 QSharedPointer<InterfaceGraphicsData> interfaceGraphics, QGraphicsItem *parent):
SWConnectionEndpoint(parent, QVector2D(1.0f, 0.0f)),
nameLabel_(this),
component_(component),
comInterface_(comIf),
apiInterface_(),
oldPos_(),
oldStack_(0),
oldInterfacePositions_(),
offPageConnector_(0),
graphicsData_(interfaceGraphics)
{
    Q_ASSERT(comIf != 0);
    setType(ENDPOINT_TYPE_COM);
    setTypeLocked(true);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::~SWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem::~SWInterfaceItem()
{
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::name()
//-----------------------------------------------------------------------------
QString SWInterfaceItem::name() const
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
        return nameLabel_.toPlainText();
    }
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void SWInterfaceItem::setName(const QString& name)
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
        nameLabel_.setPlainText(name);
    }

    graphicsData_->setName(name);

	updateInterface();
    endUpdateConnectionNames();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::getComInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterface> SWInterfaceItem::getComInterface() const
{
    return comInterface_;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::getApiInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterface> SWInterfaceItem::getApiInterface() const
{
    return apiInterface_;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::updateInterface()
//-----------------------------------------------------------------------------
void SWInterfaceItem::updateInterface()
{
    SWConnectionEndpoint::updateInterface();

    // Update the polygon shape based on the direction.
    int squareSize = GridSize;
    QPolygonF shape;

    if (isApi())
    {
        if (apiInterface_->getDependencyDirection() == DEPENDENCY_PROVIDER)
        {
            /*  ||
             *  \/
             */
            shape << QPointF(-squareSize/2, squareSize / 2)
                  << QPointF(-squareSize/2, -squareSize)
                  << QPointF(squareSize/2, -squareSize)
                  << QPointF(squareSize/2, squareSize / 2)
                  << QPointF(0, squareSize);
        }
        else
        {
            /*  ||
             *  /\
             */
            shape << QPointF(-squareSize/2, squareSize)
                  << QPointF(-squareSize/2, -squareSize)
                  << QPointF(squareSize/2, -squareSize)
                  << QPointF(squareSize/2, squareSize)
                  << QPointF(0, squareSize / 2);
        }
    }
    else if (isCom())
    {
        if (comInterface_->getDirection() == DirectionTypes::IN)
        {
            /*  /\
             *  ||
             */
            shape << QPointF(-squareSize/2, squareSize)
                << QPointF(-squareSize/2, -squareSize / 2)
                << QPointF(0, -squareSize)
                << QPointF(squareSize/2, -squareSize / 2)
                << QPointF(squareSize/2, squareSize);
        }
        else if (comInterface_->getDirection() == DirectionTypes::OUT)
        {
            /*  ||
             *  \/
             */
            shape << QPointF(-squareSize/2, squareSize / 2)
                << QPointF(-squareSize/2, -squareSize)
                << QPointF(squareSize/2, -squareSize)
                << QPointF(squareSize/2, squareSize / 2)
                << QPointF(0, squareSize);
        }
        else
        {
            /*  /\
             *  ||
             *  \/
             */
            shape << QPointF(-squareSize/2, squareSize / 2)
                << QPointF(-squareSize/2, -squareSize / 2)
                << QPointF(0, -squareSize)
                << QPointF(squareSize/2, -squareSize / 2)
                << QPointF(squareSize/2, squareSize / 2)
                << QPointF(0, squareSize);
        }
    }
    else
    {
        /*  /\
         *  ||
         *  \/
         */
        shape << QPointF(-squareSize/2, squareSize / 2)
            << QPointF(-squareSize/2, -squareSize / 2)
            << QPointF(0, -squareSize)
            << QPointF(squareSize/2, -squareSize / 2)
            << QPointF(squareSize/2, squareSize / 2)
            << QPointF(0, squareSize);
    }

    setPolygon(shape);

    // Update the name label.
    nameLabel_.setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + name() + "</div>");

	setLabelPosition();

    offPageConnector_->updateInterface();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::isHierachical()
//-----------------------------------------------------------------------------
bool SWInterfaceItem::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::onConnect()
//-----------------------------------------------------------------------------
bool SWInterfaceItem::onConnect(ConnectionEndpoint const* other)
{
    // If the port is undefined, try to copy the configuration from the other end point.
    if (!isInvalid() && getType() == ENDPOINT_TYPE_UNDEFINED)
    {
        if (other->getType() == ENDPOINT_TYPE_API)
        {
            // Determine the name for the interface.
            QString name = other->getApiInterface()->name();
            unsigned int index = 0;

            while (component_->getApiInterface(name) != 0)
            {
                ++index;
                name = other->getApiInterface()->name() + "_" + QString::number(index);
            }

            apiInterface_ = QSharedPointer<ApiInterface>(new ApiInterface());
            apiInterface_->setName(SWInterfaceItem::name());
            apiInterface_->setApiType(other->getApiInterface()->getApiType());
            apiInterface_->setDependencyDirection(other->getApiInterface()->getDependencyDirection());
            
            QList<QSharedPointer<ApiInterface> > componentApis = getOwnerComponent()->getApiInterfaces();
            componentApis.append(apiInterface_);
            getOwnerComponent()->setApiInterfaces(componentApis);
        }
        else if (other->getType() == ENDPOINT_TYPE_COM)
        {
            // Determine the name for the interface.
            QString name = other->getComInterface()->name();
            unsigned int index = 0;

            while (component_->getComInterface(name) != 0)
            {
                ++index;
                name = other->getApiInterface()->name() + "_" + QString::number(index);
            }

            comInterface_ = QSharedPointer<ComInterface>(new ComInterface());
            comInterface_->setName(SWInterfaceItem::name());
            comInterface_->setComType(other->getComInterface()->getComType());
            comInterface_->setTransferType(other->getComInterface()->getTransferType());
            comInterface_->setDirection(other->getComInterface()->getDirection());

            QList<QSharedPointer<ComInterface> > componentComs = getOwnerComponent()->getComInterfaces();
            componentComs.append(comInterface_);
            getOwnerComponent()->setComInterfaces(componentComs);
        }

        setType(other->getType());
        updateInterface();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::onDisconnect()
//-----------------------------------------------------------------------------
void SWInterfaceItem::onDisconnect(ConnectionEndpoint const*)
{
    // Undefine the interface if it not typed.
    if (!isTypeLocked() && !isConnected())
    {
        setTypeDefinition(VLNV());
        updateInterface();
    }

    if (getType() == ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED)
    {
        if (apiInterface_)
        {
            QList<QSharedPointer<ApiInterface> > componentApis = getOwnerComponent()->getApiInterfaces();
            componentApis.removeAll(apiInterface_);
            getOwnerComponent()->setApiInterfaces(componentApis);
        }
        else if (comInterface_)
        {
            QList<QSharedPointer<ComInterface> > componentComs = getOwnerComponent()->getComInterfaces();
            componentComs.removeAll(comInterface_);
            getOwnerComponent()->setComInterfaces(componentComs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::isConnectionValid()
//-----------------------------------------------------------------------------
bool SWInterfaceItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!SWConnectionEndpoint::isConnectionValid(other))
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

            return (apiIf1->getDependencyDirection() == apiIf2->getDependencyDirection());
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

            return (comIf1->getDirection() == comIf2->getDirection());
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
// Function: SWInterfaceItem::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* SWInterfaceItem::encompassingComp() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> SWInterfaceItem::getOwnerComponent() const
{
	return component_;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::getInterfaceGraphicsData()
//-----------------------------------------------------------------------------
QSharedPointer<InterfaceGraphicsData> SWInterfaceItem::getInterfaceGraphicsData() const
{
    return graphicsData_;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::itemChange()
//-----------------------------------------------------------------------------
QVariant SWInterfaceItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        return snapPointToGrid(value.toPointF());
    }
    else if (change == ItemRotationHasChanged)
    {
        nameLabel_.setRotation(-rotation());

        graphicsData_->setDirection(getDirection());
    }
    else if (change == ItemScenePositionHasChanged)
    {
        graphicsData_->setPosition(value.toPointF());

        foreach (GraphicsConnection* conn, getConnections())
        {
            conn->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool SWInterfaceItem::isDirectionFixed() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void SWInterfaceItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the System is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    SWConnectionEndpoint::mouseMoveEvent(event);

    setPos(parentItem()->mapFromScene(oldStack_->mapStackToScene(pos())));

    IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());
    Q_ASSERT(stack != 0);
    stack->onMoveItem(this);
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::mousePressEvent()
//-----------------------------------------------------------------------------
void SWInterfaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    SWConnectionEndpoint::mousePressEvent(event);    

    oldPos_ = scenePos();
    oldStack_ = dynamic_cast<IGraphicsItemStack*>(parentItem());
    Q_ASSERT(oldStack_ != 0);

    // Save the positions of the other interfaces.
    /*foreach (GraphicsColumn* column, static_cast<GraphicsColumn*>(oldStack_)->getLayout().getColumns())
    {*/
        foreach (QGraphicsItem* item, scene()->items()) //column->childItems())
        {
            if (item->type() == SWInterfaceItem::Type)
            {
                SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);
                oldInterfacePositions_.insert(interface, interface->scenePos());
            }
        }
    //}

    // Begin the position update for all connections.
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
// Function: SWInterfaceItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SWInterfaceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    SWConnectionEndpoint::mouseReleaseEvent(event);

    if (oldStack_ != 0)
    {
        IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(stack != 0);
        stack->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        // Check if the interface position was really changed.
        if (oldPos_ != scenePos())
        {
            cmd = QSharedPointer<QUndoCommand>(new SWInterfaceMoveCommand(this, oldPos_, oldStack_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<SWInterfaceItem*, QPointF>::iterator cur = oldInterfacePositions_.begin();

        while (cur != oldInterfacePositions_.end())
        {
            if (cur.key()->scenePos() != cur.value() && cur.key() != this)
            {
                new SWInterfaceMoveCommand(cur.key(), cur.value(),
                    dynamic_cast<IGraphicsItemStack*>(cur.key()->parentItem()), cmd.data());
            }

            ++cur;
        }

        oldInterfacePositions_.clear();

        // End the position update for all connections.
        foreach (QGraphicsItem *item, scene()->items())
        {
            GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

            if (conn != 0)
            {
                conn->endUpdatePosition(cmd.data());
            }
        }

        // Add the undo command to the edit stack only if it has changes.
        if (cmd->childCount() > 0 || oldPos_ != scenePos())
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider()->addCommand(cmd);

            cmd->redo();
        }

        oldStack_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::description()
//-----------------------------------------------------------------------------
QString SWInterfaceItem::description() const
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
// Function: SWInterfaceItem::setDescription()
//-----------------------------------------------------------------------------
void SWInterfaceItem::setDescription(QString const& description)
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
// Function: SWInterfaceItem::initialize()
//-----------------------------------------------------------------------------
void SWInterfaceItem::initialize()
{
    QFont font = nameLabel_.font();
    font.setPointSize(8);
    nameLabel_.setFont(font);
    nameLabel_.setFlag(ItemStacksBehindParent);
    nameLabel_.setRotation(-rotation());

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
    nameLabel_.setGraphicsEffect(shadow);

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    // Create the off-page connector.
    offPageConnector_ = new SWOffPageConnectorItem(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);

    graphicsData_->setName(name());

    if (graphicsData_->hasPosition())
    {
        setPos(graphicsData_->getPosition());
    }
    if (graphicsData_->hasDirection())
    {
        setDirection(graphicsData_->getDirection());
    }
    else
    {
        graphicsData_->setDirection(getDirection());
    }

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::onBeginConnect()
//-----------------------------------------------------------------------------
void SWInterfaceItem::onBeginConnect()
{
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::onEndConnect()
//-----------------------------------------------------------------------------
void SWInterfaceItem::onEndConnect()
{
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::setTypeDefinition()
//-----------------------------------------------------------------------------
void SWInterfaceItem::setTypeDefinition(VLNV const& type)
{
    // Disconnect existing connections before setting the type.
    foreach (GraphicsConnection* conn, getConnections())
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
            getOwnerComponent()->getVendorExtensions()->append(apiInterface_);

            setType(ENDPOINT_TYPE_API);
            setTypeLocked(true);
        }
        else if (type.getType() == VLNV::COMDEFINITION)
        {
            comInterface_ = QSharedPointer<ComInterface>(new ComInterface());
            comInterface_->setName(nameLabel_.toPlainText());
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
            getOwnerComponent()->getVendorExtensions()->removeOne(apiInterface_);
            apiInterface_.clear();
        }

        if (comInterface_ != 0)
        {
            getOwnerComponent()->getVendorExtensions()->removeOne(comInterface_);
            comInterface_.clear();
        }

        setType(ENDPOINT_TYPE_UNDEFINED);
        setTypeLocked(false);
    }

    updateInterface();

    if (getType() != ENDPOINT_TYPE_UNDEFINED)
    {
        // Undefined endpoints of the connections can now be defined.
        foreach (GraphicsConnection* conn, getConnections())
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
VLNV SWInterfaceItem::getTypeDefinition() const
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
// Function: setDirection()
//-----------------------------------------------------------------------------
void SWInterfaceItem::setDirection(QVector2D const& dir)
{
    SWConnectionEndpoint::setDirection(dir);

    graphicsData_->setDirection(dir);

	setLabelPosition();	
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::isExclusive()
//-----------------------------------------------------------------------------
bool SWInterfaceItem::isExclusive() const
{
    if (getType() == ENDPOINT_TYPE_COM)
    {
        // All COM interfaces are exclusive.
        return true;
    }
    else if (getType() == ENDPOINT_TYPE_API)
    {
        // Provider is exclusive.
        return (getApiInterface()->getDependencyDirection() == DEPENDENCY_PROVIDER);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* SWInterfaceItem::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::define()
//-----------------------------------------------------------------------------
void SWInterfaceItem::define(QSharedPointer<ApiInterface> apiIf)
{
    apiInterface_ = apiIf;
    getOwnerComponent()->getVendorExtensions()->append(apiInterface_);

    setType(ENDPOINT_TYPE_API);
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::define()
//-----------------------------------------------------------------------------
void SWInterfaceItem::define(QSharedPointer<ComInterface> comIf)
{
    comInterface_ = comIf;
    getOwnerComponent()->getVendorExtensions()->append(comInterface_);

    setType(ENDPOINT_TYPE_COM);
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::undefine()
//-----------------------------------------------------------------------------
void SWInterfaceItem::undefine()
{
    if (apiInterface_ != 0)
    {
        getOwnerComponent()->getVendorExtensions()->removeOne(apiInterface_);
        apiInterface_.clear();
    }

    if (comInterface_ != 0)
    {
        getOwnerComponent()->getVendorExtensions()->removeOne(comInterface_);
        comInterface_.clear();
    }

    setType(ENDPOINT_TYPE_UNDEFINED);
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::setLabelPosition()
//-----------------------------------------------------------------------------
void SWInterfaceItem::setLabelPosition()
{
	qreal nameWidth = nameLabel_.boundingRect().width();

	// Check if the port is directed to the left.
	if (getDirection().x() < 0)
	{
		nameLabel_.setPos(0, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
	}
	// Otherwise the port is directed to the right.
	else
	{
		nameLabel_.setPos(0, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
	}
}