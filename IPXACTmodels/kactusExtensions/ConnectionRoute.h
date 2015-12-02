//-----------------------------------------------------------------------------
// File: ConnectionRoute.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.09.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef CONNECTIONROUTE_H
#define CONNECTIONROUTE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//!
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ConnectionRoute : public VendorExtension
{
public:
    //! The constructor.
    ConnectionRoute(QString const& connectionName);

    //! The destructor.
    ~ConnectionRoute();

    virtual VendorExtension* clone() const;
    
    virtual QString type() const;
    
    virtual void write(QXmlStreamWriter& writer) const;

    void setName(QString const& name);

    QString name() const;

    void setOffpage(bool offpage);

    bool isOffpage() const;

    void addPoint(QPointF const& point);

    void setRoute(QList<QPointF> const& route);

    QList<QPointF> getRoute() const;

private:

    // Disable copying.
    ConnectionRoute(ConnectionRoute const& other);
    ConnectionRoute& operator=(ConnectionRoute const& rhs); 

    //! The name of the connection.
    QString name_;

    //! Whether or not the connection is drawn in off-page mode.
    bool offpage_;

    //! The corner points on the route.
    QList<QPointF> route_;
};

#endif // CONNECTIONROUTE_H