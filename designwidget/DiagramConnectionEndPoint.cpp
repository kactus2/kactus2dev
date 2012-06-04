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
    : ConnectionEndpoint(parent),
      connections_()
{
    setDirection(dir);
}

//-----------------------------------------------------------------------------
// Function: ~DiagramConnectionEndpoint()
//-----------------------------------------------------------------------------
DiagramConnectionEndpoint::~DiagramConnectionEndpoint()
{
}

//-----------------------------------------------------------------------------
// Function: addInterconnection()
//-----------------------------------------------------------------------------
void DiagramConnectionEndpoint::addInterconnection(DiagramInterconnection* connection)
{
    connections_.append(connection);
}

//-----------------------------------------------------------------------------
// Function: removeInterconnection()
//-----------------------------------------------------------------------------
void DiagramConnectionEndpoint::removeInterconnection(DiagramInterconnection* connection)
{
    connections_.removeAll(connection);
}

//-----------------------------------------------------------------------------
// Function: getInterconnections()
//-----------------------------------------------------------------------------
QList<DiagramInterconnection*> const& DiagramConnectionEndpoint::getInterconnections() const
{
    return connections_;
}

//-----------------------------------------------------------------------------
// Function: isConnected()
//-----------------------------------------------------------------------------
bool DiagramConnectionEndpoint::isConnected() const
{
    return (!connections_.empty());
}

//-----------------------------------------------------------------------------
// Function: getOffPageConnector()
//-----------------------------------------------------------------------------
DiagramConnectionEndpoint* DiagramConnectionEndpoint::getOffPageConnector()
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: DiagramConnectionEndpoint::isAdHoc()
//-----------------------------------------------------------------------------
bool DiagramConnectionEndpoint::isAdHoc() const
{
    return (!isBus());
}
