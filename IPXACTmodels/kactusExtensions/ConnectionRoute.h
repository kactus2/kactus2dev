//-----------------------------------------------------------------------------
// File: ConnectionRoute.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.09.2015
//
// Description:
// Vendor extension for storing connection routes in design.
//-----------------------------------------------------------------------------

#ifndef CONNECTIONROUTE_H
#define CONNECTIONROUTE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Vendor extension for storing connection routes in design.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ConnectionRoute : public VendorExtension
{
public:
    //! The constructor.
    ConnectionRoute(QString const& connectionName);

    //! The destructor.
    ~ConnectionRoute();
    
    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual VendorExtension* clone() const;
    
    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;
    
    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Sets the connection name.
     *
     *      @param [in] name   The name of the connection.
     */
    void setName(QString const& name);

    /*!
     *  Gets the name of the connection the route is stored for.
     *
     *      @return The name of the route represented by the item.
     */
    QString name() const;

    /*!
     *  Sets the connection to offpage mode or normal mode.
     *
     *      @param [in] offpage   Offpage mode to set.
     */
    void setOffpage(bool offpage);

    /*!
     *  Checks if the connection is set for offpage mode.
     *
     *      @return True, if the connection is in offpage mode, otherwise false.
     */
    bool isOffpage() const;

    /*!
     *  Adds a route point in the connection.
     *
     *      @param [in] point   The point to add.
     */
    void addPoint(QPointF const& point);

    /*!
     *  Sets the route points for the connection.
     *
     *      @param [in] route   The route points to set.
     */
    void setRoute(QList<QPointF> const& route);

    /*!
     *  Gets the route points for the connection.
     *
     *      @return The route points.
     */
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