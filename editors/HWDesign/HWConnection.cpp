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

#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>

//-----------------------------------------------------------------------------
// Function: HWConnection::HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2, 
    QSharedPointer<Interconnection> interconnection, QSharedPointer<ConnectionRoute> route, DesignDiagram* parent):
GraphicsConnection(endpoint1, endpoint2, false, interconnection->name(), interconnection->displayName(),
    interconnection->description(), parent),
    interconnection_(interconnection),
    route_(route)
{
    GraphicsConnection::setRoute(route_->getRoute());
}

//-----------------------------------------------------------------------------
// Function: HWConnection::HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2, QVector2D const& dir2,
    DesignDiagram* parent):
GraphicsConnection(p1, dir1, p2, dir2, parent)
{

}

//-----------------------------------------------------------------------------
// Function: HWConnection::~HWConnection()
//-----------------------------------------------------------------------------
HWConnection::~HWConnection()
{
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setName()
//-----------------------------------------------------------------------------
void HWConnection::setName(QString const& name)
{
    GraphicsConnection::setName(name);

    interconnection_->setName(name);
    route_->setName(name);
    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::name()
//-----------------------------------------------------------------------------
QString HWConnection::name() const
{
    return interconnection_->name();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setDescription()
//-----------------------------------------------------------------------------
void HWConnection::setDescription(QString const& description)
{
    interconnection_->setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::description()
//-----------------------------------------------------------------------------
QString HWConnection::description() const
{
    return interconnection_->description();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::getInterconnection()
//-----------------------------------------------------------------------------
QSharedPointer<Interconnection> HWConnection::getInterconnection()
{
    return interconnection_;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::getRouteExtension()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> HWConnection::getRouteExtension() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::toggleOffPage()
//-----------------------------------------------------------------------------
void HWConnection::toggleOffPage()
{
    route_->setOffpage(!route_->isOffpage());
    GraphicsConnection::toggleOffPage();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setRoute()
//-----------------------------------------------------------------------------
void HWConnection::setRoute(QList<QPointF> path)
{
    route_->setRoute(path);

    GraphicsConnection::setRoute(path);
}

//-----------------------------------------------------------------------------
// Function: HWConnection::isBus()
//-----------------------------------------------------------------------------
bool HWConnection::isBus() const
{
    return endpoint1()->isBus();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::changeConnectionComponentReference()
//-----------------------------------------------------------------------------
void HWConnection::changeConnectionComponentReference(QString const& oldName, QString const& newName)
{
    interconnection_->changeInterfaceComponentReferences(oldName, newName);
}
