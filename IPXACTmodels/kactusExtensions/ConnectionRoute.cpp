//-----------------------------------------------------------------------------
// File: ConnectionRoute.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.09.2015
//
// Description:
// Vendor extension for storing connection routes in design.
//-----------------------------------------------------------------------------

#include "ConnectionRoute.h"

#include <QPointF>

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::ConnectionRoute()
//-----------------------------------------------------------------------------
ConnectionRoute::ConnectionRoute(QString const& connectionName) :
name_(connectionName),
    offpage_(false),
    route_()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::~ConnectionRoute()
//-----------------------------------------------------------------------------
ConnectionRoute::~ConnectionRoute()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::clone()
//-----------------------------------------------------------------------------
VendorExtension* ConnectionRoute::clone() const
{
    return new ConnectionRoute(*this);
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::type()
//-----------------------------------------------------------------------------
QString ConnectionRoute::type() const
{
    return "kactus2:route";
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::write()
//-----------------------------------------------------------------------------
void ConnectionRoute::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:route");
    
    writer.writeAttribute("kactus2:connRef", name());
    if (isOffpage())
    {
        writer.writeAttribute("kactus2:offPage", "true");
    }
    else
    {
        writer.writeAttribute("kactus2:offPage", "false");
    }

    foreach (QPointF routePoint, route_)
    {
        writer.writeEmptyElement("kactus2:position");
        writer.writeAttribute("x", QString::number(routePoint.x()));
        writer.writeAttribute("y", QString::number(routePoint.y()));
    }

    writer.writeEndElement();
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::setName()
//-----------------------------------------------------------------------------
void ConnectionRoute::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::name()
//-----------------------------------------------------------------------------
QString ConnectionRoute::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::setOffpage()
//-----------------------------------------------------------------------------
void ConnectionRoute::setOffpage(bool offpage)
{
    offpage_ = offpage;
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::isOffpage()
//-----------------------------------------------------------------------------
bool ConnectionRoute::isOffpage() const
{
    return offpage_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::addPoint()
//-----------------------------------------------------------------------------
void ConnectionRoute::addPoint(QPointF const& point)
{
    route_.append(point);
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::setRoute()
//-----------------------------------------------------------------------------
void ConnectionRoute::setRoute(QList<QPointF> const& route)
{
    route_ = route;
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::getRoute()
//-----------------------------------------------------------------------------
QList<QPointF> ConnectionRoute::getRoute() const
{
    return route_;
}

//-----------------------------------------------------------------------------
// Function: ConnectionRoute::ConnectionRoute()
//-----------------------------------------------------------------------------
ConnectionRoute::ConnectionRoute(ConnectionRoute const& other):
name_(other.name_),
    offpage_(other.offpage_),
    route_(other.route_)
{

}
