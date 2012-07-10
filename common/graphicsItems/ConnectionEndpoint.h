//-----------------------------------------------------------------------------
// File: ConnectionEndpoint.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.6.2012
//
// Description:
// Base class for connection endpoints (both SW and HW).
//-----------------------------------------------------------------------------

#ifndef CONNECTIONENDPOINT_H
#define CONNECTIONENDPOINT_H

#include <models/generaldeclarations.h>
#include <models/component.h>

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>
#include <QGraphicsPolygonItem>

class ComponentItem;
class ApiInterface;
class ComInterface;
class BusInterface;
class GraphicsConnection;

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
     *      @param [in] temporary  If true, the endpoint is set temporary.
     */
    ConnectionEndpoint(QGraphicsItem* parent = 0, bool temporary = false);

    /*!
     *  Destructor.
     */
    virtual ~ConnectionEndpoint();

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
    virtual bool onConnect(ConnectionEndpoint const* other) = 0;

    /*!
     *  Called when a connection has been removed from between this and another endpoint.
     *
     *      @param [in] other The other endpoint of the connection.
     */
    virtual void onDisconnect(ConnectionEndpoint const* other) = 0;

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
     *      @param [in] other The other endpoint.
     *
     *      @remarks Does not take existing connections into account but simply
     *               validates whether a connection between the endpoints would be valid
     *               in a general case.
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
	virtual QVector2D const& getDirection() const;

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

    /*! 
     *  Returns the name of the endpoint.
     */
    virtual QString name() const = 0;

	/*! \brief Set the name of the endpoint.
	 *
	 * \param name The name to set for the endpoint.
	 *
	*/
	virtual void setName(const QString& name) = 0;

	/*! \brief Get the description of the endpoint.
	 *
	 *
	 * \return QString contains the description.
	*/
	virtual QString description() const = 0;

	/*! \brief Set the description for the endpoint.
	 *
	 * \param description Contains the description to set.
	 *
	*/
	virtual void setDescription(const QString& description) = 0;

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
    virtual Port* getPort() const;

    /*!
     *  Returns the corresponding off-page connector or a null pointer if the endpoint does not have one.
     */
    virtual ConnectionEndpoint* getOffPageConnector();

    /*!
     *  Returns true if the endpoint represents a hierarchical connection.
     */
    virtual bool isHierarchical() const = 0;

    /*!
     *  Returns true if the endpoint is a bus interface endpoint.
     */
    virtual bool isBus() const;

    /*!
     *  Returns true if the endpoint is an ad-hoc port.
     */
    virtual bool isAdHoc() const;

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
     *
     *      @param [in] type The endpoint type to set.
     */
    void setType(EndpointType type);

    /*!
     *  Returns the type of the endpoint (API/COM/bus/ad-hoc/undefined).
     */
    virtual EndpointType getType() const;

    /*!
     *  Sets the endpoint temporary or not temporary. Temporary endpoints can be deleted.
     *
     *      @param [in] temp True if temporary; false if not temporary.
     */
    void setTemporary(bool temp);

    /*!
     *  Returns true if the endpoint is temporary.
     */
    bool isTemporary() const;

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

signals:
    //! Signals that the contents of the interface have been changed.
    void contentChanged();

    //! \brief Emitted when the endpoint is destroyed.
    void destroyed(ConnectionEndpoint* endpoint);

private:
    // Disable copying.
    ConnectionEndpoint(ConnectionEndpoint const& rhs);
    ConnectionEndpoint& operator=(ConnectionEndpoint const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint's direction.
    QVector2D dir_;

    //! The endpoint's type.
    EndpointType type_;

    //! The connections to this endpoint.
    QList<GraphicsConnection*> connections_;

    //! Boolean flag for determining if the endpoint is temporary or not.
    bool temporary_;

    //! Boolean flag for determining if the endpoint is typed or not (non-typed).
    bool typeLocked_;
};

//-----------------------------------------------------------------------------

#endif // CONNECTIONENDPOINT_H
