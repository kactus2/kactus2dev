//-----------------------------------------------------------------------------
// File: HWConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT interconnection.
//-----------------------------------------------------------------------------

#include "HWConnection.h"
#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "HWDesignWidget.h"

#include <editors/common/diagramgrid.h>
#include <editors/common/DiagramUtil.h>

#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/generaldeclarations.h>


//-----------------------------------------------------------------------------
// Function: HWConnection::HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2, 
    QSharedPointer<Interconnection> interconnection, QSharedPointer<ConnectionRoute> route, DesignDiagram* parent):
ConnectionItem(endpoint1, endpoint2, interconnection, route, false, parent)
{

}

//-----------------------------------------------------------------------------
// Function: HWConnection::HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
    DesignDiagram* parent):
    ConnectionItem(p1, dir1, p2, dir2, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ConnectionItem::isBus()
//-----------------------------------------------------------------------------
bool HWConnection::isBus() const
{
    return endpoint1()->isBus();
}

