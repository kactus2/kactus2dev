//-----------------------------------------------------------------------------
// File: ComGraphicsConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.02.2016
//
// Description:
// A graphical representation of a Kactus2 com interconnection.
//-----------------------------------------------------------------------------

#include "ComGraphicsConnection.h"

#include <editors/common/diagramgrid.h>
#include <editors/common/DiagramUtil.h>

#include <IPXACTmodels/kactusExtensions/ComInterconnection.h>
#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>
#include <IPXACTmodels/generaldeclarations.h>


//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::ComGraphicsConnection()
//-----------------------------------------------------------------------------
ComGraphicsConnection::ComGraphicsConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2,
                                             QVector2D const& dir2, DesignDiagram* parent):
ConnectionItem(p1, dir1, p2, dir2,  parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::ComGraphicsConnection()
//-----------------------------------------------------------------------------
ComGraphicsConnection::ComGraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                                             QSharedPointer<ComInterconnection> comInterconnection,
                                             QSharedPointer<ConnectionRoute> route, bool autoConnect,
                                             DesignDiagram* parent):
    ConnectionItem(endpoint1, endpoint2, comInterconnection, route, autoConnect, parent)
{

}
