//-----------------------------------------------------------------------------
// File: ConnectionEndpoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 4.6.2012
//
// Description:
// Base class for connection endpoints (both SW and HW).
//-----------------------------------------------------------------------------

#include "ConnectionEndpoint.h"

#include "GraphicsConnection.h"

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>

#include <common/KactusColors.h>

#include <QGraphicsScene>
#include <QPen>
#include "editors/HWDesign/OffPageConnectorItem.h"

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::ConnectionEndpoint()
//-----------------------------------------------------------------------------
ConnectionEndpoint::ConnectionEndpoint(QGraphicsItem* parent /*= 0*/) :
QGraphicsPolygonItem(parent)
{
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::~ConnectionEndpoint()
//-----------------------------------------------------------------------------
ConnectionEndpoint::~ConnectionEndpoint()
{
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::setHighlight()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::setHighlight(HighlightMode mode)
{
    if (mode == HIGHLIGHT_OFF)
    {
        if (isInvalid())
        {
            setPen(QPen(Qt::red, 0));
        }
        else
        {
            setPen(QPen(Qt::black, 0));
        }
    }
    else if (mode == HIGHLIGHT_ALLOWED)
    {
        setPen(QPen(KactusColors::DIAGRAM_ALLOWED_INTERFACE, 2));
    }
    else if (mode == HIGHLIGHT_HOVER)
    {
        setPen(QPen(KactusColors::DIAGRAM_SELECTION, 2));
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::setSelectionHighlight()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::setSelectionHighlight(bool on)
{
    if (on)
    {
        setHighlight(HIGHLIGHT_HOVER);
    }
    else
    {
        setHighlight(HIGHLIGHT_OFF);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::addConnection()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::addConnection(GraphicsConnection* connection)
{
     connections_.append(connection);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::removeConnection()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::removeConnection(GraphicsConnection* connection)
{
    connections_.removeAll(connection);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getConnections()
//-----------------------------------------------------------------------------
QList<GraphicsConnection*> const& ConnectionEndpoint::getConnections() const
{
    return connections_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isConnected()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isConnected() const
{
    return (!connections_.empty());
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::beginUpdateConnectionPositions()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::beginUpdateConnectionPositions() const
{
    for (QGraphicsItem* item : scene()->items())
    {
        auto conn = dynamic_cast<GraphicsConnection*>(item);
        if (conn != nullptr)
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::endUpdateConnectionPositions()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::endUpdateConnectionPositions(QUndoCommand* parentCommand) const
{
    for (QGraphicsItem* item : scene()->items())
    {
        auto conn = dynamic_cast<GraphicsConnection*>(item);
        if (conn != nullptr)
        {
            conn->endUpdatePosition(parentCommand);
        }
    }
}
//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::updateConnectionPositions()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::updateConnectionPositions() const 
{
    for (GraphicsConnection* connection : getConnections())
    {
        connection->updatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::onDisconnect()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::onDisconnect()
{
    // Intentionally empty.
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::canConnect()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::canConnect(ConnectionEndpoint const* other) const
{
    if (isConnected() && isExclusive())
    {
        return false;
    }

    return isConnectionValid(other);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::setDirection()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::setDirection(QVector2D const& dir)
{
    // Determine the rotation based on the new direction.
    if (qFuzzyCompare(dir, QVector2D(0.0f, -1.0f)))
    {
        setRotation(0.0);
    }
    else if (qFuzzyCompare(dir, QVector2D(0.0f, 1.0f)))
    {
        setRotation(180.0);
    }
    else if (qFuzzyCompare(dir, QVector2D(1.0f, 0.0f)))
    {
        setRotation(90.0);
    }
    else if (qFuzzyCompare(dir, QVector2D(-1.0f, 0.0f)))
    {
        setRotation(-90.0);
    }

    dir_ = dir;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getDirection()
//-----------------------------------------------------------------------------
QVector2D ConnectionEndpoint::getDirection() const
{
    return dir_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::updateInterface()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::updateInterface()
{
    if (isInvalid())
    {
        setPen(QPen(Qt::red, 0));
    }
    else
    {
        setPen(QPen(Qt::black, 0));
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isDirectionFixed()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isDirectionFixed() const noexcept
{
    // The endpoint direction is not fixed by default.
    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> ConnectionEndpoint::getBusInterface() const
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getComInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterface> ConnectionEndpoint::getComInterface() const
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getApiInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterface> ConnectionEndpoint::getApiInterface() const
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> ConnectionEndpoint::getPort() const
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isBus()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isBus() const noexcept
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isAdHoc()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isAdHoc() const noexcept
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isCom()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isCom() const noexcept
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isApi()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isApi() const noexcept
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getType()
//-----------------------------------------------------------------------------
ConnectionEndpoint::EndpointType ConnectionEndpoint::getType() const noexcept
{
    return ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* ConnectionEndpoint::getOffPageConnector() const
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getParentConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* ConnectionEndpoint::getParentConnector() const
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::setTemporary()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::setTemporary(bool temp)
{
    temporary_ = temp;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isTemporary()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isTemporary() const noexcept
{
    return temporary_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isInvalid()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isInvalid() const
{
    return (temporary_ && getOwnerComponent()->getVlnv().isValid());
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::setTypeLocked()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::setTypeLocked(bool locked)
{
    typeLocked_ = locked;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isTypeLocked()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isTypeLocked() const
{
    return typeLocked_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isConnectionValid()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isConnectionValid(ConnectionEndpoint const* other) const
{
    // Invalid endpoints cannot be connected to.
    // Two hierarchical endpoints cannot be connected.
    if (isInvalid() || (!isAdHoc() && isHierarchical() && other->isHierarchical()))
    {
        return false;
    }

    // Prevent double connections
    ConnectionEndpoint* thisOffPage = this->getOffPageConnector();
    ConnectionEndpoint* otherOffPage = other->getOffPageConnector();
    ConnectionEndpoint* thisParent = this->getParentConnector();
    ConnectionEndpoint* otherParent = other->getParentConnector();

    // All possible combinations are handled below.
    if (!other->isOffPage())
    {
        if (!this->isOffPage())
        {
            return !(connectionToPointExists(other) ||
                (thisOffPage && otherOffPage && thisOffPage->connectionToPointExists(otherOffPage)));
        }
        else 
        {
            return !((otherOffPage && connectionToPointExists(otherOffPage) ||
                thisParent->connectionToPointExists(other)));
        }
    }
    else 
    {
        if (!this->isOffPage())
        {
            return !(connectionToPointExists(otherParent) ||
                (thisOffPage && thisOffPage->connectionToPointExists(other)));
        }
        else
        {
            return !(connectionToPointExists(other) ||
                (thisParent->connectionToPointExists(otherParent)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::revalidateConnectionToEndpoint()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isExistingConnectionValid(ConnectionEndpoint const* other) const
{
	if (isInvalid() || (!isAdHoc() && isHierarchical() && other->isHierarchical()))
	{
		return false;
	}

	return std::any_of(other->getConnections().cbegin(), other->getConnections().cend(),
		[this](GraphicsConnection const* connection)
		{
			return connection->endpoint1() == this || connection->endpoint2() == this;
		});
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::revalidateConnections()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::revalidateConnections()
{
    for (GraphicsConnection* conn : getConnections())
    {
		conn->reValidate();
    }

    if (getOffPageConnector() != nullptr)
    {
        for (GraphicsConnection* conn : getOffPageConnector()->getConnections())
        {
			conn->reValidate();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::beginUpdateConnectionNames()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::beginUpdateConnectionNames()
{
    Q_ASSERT(connUpdateList_.isEmpty());

    for (GraphicsConnection* conn : getConnections())
    {
        if (conn->hasDefaultName())
        {
            connUpdateList_.append(conn);
        }
    }

    if (getOffPageConnector() != nullptr)
    {
        for (GraphicsConnection* conn : getOffPageConnector()->getConnections())
        {
            connUpdateList_.append(conn);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::endUpdateConnectionNames()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::endUpdateConnectionNames()
{
    for (GraphicsConnection* conn: connUpdateList_)
    {
        conn->setName(conn->createDefaultName());
    }

    connUpdateList_.clear();
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::connectionToPointExists()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::connectionToPointExists(ConnectionEndpoint const* other) const
{
    return std::any_of(
        other->getConnections().cbegin(),
        other->getConnections().cend(),
        [this](GraphicsConnection const* connection)
        {
            return connection->endpoint1() == this || connection->endpoint2() == this;
        });
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode ConnectionEndpoint::getInterfaceMode() const
{
    return General::INTERFACE_MODE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getNameLength()
//-----------------------------------------------------------------------------
qreal ConnectionEndpoint::getNameLength()
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::shortenNameLabel()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::shortenNameLabel( qreal /*width */)
{
    // Intentionally empty.
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isOffPage()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isOffPage() const
{
    return type() == OffPageConnectorItem::Type;
}