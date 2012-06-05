//-----------------------------------------------------------------------------
// File: DiagramConnectionEndpoint.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.3.2011
//
// Description:
// Interface class for the diagram connection end points.
//-----------------------------------------------------------------------------

#include "DiagramConnectionEndpoint.h"

#include <QPen>

//-----------------------------------------------------------------------------
// Function: DiagramConnectionEndpoint()
//-----------------------------------------------------------------------------
DiagramConnectionEndpoint::DiagramConnectionEndpoint(QGraphicsItem* parent,
                                                     QVector2D const& dir)
    : ConnectionEndpoint(parent)
{
    setDirection(dir);
}

//-----------------------------------------------------------------------------
// Function: ~DiagramConnectionEndpoint()
//-----------------------------------------------------------------------------
DiagramConnectionEndpoint::~DiagramConnectionEndpoint()
{
}
