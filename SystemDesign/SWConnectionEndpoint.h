//-----------------------------------------------------------------------------
// File: SWConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.5.2012
//
// Description:
// Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------

#ifndef SWCONNECTIONENDPOINT_H
#define SWCONNECTIONENDPOINT_H

#include <models/generaldeclarations.h>
#include <models/component.h>

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>
#include <QGraphicsPolygonItem>

class ComponentItem;
class ApiInterface;
class ComInterface;
class SWConnection;
class VLNV;

//-----------------------------------------------------------------------------
//! Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------
class SWConnectionEndpoint : public ConnectionEndpoint
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Endpoint type enumeration.
    //-----------------------------------------------------------------------------
    enum EndpointType
    {
        ENDPOINT_TYPE_UNDEFINED = 0,
        ENDPOINT_TYPE_API,
        ENDPOINT_TYPE_COM
    };

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent graphics item.
     *      @param [in] dir    The initial direction for the endpoint.
     */
    SWConnectionEndpoint(QGraphicsItem* parent = 0,
                         QVector2D const& rhs = QVector2D(0.0f, -1.0f));

    /*!
     *  Destructor.
     */
    virtual ~SWConnectionEndpoint();

    /*!
     *  Sets the selection highlight on/off.
     *
     *      @param [in] on If true, the selection highlight is turned on. Otherwise it is turned off.
     */
    virtual void setSelectionHighlight(bool on);

    /*!
     *  Sets the COM/API type. The type of the VLNV determines the type of the endpoint.
     *
     *      @param [in] type The VLNV of the COM/API definition.
     */
    virtual void setTypeDefinition(VLNV const& type) = 0;

    /*!
     *  Returns the currently set COM/API definition.
     */
    virtual VLNV getTypeDefinition() const = 0;

    /*!
     *  Called when creating of a connection for this port has begun.
     */
    virtual void onBeginConnect() = 0;

    /*!
     *  Called when creating of a connection has ended.
     */
    virtual void onEndConnect() = 0;

    /*!
     *  Attaches the endpoint to a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void addConnection(SWConnection* connection);

    /*!
     *  Unattaches the endpoint from a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void removeConnection(SWConnection* connection);

    /*!
     *  Returns the list of connections that are connected to this endpoint.
     */
    virtual QList<SWConnection*> const& getConnections() const;

    /*!
     *  Returns true if the endpoint has at least one connection.
     */
    bool isConnected() const;

    /*!
     *  Called when a connection between this and another endpoint is done.
     *
     *      @param [in] other The other endpoint of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(SWConnectionEndpoint const* other) = 0;

    /*!
     *  Called when a connection has been removed from between this and another endpoint.
     *
     *      @param [in] other The other endpoint of the connection.
     */
    virtual void onDisconnect(SWConnectionEndpoint const* other) = 0;

    /*! 
     *  Returns true if this endpoint can be connected to the given endpoint.
     *
     *      @param [in] other The endpoint to which to connect.
     */
    virtual bool canConnect(SWConnectionEndpoint const* other) const;

    /*!
     *  Updates the endpoint interface.
     */
    virtual void updateInterface();

    /*!
     *  Returns true if the endpoint is a COM interface endpoint.
     */
    bool isCom() const;

    /*!
     *  Returns true if the endpoint is an API interface endpoint.
     */
    bool isApi() const;

    /*!
     *  Sets the endpoint type.
     */
    void setType(EndpointType type);

    /*!
     *  Returns the type of the endpoint (API/COM/undefined).
     */
    EndpointType getType() const;

private:
    // Disable copying.
    SWConnectionEndpoint(SWConnectionEndpoint const& rhs);
    SWConnectionEndpoint& operator=(SWConnectionEndpoint const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint's type.
    EndpointType type_;

    //! The connections to this endpoint.
    QList<SWConnection*> connections_;
};

//-----------------------------------------------------------------------------

#endif // SWCONNECTIONENDPOINT_H
