//-----------------------------------------------------------------------------
// File: ComGraphicsConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.02.2016
//
// Description:
// A graphical representation of a Kactus2 com interconnection.
//-----------------------------------------------------------------------------

#include "ComGraphicsConnection.h"

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>

#include <IPXACTmodels/kactusExtensions/ComInterconnection.h>
#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>
#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::ComGraphicsConnection()
//-----------------------------------------------------------------------------
ComGraphicsConnection::ComGraphicsConnection(ConnectionEndpoint *endpoint1, ConnectionEndpoint *endpoint2,
                                             bool autoConnect, QString const& name, QString const& displayName,
                                             QString const& description, DesignDiagram* parent):
GraphicsConnection(endpoint1, endpoint2, autoConnect, name, displayName, description, parent),
comInterconnection_(),
route_()
{

}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::ComGraphicsConnection()
//-----------------------------------------------------------------------------
ComGraphicsConnection::ComGraphicsConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2,
                                             QVector2D const& dir2, QString const& displayName,
                                             QString const& description, DesignDiagram* parent):
GraphicsConnection(p1, dir1, p2, dir2, displayName, description, parent),
comInterconnection_(),
route_()
{

}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::ComGraphicsConnection()
//-----------------------------------------------------------------------------
ComGraphicsConnection::ComGraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                                             QSharedPointer<ComInterconnection> comInterconnection,
                                             QSharedPointer<ConnectionRoute> route, bool autoConnect,
                                             DesignDiagram* parent):
GraphicsConnection(endpoint1, endpoint2, autoConnect, comInterconnection->name(),
                   comInterconnection->displayName(), comInterconnection->description(), parent),
comInterconnection_(comInterconnection),
route_(route)
{
    if (route_->isOffpage())
    {
        GraphicsConnection::toggleOffPage();
    }
    else
    {
        GraphicsConnection::setRoute(route_->getRoute());
    }
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::~ComGraphicsConnection()
//-----------------------------------------------------------------------------
ComGraphicsConnection::~ComGraphicsConnection()
{

}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::setName()
//-----------------------------------------------------------------------------
void ComGraphicsConnection::setName(QString const& name)
{
    GraphicsConnection::setName(name);

    comInterconnection_->setName(name);
    route_->setName(name);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::name()
//-----------------------------------------------------------------------------
QString ComGraphicsConnection::name() const
{
    return comInterconnection_->name();
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::setDescription()
//-----------------------------------------------------------------------------
void ComGraphicsConnection::setDescription(QString const& description)
{
    comInterconnection_->setDescription(description);

    GraphicsConnection::setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::description()
//-----------------------------------------------------------------------------
QString ComGraphicsConnection::description() const
{
    return comInterconnection_->description();
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::getComInterconnection()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterconnection> ComGraphicsConnection::getComInterconnection()
{
    return comInterconnection_;
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::getRouteExtension()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> ComGraphicsConnection::getRouteExtension() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::toggleOffPage()
//-----------------------------------------------------------------------------
void ComGraphicsConnection::toggleOffPage()
{
    route_->setOffpage(!route_->isOffpage());
    GraphicsConnection::toggleOffPage();
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::isOffPage()
//-----------------------------------------------------------------------------
bool ComGraphicsConnection::isOffPage() const
{
    return route_->isOffpage();
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::setRoute()
//-----------------------------------------------------------------------------
void ComGraphicsConnection::setRoute(QList<QPointF> path)
{
    route_->setRoute(path);

    GraphicsConnection::setRoute(path);
}

//-----------------------------------------------------------------------------
// Function: ComGraphicsConnection::changeConnectionComponentReference()
//-----------------------------------------------------------------------------
void ComGraphicsConnection::changeConnectionComponentReference(QString const& oldName, QString const& newName)
{
    comInterconnection_->changetInterfaceComponentReferences(oldName, newName);
}
