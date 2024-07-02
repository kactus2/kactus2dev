//-----------------------------------------------------------------------------
// File: ConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 4.6.2012
//
// Description:
// Base class for connection endpoints (both SW and HW).
//-----------------------------------------------------------------------------

#ifndef CONNECTIONENDPOINT_H
#define CONNECTIONENDPOINT_H

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Component.h>

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>
#include <QGraphicsPolygonItem>
#include <QUndoCommand>

class ApiInterface;
class Component;
class ComponentItem;
class ComInterface;
class BusInterface;
class GraphicsConnection;
class Port;

//-----------------------------------------------------------------------------
//! Base class for connection endpoints (both SW and HW).
//-----------------------------------------------------------------------------
class ConnectionEndpoint : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! HighlightMode enumeration.
    //-----------------------------------------------------------------------------
    enum HighlightMode
    {
        HIGHLIGHT_OFF = 0,
        HIGHLIGHT_ALLOWED,
        HIGHLIGHT_HOVER,
    };

    //-----------------------------------------------------------------------------
    //! Endpoint type enumeration.
    //-----------------------------------------------------------------------------
    enum EndpointType
    {
        ENDPOINT_TYPE_UNDEFINED = 0,    //!< The endpoint type is not set.
        ENDPOINT_TYPE_API,              //!< The endpoint is an API interface.
        ENDPOINT_TYPE_COM,              //!< The endpoint is a COM interface.
        ENDPOINT_TYPE_BUS,              //!< The endpoint is a bus interface.
        ENDPOINT_TYPE_ADHOC             //!< The endpoint is an ad-hoc port.
    };

    /*!
     *  Constructor.
     *
     *      @param [in] parent     The parent graphics item.
     */
    ConnectionEndpoint(QGraphicsItem* parent = 0);

    /*!
     *  Destructor.
     */
    ~ConnectionEndpoint() override;


    // Disable copying.
    ConnectionEndpoint(ConnectionEndpoint const& rhs) = delete;
    ConnectionEndpoint& operator=(ConnectionEndpoint const& rhs) = delete;

    /*!
     *  Sets the highlighting mode.
     *
     *      @param [in] mode The highlight mode.
     */
    void setHighlight(HighlightMode mode);

    /*!
     *  Sets the selection highlight on/off.
     *
     *      @param [in] on If true, the selection highlight is turned on. Otherwise it is turned off.
     */
    virtual void setSelectionHighlight(bool on);

    /*!
     *  Attaches the endpoint to a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void addConnection(GraphicsConnection* connection);

    /*!
     *  Unattaches the endpoint from a connection.
     *
     *      @param [in] connection The connection.
     */
    virtual void removeConnection(GraphicsConnection* connection);

    /*!
     *  Returns the list of connections that are connected to this endpoint.
     */
    virtual QList<GraphicsConnection*> const& getConnections() const;

    /*!
     *  Revalidates all connections.
     */
    void revalidateConnections();

    /*!
     *  Returns true if the endpoint has at least one connection.
     */
    bool isConnected() const;


    /*!
     *  Update the connection positions of the associated end points.
     */
    void beginUpdateConnectionPositions() const;

    void endUpdateConnectionPositions(QUndoCommand* parentCommand) const;

    void updateConnectionPositions() const;
    
    /*!
     *  Called when a connection between this and another endpoint is done.
     *
     *      @param [in] other The other endpoint of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(ConnectionEndpoint const* other) = 0;

    /*!
     *  Called when a connection has been removed from between this and another endpoint.
     *
     *      @param [in] other The other endpoint of the connection.
     */
    virtual void onDisconnect();

    /*! 
     *  Returns true if this endpoint can be connected to the given endpoint, taking existing
     *  connections into account.
     *
     *      @param [in] other The endpoint to which to connect.
     */
    virtual bool canConnect(ConnectionEndpoint const* other) const;

    /*!
     *  Returns true if a connection is valid between the two endpoints.
     *
     *      @param      [in]    other The other endpoint.
     *
     *      @remarks            The function checks all possible connections between
     *                          two endpoint, including there off-page connectors
     *                          and parent connectors. Does not take into account 
     *                          if endpoint is exclusive. 
     * 
     *      @return             Boolean true if two endpoints can be connected, otherwise false.
     */
    virtual bool isConnectionValid(ConnectionEndpoint const* other) const;

    /*!
     *  Returns true if the endpoint is exclusive, i.e. can only have one connection.
     */
    virtual bool isExclusive() const = 0;

    /*!
     *  Sets the draw direction of the endpoint.
     *
     *      @param [in] dir The draw direction to set.
     *
     *      @remarks The direction can be changed only if isDirectionFixed() returns false.
     */
    virtual void setDirection(QVector2D const& dir);

    /*!
	 *	Returns the draw direction of the endpoint.
	 */
	virtual QVector2D getDirection() const;

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const noexcept;

    /*! 
     *  Returns the name of the endpoint.
     */
    virtual QString name() const = 0;

	/*! Set the name of the endpoint.
	 *
	 * \param name The name to set for the endpoint.
	 *
	*/
	virtual void setName(QString const& name) = 0;

	/*! Get the description of the endpoint.
	 *
	 *
	 * \return QString contains the description.
	*/
	virtual QString description() const = 0;

	/*! Set the description for the endpoint.
	 *
	 * \param description Contains the description to set.
	 *
	*/
	virtual void setDescription(QString const& description) = 0;

    /*!
     *  Updates the endpoint interface.
     */
    virtual void updateInterface() = 0;

    /*! 
     *  Returns the encompassing component, if the endpoint represents a COM/API interface on a component.
     */
    virtual ComponentItem* encompassingComp() const = 0;

	/*!
     *  Returns pointer to the top component that owns this interface.
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const = 0;

    /*! 
     *  Returns the IP-XACT bus interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a bus interface.
     *               Use isBus() function to check for bus interface support.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*! 
     *  Returns the COM interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a COM interface.
     *               Use isCom() function to check for COM interface support.
     */
    virtual QSharedPointer<ComInterface> getComInterface() const;

    /*! 
     *  Returns the API interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a API interface.
     *               Use isApi() function to check for API interface support.
     */
    virtual QSharedPointer<ApiInterface> getApiInterface() const;

    /*!
     *  Returns the ad-hoc port of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is a bus interface.
     *               Use isAdHoc() function to check for ad-hoc support.
     */
    virtual QSharedPointer<Port> getPort() const;

    /*!
     *  Returns the corresponding off-page connector or a null pointer if the endpoint does not have one.
     */
    virtual ConnectionEndpoint* getOffPageConnector() const;

    /*!
     *  Returns the corresponding parent connector or a null pointer if the endpoint is not off-page.
     */
    virtual ConnectionEndpoint* getParentConnector() const;

    /*!
     *  Returns true if the endpoint represents a hierarchical connection.
     */
    virtual bool isHierarchical() const noexcept = 0;

    /*!
     *  Returns true if the endpoint is a bus interface endpoint.
     */
    virtual bool isBus() const noexcept;

    /*!
     *  Returns true if the endpoint is an ad-hoc port.
     */
    virtual bool isAdHoc() const noexcept;

    /*!
     *  Returns true if the endpoint is a COM interface endpoint.
     */
    virtual bool isCom() const noexcept;

    /*!
     *  Returns true if the endpoint is an API interface endpoint.
     */
    virtual bool isApi() const noexcept;

    /*!
     *  Returns the type of the endpoint (API/COM/bus/ad-hoc/undefined).
     */
    virtual EndpointType getType() const noexcept;

    /*!
     *  Sets the endpoint temporary or not temporary. Temporary endpoints can be deleted.
     *
     *      @param [in] temp True if temporary; false if not temporary.
     */
    virtual void setTemporary(bool temp);

    /*!
     *  Returns true if the endpoint is temporary.
     */
    bool isTemporary() const noexcept;

    /*!
     *  Returns true if the endpoint is invalid (i.e. a removed one).
     */
    bool isInvalid() const;

    /*!
     *  Sets the endpoint type locked or not. A locked endpoint has a strict type, while a non-locked
     *  endpoint automatically adjusts its type when connections are added/removed.
     *
     *      @param [in] typed If true, the endpoint is set as typed.
     */
    void setTypeLocked(bool typed);

    /*!
     *  Returns true if the endpoint's type is locked.
     */
    bool isTypeLocked() const;

    /*!
     *  Get the interface mode of the connected bus interface.
     *
     *      @return The interface mode of the end point.
     */
    virtual General::InterfaceMode getInterfaceMode() const;
    
    /*!
	 *  Return the correct length of the name label.
	 */
	virtual qreal getNameLength();

	/*!
	 *  Shorten the name label to better fit the component.
	 *  
	 *      @param [in] width   The width of the shortened name.
	 */
	virtual void shortenNameLabel( qreal width );

    /*!
     *  Checks if the endpoint is off-page.
     *
     *      @return     Boolean true if endpoint is off-page, otherwise false.
     */
    bool isOffPage() const;

signals:
    //! Signals that the contents of the interface have been changed.
    void contentChanged();

    //! Emitted when the endpoint is destroyed.
    void destroyed(ConnectionEndpoint* endpoint);

    //! Emitted when the end point has been moved.
    void moved(ConnectionEndpoint* endpoint);

protected:
    /*!
     *  Begins updating connection names.
     */
    void beginUpdateConnectionNames();

    /*!
     *  Ends updating connection names.
     */
    void endUpdateConnectionNames();

private:

    /*!
     *  Checks if points are connected
     *
     *      @param [in] other The other endpoint.
     * 
     *      @return     Boolean true is the points are connected, otherwise false.
     *
     *      @remarks    Does  not take into account weather points are off-page
                        or not, thus checking two points directly.
     */
    bool isConnectionBetweenPointsExist(ConnectionEndpoint const* other) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint's direction.
    QVector2D dir_;

    //! The connections to this endpoint.
    QList<GraphicsConnection*> connections_;

    //! Boolean flag for determining if the endpoint is temporary or not.
    bool temporary_ = false;

    //! Boolean flag for determining if the endpoint is typed or not (non-typed).
    bool typeLocked_ = true;

    //! Connection update list.
    QList<GraphicsConnection*> connUpdateList_;
};

//-----------------------------------------------------------------------------

#endif // CONNECTIONENDPOINT_H
