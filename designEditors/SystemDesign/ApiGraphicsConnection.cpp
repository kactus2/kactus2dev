//-----------------------------------------------------------------------------
// File: ApiGraphicsConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// A graphical representation of a Kactus2 API interconnection.
//-----------------------------------------------------------------------------

#include "ApiGraphicsConnection.h"

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>

#include <IPXACTmodels/kactusExtensions/ApiInterconnection.h>
#include <IPXACTmodels/kactusExtensions/ConnectionRoute.h>
#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::ApiGraphicsConnection()
//-----------------------------------------------------------------------------
ApiGraphicsConnection::ApiGraphicsConnection(ConnectionEndpoint *endpoint1, ConnectionEndpoint *endpoint2,
                                             bool autoConnect, QString const& name, QString const& displayName,
                                             QString const& description, DesignDiagram* parent):
GraphicsConnection(endpoint1, endpoint2, autoConnect, name, displayName, description, parent),
apiInterconnection_(),
route_()
{

}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::ApiGraphicsConnection()
//-----------------------------------------------------------------------------
ApiGraphicsConnection::ApiGraphicsConnection(QPointF const& p1, QVector2D const& dir1, QPointF const& p2,
                                             QVector2D const& dir2, QString const& displayName,
                                             QString const& description, DesignDiagram* parent):
GraphicsConnection(p1, dir1, p2, dir2, displayName, description, parent),
apiInterconnection_(),
route_()
{

}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::ApiGraphicsConnection()
//-----------------------------------------------------------------------------
ApiGraphicsConnection::ApiGraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                                             QSharedPointer<ApiInterconnection> apiInterconnection,
                                             QSharedPointer<ConnectionRoute> route, bool autoConnect,
                                             DesignDiagram* parent):
GraphicsConnection(endpoint1, endpoint2, autoConnect, apiInterconnection->name(), apiInterconnection->displayName(),
                   apiInterconnection->description(), parent),
apiInterconnection_(apiInterconnection),
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
// Function: ApiGraphicsConnection::~ApiGraphicsConnection()
//-----------------------------------------------------------------------------
ApiGraphicsConnection::~ApiGraphicsConnection()
{

}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::setName()
//-----------------------------------------------------------------------------
void ApiGraphicsConnection::setName(QString const& name)
{
    GraphicsConnection::setName(name);

    apiInterconnection_->setName(name);
    route_->setName(name);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::name()
//-----------------------------------------------------------------------------
QString ApiGraphicsConnection::name() const
{
    return apiInterconnection_->name();
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::setDescription()
//-----------------------------------------------------------------------------
void ApiGraphicsConnection::setDescription(QString const& description)
{
    apiInterconnection_->setDescription(description);

    GraphicsConnection::setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::description()
//-----------------------------------------------------------------------------
QString ApiGraphicsConnection::description() const
{
    return apiInterconnection_->description();
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::getApiInterconnection()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterconnection> ApiGraphicsConnection::getApiInterconnection()
{
    return apiInterconnection_;
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::getRouteExtension()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> ApiGraphicsConnection::getRouteExtension() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::toggleOffPage()
//-----------------------------------------------------------------------------
void ApiGraphicsConnection::toggleOffPage()
{
    route_->setOffpage(!route_->isOffpage());
    GraphicsConnection::toggleOffPage();
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::isOffPage()
//-----------------------------------------------------------------------------
bool ApiGraphicsConnection::isOffPage() const
{
    return route_->isOffpage();
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::setRoute()
//-----------------------------------------------------------------------------
void ApiGraphicsConnection::setRoute(QList<QPointF> path)
{
    route_->setRoute(path);

    GraphicsConnection::setRoute(path);
}

//-----------------------------------------------------------------------------
// Function: ApiGraphicsConnection::changeConnectionComponentReference()
//-----------------------------------------------------------------------------
void ApiGraphicsConnection::changeConnectionComponentReference(QString const& oldName, QString const& newName)
{
    apiInterconnection_->changetInterfaceComponentReferences(oldName, newName);
}
