//-----------------------------------------------------------------------------
// File: ApiGraphicsConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// A graphical representation of a Kactus2 API interconnection.
//-----------------------------------------------------------------------------

#include "ApiGraphicsConnection.h"

#include <editors/common/diagramgrid.h>
#include <editors/common/DiagramUtil.h>

#include <IPXACTmodels/kactusExtensions/ApiInterconnection.h>
#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>
#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::ApiGraphicsConnection()
//-----------------------------------------------------------------------------
ApiGraphicsConnection::ApiGraphicsConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2,
    QVector2D const& dir2, DesignDiagram* parent) :
    ConnectionItem(p1, dir1, p2, dir2, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::ApiGraphicsConnection()
//-----------------------------------------------------------------------------
ApiGraphicsConnection::ApiGraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
    QSharedPointer<ApiInterconnection> apiInterconnection,
    QSharedPointer<ConnectionRoute> route, bool autoConnect,
    DesignDiagram* parent) :
    ConnectionItem(endpoint1, endpoint2, apiInterconnection, route, autoConnect, parent)
{

}
