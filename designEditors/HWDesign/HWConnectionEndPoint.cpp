//-----------------------------------------------------------------------------
// File: HWConnectionEndpoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#include "HWConnectionEndpoint.h"

#include <common/KactusColors.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QPen>

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpont::HWConnectionEndpoint()
//-----------------------------------------------------------------------------
HWConnectionEndpoint::HWConnectionEndpoint(QGraphicsItem* parent, QVector2D const& dir): ConnectionEndpoint(parent)
{
    setDirection(dir);
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpont::~HWConnectionEndpoint()
//-----------------------------------------------------------------------------
HWConnectionEndpoint::~HWConnectionEndpoint()
{
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpoint::updateInterface()
//-----------------------------------------------------------------------------
void HWConnectionEndpoint::updateInterface()
{
    ConnectionEndpoint::updateInterface();

    General::InterfaceMode mode = getInterfaceMode();

    if (mode == General::MASTER)
    {
        setBrush(QBrush(KactusColors::MASTER_INTERFACE));
    }
    else if (mode == General::SLAVE)
    {
        setBrush(QBrush(KactusColors::SLAVE_INTERFACE));
    }
    else if (mode == General::MIRROREDMASTER)
    {
        setBrush(QBrush(KactusColors::MIRROREDMASTER_INTERFACE));
    }
    else if (mode == General::MIRROREDSLAVE)
    {
        setBrush(QBrush(KactusColors::MIRROREDSLAVE_INTERFACE));
    }
    else if (mode == General::SYSTEM)
    {
        setBrush(QBrush(KactusColors::SYSTEM_INTERFACE));
    }
    else if (mode == General::MIRROREDSYSTEM)
    {
        setBrush(QBrush(KactusColors::MIRROREDSYSTEM_INTERFACE));
    }
    else if (mode == General::MONITOR)
    {
        setBrush(QBrush(KactusColors::MONITOR_INTERFACE));
    }
    else // if undefined
    {
        setBrush(QBrush(KactusColors::INVALID_INTERFACE));
    }
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpont::getNameLength()
//-----------------------------------------------------------------------------
qreal HWConnectionEndpoint::getNameLength()
{
	return name().length();
}

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpont::shortenNameLabel()
//-----------------------------------------------------------------------------
void HWConnectionEndpoint::shortenNameLabel(qreal /*width*/)
{
	
}
