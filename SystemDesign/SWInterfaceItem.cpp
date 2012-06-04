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

#include "SystemColumn.h"
#include "SWComponentItem.h"
#include "SWConnection.h"
#include "HWMappingItem.h"
#include "SystemMoveCommands.h"

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>
#include <common/DesignDiagram.h>

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
// Function: SWInterfaceItem::SWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem::SWInterfaceItem(QSharedPointer<Component> component,
                                 QString const& name, QGraphicsItem *parent)
    : SWConnectionEndpoint(parent, QVector2D(-1.0f, 0.0f)),
      nameLabel_(name, this),
      component_(component),
      apiInterface_(),
      comInterface_(),
      temp_(true),
      oldPos_(),
      oldStack_(0),
      oldInterfacePositions_()
{
    setType(ENDPOINT_TYPE_UNDEFINED);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::SWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem::SWInterfaceItem(QSharedPointer<Component> component,
                                 QSharedPointer<ApiInterface> apiIf, QGraphicsItem *parent)
    : SWConnectionEndpoint(parent, QVector2D(-1.0f, 0.0f)),
      nameLabel_(this),
      component_(component),
      apiInterface_(apiIf),
      comInterface_(),
      temp_(false),
      oldPos_(),
      oldInterfacePositions_()
{
    Q_ASSERT(apiIf != 0);
    setType(ENDPOINT_TYPE_API);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::SWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem::SWInterfaceItem(QSharedPointer<Component> component, 
                                 QSharedPointer<ComInterface> comIf, QGraphicsItem *parent)
    : SWConnectionEndpoint(parent, QVector2D(-1.0f, 0.0f)),
      nameLabel_(this),
      component_(component),
      apiInterface_(),
      comInterface_(comIf),
      temp_(false),
      oldPos_(),
      oldInterfacePositions_()
{
    Q_ASSERT(comIf != 0);
    setType(ENDPOINT_TYPE_COM);
    initialize();
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::~SWInterfaceItem()
//-----------------------------------------------------------------------------
SWInterfaceItem::~SWInterfaceItem()
{
}

//-----------------------------------------------------------------------------
// Function: setTemporary()
//-----------------------------------------------------------------------------
void SWInterfaceItem::setTemporary(bool temp)
{
    temp_ = temp;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::name()
//-----------------------------------------------------------------------------
QString SWInterfaceItem::name() const
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
void SWInterfaceItem::setName(const QString& name)
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
        switch (comInterface_->getDirection())
        {
        case General::IN:
            {
                /*  /\
                 *  ||
                 */
                shape << QPointF(-squareSize/2, squareSize)
                      << QPointF(-squareSize/2, -squareSize / 2)
                      << QPointF(0, -squareSize)
                      << QPointF(squareSize/2, -squareSize / 2)
                      << QPointF(squareSize/2, squareSize);
                break;
            }

        case General::OUT:
            {
                /*  ||
                 *  \/
                 */
                shape << QPointF(-squareSize/2, squareSize / 2)
                      << QPointF(-squareSize/2, -squareSize)
                      << QPointF(squareSize/2, -squareSize)
                      << QPointF(squareSize/2, squareSize / 2)
                      << QPointF(0, squareSize);
                break;
            }

        case General::INOUT:
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
                break;
            }
        }
    }
    else
    {
        /*  /\
         *  ||
         */
        shape << QPointF(-squareSize/2, squareSize / 2)
              << QPointF(-squareSize/2, -squareSize)
              << QPointF(squareSize/2, -squareSize)
              << QPointF(squareSize/2, squareSize / 2)
              << QPointF(0, squareSize);
    }

    setPolygon(shape);

    // Update the name label.
    nameLabel_.setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + name() + "</div>");

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
bool SWInterfaceItem::onConnect(SWConnectionEndpoint const* other)
{
    // If the port is undefined, try to copy the configuration from the other end point.
    if (getType() == ENDPOINT_TYPE_UNDEFINED)
    {
        if (other->getType() == ENDPOINT_TYPE_API)
        {
            // Determine the name for the interface.
            QString name = other->getApiInterface()->getName();
            unsigned int index = 0;

            while (component_->getApiInterface(name) != 0)
            {
                ++index;
                name = other->getApiInterface()->getName() + "_" + QString::number(index);
            }

            apiInterface_ = QSharedPointer<ApiInterface>(new ApiInterface());
            apiInterface_->setName(name);
            apiInterface_->setApiType(other->getApiInterface()->getApiType());
            apiInterface_->setDependencyDirection(other->getApiInterface()->getDependencyDirection());
            
            getOwnerComponent()->addApiInterface(apiInterface_);
        }
        else if (other->getType() == ENDPOINT_TYPE_COM)
        {
            // Determine the name for the interface.
            QString name = other->getComInterface()->getName();
            unsigned int index = 0;

            while (component_->getComInterface(name) != 0)
            {
                ++index;
                name = other->getApiInterface()->getName() + "_" + QString::number(index);
            }

            comInterface_ = QSharedPointer<ComInterface>(new ComInterface());
            comInterface_->setName(name);
            comInterface_->setComType(other->getComInterface()->getComType());
            comInterface_->setDataType(other->getComInterface()->getDataType());
            comInterface_->setDirection(other->getComInterface()->getDirection());

            getOwnerComponent()->addComInterface(comInterface_);
        }

        setType(other->getType());
        updateInterface();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::onDisconnect()
//-----------------------------------------------------------------------------
void SWInterfaceItem::onDisconnect(SWConnectionEndpoint const*)
{
    // Undefine the interface if it is temporary.
    if (temp_ && !isConnected())
    {
        setTypeDefinition(VLNV());
        updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceItem::canConnect()
//-----------------------------------------------------------------------------
bool SWInterfaceItem::canConnect(SWConnectionEndpoint const* other) const
{
    if (!SWConnectionEndpoint::canConnect(other))
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

            return (apiIf1->getDependencyDirection() == apiIf2->getDependencyDirection());
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
// Function: SWInterfaceItem::itemChange()
//-----------------------------------------------------------------------------
QVariant SWInterfaceItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    switch (change)
    {
    case ItemPositionChange:
        {
            return snapPointToGrid(value.toPointF());
        }

    case ItemPositionHasChanged:
        {
            if (!parentItem())
                break;

            emit contentChanged();
            break;
        }

    case ItemScenePositionHasChanged:
        {
            foreach (SWConnection* conn, getConnections())
            {
                conn->updatePosition();
            }

            break;
        }

    default:
        break;
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
    foreach (GraphicsColumn* column, static_cast<GraphicsColumn*>(oldStack_)->getLayout().getColumns())
    {
        foreach (QGraphicsItem* item, column->childItems())
        {
            if (item->type() == SWInterfaceItem::Type)
            {
                SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);
                oldInterfacePositions_.insert(interface, interface->scenePos());
            }
        }
    }

    // Begin the position update for the connections.
    foreach (SWConnection* conn, getConnections())
    {
        conn->beginUpdatePosition();
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
            cmd = QSharedPointer<QUndoCommand>(new SystemItemMoveCommand(this, oldPos_, oldStack_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<SWInterfaceItem*, QPointF>::iterator cur = oldInterfacePositions_.begin();

        while (cur != oldInterfacePositions_.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                QUndoCommand* childCmd = new SystemItemMoveCommand(cur.key(), cur.value(),
                                                                   dynamic_cast<IGraphicsItemStack*>(cur.key()->parentItem()),
                                                                   cmd.data());
            }

            ++cur;
        }

        oldInterfacePositions_.clear();

        // End the position update of the connections.
        foreach (SWConnection* conn, getConnections())
        {
            conn->endUpdatePosition(cmd.data());
        }

        // Add the undo command to the edit stack only if it has changes.
        if (cmd->childCount() > 0 || oldPos_ != scenePos())
        {

            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
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
    foreach (SWConnection* conn, getConnections())
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
        nameLabel_.setPlainText("");
    }

    updateInterface();

    if (getType() != ENDPOINT_TYPE_UNDEFINED)
    {
        // Undefined endpoints of the connections can now be defined.
        foreach (SWConnection* conn, getConnections())
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

    // Update the position of the name label based on the direction.
    qreal nameWidth = nameLabel_.boundingRect().width();

    // Check if the interface is directed to the left.
    if (dir.x() < 0)
    {
        nameLabel_.setPos(0, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the interface is directed to the right.
    else
    {
        nameLabel_.setPos(0, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }
}