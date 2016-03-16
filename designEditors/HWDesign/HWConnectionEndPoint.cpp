//-----------------------------------------------------------------------------
// File: HWConnectionEndpoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#include "HWConnectionEndpoint.h"

#include <QPen>

//-----------------------------------------------------------------------------
// Function: HWConnectionEndpont::HWConnectionEndpoint()
//-----------------------------------------------------------------------------
HWConnectionEndpoint::HWConnectionEndpoint(QGraphicsItem* parent, QVector2D const& dir)
    : ConnectionEndpoint(parent)
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
