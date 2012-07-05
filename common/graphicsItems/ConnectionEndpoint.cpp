//-----------------------------------------------------------------------------
// File: ConnectionEndpoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.6.2012
//
// Description:
// Base class for connection endpoints (both SW and HW).
//-----------------------------------------------------------------------------

#include "ConnectionEndpoint.h"

#include <models/businterface.h>
#include <models/ApiInterface.h>
#include <models/ComInterface.h>

#include <common/KactusColors.h>

#include <QPen>

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::ConnectionEndpoint()
//-----------------------------------------------------------------------------
ConnectionEndpoint::ConnectionEndpoint(QGraphicsItem* parent, bool temporary)
    : QGraphicsPolygonItem(parent),
       dir_(),
       type_(ENDPOINT_TYPE_UNDEFINED),
       connections_(),
       temporary_(temporary),
       typeLocked_(true)
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
    switch (mode)
    {
    case HIGHLIGHT_OFF:
        {
            if (isInvalid())
            {
                setPen(QPen(Qt::red, 1));
            }
            else
            {
                setPen(QPen(Qt::black, 1));
            }
            break;
        }

    case HIGHLIGHT_ALLOWED:
        {
            setPen(QPen(KactusColors::DIAGRAM_ALLOWED_INTERFACE, 2));
            break;
        }

    case HIGHLIGHT_HOVER:
        {
            setPen(QPen(KactusColors::DIAGRAM_SELECTION, 2));
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::setHighlight()
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
QVector2D const& ConnectionEndpoint::getDirection() const
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
        setPen(QPen(Qt::red, 1));
    }
    else
    {
        setPen(QPen(Qt::black, 1));
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isDirectionFixed()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isDirectionFixed() const
{
    // The endpoint direction is not fixed by default.
    return false;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> ConnectionEndpoint::getBusInterface() const
{
    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getComInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterface> ConnectionEndpoint::getComInterface() const
{
    return QSharedPointer<ComInterface>();
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getApiInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterface> ConnectionEndpoint::getApiInterface() const
{
    return QSharedPointer<ApiInterface>();
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getPort()
//-----------------------------------------------------------------------------
Port* ConnectionEndpoint::getPort() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isBus()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isBus() const
{
    return (type_ == ENDPOINT_TYPE_BUS);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isAdHoc()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isAdHoc() const
{
    return (type_ == ENDPOINT_TYPE_ADHOC);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isCom()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isCom() const
{
    return (type_ == ENDPOINT_TYPE_COM);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isApi()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isApi() const
{
    return (type_ == ENDPOINT_TYPE_API);
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::setType()
//-----------------------------------------------------------------------------
void ConnectionEndpoint::setType(EndpointType type)
{
    type_ = type;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getType()
//-----------------------------------------------------------------------------
ConnectionEndpoint::EndpointType ConnectionEndpoint::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* ConnectionEndpoint::getOffPageConnector()
{
    return 0;
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
bool ConnectionEndpoint::isTemporary() const
{
    return temporary_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionEndpoint::isInvalid()
//-----------------------------------------------------------------------------
bool ConnectionEndpoint::isInvalid() const
{
    return (temporary_ && getOwnerComponent()->getVlnv()->isValid());
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
    if (isInvalid())
    {
        return false;
    }

    // Two hierarchical endpoints cannot be connected.
    if (isHierarchical() && other->isHierarchical())
    {
        return false;
    }

    return true;
}
