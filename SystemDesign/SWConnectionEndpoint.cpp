//-----------------------------------------------------------------------------
// File: SWConnectionEndpoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.5.2012
//
// Description:
// Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------

#include "SWConnectionEndpoint.h"

#include <QPen>

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint()
//-----------------------------------------------------------------------------
SWConnectionEndpoint::SWConnectionEndpoint(QGraphicsItem* parent,
                                           QVector2D const& dir)
    : QGraphicsPolygonItem(parent),
      type_(ENDPOINT_TYPE_UNDEFINED),
      connections_(),
      dir_()
{
    setDirection(dir);
}

//-----------------------------------------------------------------------------
// Function: ~SWConnectionEndpoint()
//-----------------------------------------------------------------------------
SWConnectionEndpoint::~SWConnectionEndpoint()
{
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: setHighlight()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::setHighlight(HighlightMode mode)
{
    switch (mode)
    {
    case HIGHLIGHT_OFF:
        {
            setPen(QPen(Qt::black, 1));
            break;
        }

    case HIGHLIGHT_ALLOWED:
        {
            setPen(QPen(Qt::blue, 2));
            break;
        }

    case HIGHLIGHT_HOVER:
        {
            setPen(QPen(Qt::red, 2));
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addConnection()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::addConnection(SWConnection* connection)
{
    connections_.append(connection);
}

//-----------------------------------------------------------------------------
// Function: removeConnection()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::removeConnection(SWConnection* connection)
{
    connections_.removeAll(connection);
}

//-----------------------------------------------------------------------------
// Function: getConnections()
//-----------------------------------------------------------------------------
QList<SWConnection*> const& SWConnectionEndpoint::getConnections() const
{
    return connections_;
}

//-----------------------------------------------------------------------------
// Function: isConnected()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::isConnected() const
{
    return (!connections_.empty());
}

//-----------------------------------------------------------------------------
// Function: setDirection()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::setDirection(QVector2D const& dir)
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
// Function: isDirectionFixed()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::isDirectionFixed() const
{
    // The end point direction is not fixed by default.
    return false;
}

//-----------------------------------------------------------------------------
// Function: getDirection()
//-----------------------------------------------------------------------------
QVector2D const& SWConnectionEndpoint::getDirection() const
{
    return dir_;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::setType()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::setType(EndpointType type)
{
    type_ = type;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::getType()
//-----------------------------------------------------------------------------
SWConnectionEndpoint::EndpointType SWConnectionEndpoint::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::isCom()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::isCom() const
{
    return (type_ == ENDPOINT_TYPE_COM);
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::isApi()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::isApi() const
{
    return (type_ == ENDPOINT_TYPE_API);
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::updateInterface()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::updateInterface()
{
    switch (type_)
    {
    case ENDPOINT_TYPE_UNDEFINED:
        {
            setBrush(QBrush(Qt::black));
            break;
        }

    case ENDPOINT_TYPE_API:
        {
            setBrush(QBrush(QColor(0xcb,0xfd,0xfd)));
            break;
        }

    case ENDPOINT_TYPE_COM:
        {
            setBrush(QBrush(QColor(0x32,0x99,0x64)));
            break;
        }

    default:
        {
            Q_ASSERT(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::canConnect()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::canConnect(SWConnectionEndpoint const* other) const
{
    bool fullyDefined = getType() != ENDPOINT_TYPE_UNDEFINED && other->getType() != ENDPOINT_TYPE_UNDEFINED;

    // Check if the types do not match.
    if (fullyDefined && getType() != other->getType())
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
