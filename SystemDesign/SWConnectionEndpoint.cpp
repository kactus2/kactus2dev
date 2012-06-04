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
    : ConnectionEndpoint(parent),
      type_(ENDPOINT_TYPE_UNDEFINED),
      connections_()
{
    setDirection(dir);
}

//-----------------------------------------------------------------------------
// Function: ~SWConnectionEndpoint()
//-----------------------------------------------------------------------------
SWConnectionEndpoint::~SWConnectionEndpoint()
{
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

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::setSelectionHighlight()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::setSelectionHighlight(bool on)
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

