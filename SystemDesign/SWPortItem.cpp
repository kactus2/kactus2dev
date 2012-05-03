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
#include "../EndpointDesign/SystemMoveCommands.h"

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
// Function: SWPortItem::SWPortItem()
//-----------------------------------------------------------------------------
SWPortItem::SWPortItem(QSharedPointer<ApiInterface> apiIf, QGraphicsItem *parent)
    : SWConnectionEndpoint(parent),
      apiInterface_(apiIf),
      comInterface_(),
      temp_(false),
      oldPos_(),
      oldPortPositions_()
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
      apiInterface_(),
      comInterface_(comIf),
      temp_(false),
      oldPos_(),
      oldPortPositions_()
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
        return QString();
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
    nameLabel_->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + name() + "</div>");

    qreal nameWidth = nameLabel_->boundingRect().width();
    qreal nameHeight = nameLabel_->boundingRect().height();

    if (pos().x() < 0)
    {
        nameLabel_->setPos(nameHeight / 2, GridSize);
    }
    else
    {
        nameLabel_->setPos(-nameHeight / 2, GridSize + nameWidth);
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
bool SWPortItem::onConnect(SWConnectionEndpoint const* other)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::onDisconnect()
//-----------------------------------------------------------------------------
void SWPortItem::onDisconnect(SWConnectionEndpoint const*)
{
    // TODO:
    if (temp_)
    {
        updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::canConnect()
//-----------------------------------------------------------------------------
bool SWPortItem::canConnect(SWConnectionEndpoint const* other) const
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
SWComponentItem* SWPortItem::encompassingComp() const
{
    return static_cast<SWComponentItem*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: SWPortItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> SWPortItem::getOwnerComponent() const
{
	SWComponentItem* comp = encompassingComp();
	Q_ASSERT(comp);

	QSharedPointer<Component> compModel = comp->componentModel();
	Q_ASSERT(compModel);
	return compModel;
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

    case ItemPositionHasChanged:
        {
            if (!parentItem())
                break;

            qreal nameWidth = nameLabel_->boundingRect().width();
            qreal nameHeight = nameLabel_->boundingRect().height();

            QRectF parentRect = static_cast<SWComponentItem*>(parentItem())->rect();

            // Check if the port is directed to the left.
            if (pos().x() < 0)
            {
                setDirection(QVector2D(-1.0f, 0.0f));
                nameLabel_->setPos(nameHeight/2, GridSize);
            }
            // Otherwise the port is directed to the right.
            else
            {
                setDirection(QVector2D(1.0f, 0.0f));
                nameLabel_->setPos(-nameHeight/2, GridSize + nameWidth);
            }

            emit contentChanged();
            break;
        }

    case ItemScenePositionHasChanged:
        // Check if the updates are not disabled.
//         if (!static_cast<SWComponentItem*>(parentItem())->isConnectionUpdateDisabled())
//         {
//             // Update the connections.
//             foreach (SWConnection *connection, getConnections())
//             {
//                 connection->updatePosition();
//             }
//         }

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
    encompassingComp()->onMovePort(this);
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
//     foreach (SWConnection* conn, getConnections())
//     {
//         conn->beginUpdatePosition();
//     }
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
//     foreach (SWConnection* conn, getConnections())
//     {
//         conn->endUpdatePosition(cmd.data());
//     }
// 
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
    nameLabel_ = new QGraphicsTextItem("", this);
    QFont font = nameLabel_->font();
    font.setPointSize(8);
    nameLabel_->setFont(font);
    nameLabel_->setFlag(ItemIgnoresTransformations);
    nameLabel_->setFlag(ItemStacksBehindParent);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
    nameLabel_->setGraphicsEffect(shadow);

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    updateInterface();
}
