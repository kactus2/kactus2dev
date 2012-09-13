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

#include <models/ComInterface.h>

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
            // Generate a hexadecimal color based on the transfer type.
            unsigned int color = qHash(getComInterface()->getTransferType()) & 0x00FFFFFF;

            // Extract the color components and make small modifications.
            unsigned char r = (color - 0xDEADC0DE) >> 16;
            unsigned char g = 255 - ((color & 0x0000FF00) >> 8);
            unsigned char b = (color & 0x000000FF);

            setBrush(QBrush(QColor(r, g, b)));
            break;
        }

    default:
        {
            Q_ASSERT(false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWConnectionEndpoint::isConnectionValid()
//-----------------------------------------------------------------------------
bool SWConnectionEndpoint::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!ConnectionEndpoint::isConnectionValid(other))
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
