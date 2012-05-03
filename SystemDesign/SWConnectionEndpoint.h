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

#include <QList>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>
#include <QGraphicsPolygonItem>

class SWComponentItem;
class ApiInterface;
class ComInterface;
class SWConnection;
class VLNV;

//-----------------------------------------------------------------------------
//! Common interface for SW connection endpoints.
//-----------------------------------------------------------------------------
class SWConnectionEndpoint : public QObject, public QGraphicsPolygonItem
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
     *  Sets the highlighting mode.
     *
     *      @param [in] mode The highlight mode.
     */
    void setHighlight(HighlightMode mode);

    /*!
     *  Sets the COM/API type. The type of the VLNV determines the type of the endpoint.
     */
    //virtual void setTypeDefinition(VLNV const& type) = 0;

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
     *  Returns the encompassing component, if the endpoint represents a COM/API interface on a component.
     */
    virtual SWComponentItem* encompassingComp() const = 0;

	/*!
     *  Returns pointer to the top component that owns this interface.
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const = 0;

    /*! 
     *  Returns the COM interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a COM interface.
     *               Use isCom() function to check for COM interface support.
     */
    virtual QSharedPointer<ComInterface> getComInterface() const = 0;

    /*! 
     *  Returns the API interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a API interface.
     *               Use isApi() function to check for API interface support.
     */
    virtual QSharedPointer<ApiInterface> getApiInterface() const = 0;

    /*!
     *  Returns true if the endpoint represents a hierarchical connection.
     */
    virtual bool isHierarchical() const = 0;

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

signals:
    //! Signals that the contents of the interface have been changed.
    void contentChanged();

	//! \brief Emitted when a SWConnectionEndpoint is destroyed.
	void destroyed(SWConnectionEndpoint* endPoint);

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

    //! The endpoint's direction.
    QVector2D dir_;
};

//-----------------------------------------------------------------------------

#endif // SWCONNECTIONENDPOINT_H
