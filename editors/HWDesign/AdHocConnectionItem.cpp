//-----------------------------------------------------------------------------
// File: AdHocConnectionItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.10.2015
//
// Description:
// Graphical item to represent ad-hoc connections in a design.
//-----------------------------------------------------------------------------

#include "AdHocConnectionItem.h"

#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "HWDesignWidget.h"

#include <editors/common/diagramgrid.h>
#include <editors/common/DiagramUtil.h>

#include <IPXACTmodels/Design/AdHocConnection.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::AdHocConnectionItem()
//-----------------------------------------------------------------------------
AdHocConnectionItem::AdHocConnectionItem(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2, 
    QSharedPointer<AdHocConnection> adhocConnection, QSharedPointer<ConnectionRoute> route,
    DesignDiagram* parent):
    ConnectionItem(endpoint1, endpoint2, adhocConnection, route, false, parent)
{
    setLineWidth(1);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::AdHocConnectionItem()
//-----------------------------------------------------------------------------
AdHocConnectionItem::AdHocConnectionItem(QPointF const& p1, QVector2D const& dir1,
    QPointF const& p2, QVector2D const& dir2,
    DesignDiagram* parent):
    ConnectionItem(p1, dir1, p2, dir2, parent)
{
    setLineWidth(1);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::isBus()
//-----------------------------------------------------------------------------
bool AdHocConnectionItem::isBus() const noexcept
{
    return false;
}

