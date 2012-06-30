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
SWConnectionEndpoint::SWConnectionEndpoint(QGraphicsItem* parent, bool temporary,
                                           QVector2D const& dir)
    : ConnectionEndpoint(parent, temporary)
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
// Function: SWConnectionEndpoint::updateInterface()
//-----------------------------------------------------------------------------
void SWConnectionEndpoint::updateInterface()
{
    ConnectionEndpoint::updateInterface();

    if (isInvalid())
    {
        setBrush(QBrush(Qt::red));
        return;
    }

    switch (getType())
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
bool SWConnectionEndpoint::canConnect(ConnectionEndpoint const* other) const
{
    if (!ConnectionEndpoint::canConnect(other))
    {
        return false;
    }

    // Disallow connections to invalid endpoints.
    if (isInvalid())
    {
        return false;
    }

    bool fullyDefined = getType() != ENDPOINT_TYPE_UNDEFINED && other->getType() != ENDPOINT_TYPE_UNDEFINED;

    // Check if the types do not match.
    if (fullyDefined && getType() != other->getType())
    {
        return false;
    }

    return true;
}
