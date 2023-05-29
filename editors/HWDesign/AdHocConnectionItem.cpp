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
    DesignDiagram* parent)
    : GraphicsConnection(endpoint1, endpoint2, true,
    adhocConnection->name(),
    adhocConnection->displayName(),
    adhocConnection->description(), parent),
    adHocConnection_(adhocConnection),
    route_(route)
{
    GraphicsConnection::setRoute(route_->getRoute());

    setLineWidth(1);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::AdHocConnectionItem()
//-----------------------------------------------------------------------------
AdHocConnectionItem::AdHocConnectionItem(QPointF const& p1, QVector2D const& dir1,
    QPointF const& p2, QVector2D const& dir2,
    DesignDiagram* parent):
GraphicsConnection(p1, dir1, p2, dir2, parent),
    adHocConnection_(),
    route_()
{
    setLineWidth(1);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::~AdHocConnectionItem()
//-----------------------------------------------------------------------------
AdHocConnectionItem::~AdHocConnectionItem()
{
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setName()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setName(QString const& name)
{
    GraphicsConnection::setName(name);

    adHocConnection_->setName(name);
    route_->setName(name);
    
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::name()
//-----------------------------------------------------------------------------
QString AdHocConnectionItem::name() const
{
    return adHocConnection_->name();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setDescription()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setDescription(QString const& description)
{
    adHocConnection_->setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::description()
//-----------------------------------------------------------------------------
QString AdHocConnectionItem::description() const
{
    return adHocConnection_->description();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::getAdHocConnection()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> AdHocConnectionItem::getAdHocConnection()
{
    return adHocConnection_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::getRouteExtension()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectionRoute> AdHocConnectionItem::getRouteExtension() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::connectEnds()
//-----------------------------------------------------------------------------
bool AdHocConnectionItem::connectEnds()
{
    if (!GraphicsConnection::connectEnds())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::toggleOffPage()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::toggleOffPage()
{
    route_->setOffpage(!route_->isOffpage());
    GraphicsConnection::toggleOffPage();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::setRoute()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::setRoute(QList<QPointF> path)
{
    route_->setRoute(path);

    GraphicsConnection::setRoute(path);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::isBus()
//-----------------------------------------------------------------------------
bool AdHocConnectionItem::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionItem::changeConnectionComponentReference()
//-----------------------------------------------------------------------------
void AdHocConnectionItem::changeConnectionComponentReference(QString const& oldName, QString const& newName)
{
    for (QSharedPointer<PortReference> portReference : *adHocConnection_->getInternalPortReferences())
    {
        if (portReference->getComponentRef().compare(oldName.toStdString()) == 0)
        {
            portReference->setComponentRef(newName.toStdString());
        }
    }
}
