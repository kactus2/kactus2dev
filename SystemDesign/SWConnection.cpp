//-----------------------------------------------------------------------------
// File: SWConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.5.2012
//
// Description:
// Graphics item for SW connections (API/COM).
//-----------------------------------------------------------------------------

#include "SWConnection.h"

#include "SystemMoveCommands.h"

#include "SWComponentItem.h"

#include <common/DesignDiagram.h>
#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>

#include <models/generaldeclarations.h>
#include <models/ApiInterface.h>
#include <models/ComInterface.h>

//-----------------------------------------------------------------------------
// Function: SWConnection()
//-----------------------------------------------------------------------------
SWConnection::SWConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                           bool autoConnect, QString const& name, QString const& displayName,
                           QString const& description, DesignDiagram* parent)
    : GraphicsConnection(endpoint1, endpoint2, autoConnect, name, displayName, description, parent)
{
}

//-----------------------------------------------------------------------------
// Function: SWConnection()
//-----------------------------------------------------------------------------
SWConnection::SWConnection(QPointF const& p1, QVector2D const& dir1,
                           QPointF const& p2, QVector2D const& dir2,
                           QString const& displayName,
                           QString const& description,
                           DesignDiagram* parent)
    : GraphicsConnection(p1, dir1, p2, dir2, displayName, description, parent)
{
}

//-----------------------------------------------------------------------------
// Function: ~SWConnection()
//-----------------------------------------------------------------------------
SWConnection::~SWConnection()
{
}
