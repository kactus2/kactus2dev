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
// Function: HWConnectionEndpoint()
//-----------------------------------------------------------------------------
HWConnectionEndpoint::HWConnectionEndpoint(QGraphicsItem* parent, bool temporary,
                                           QVector2D const& dir)
    : ConnectionEndpoint(parent, temporary)
{
    setDirection(dir);
}

//-----------------------------------------------------------------------------
// Function: ~HWConnectionEndpoint()
//-----------------------------------------------------------------------------
HWConnectionEndpoint::~HWConnectionEndpoint()
{
}
