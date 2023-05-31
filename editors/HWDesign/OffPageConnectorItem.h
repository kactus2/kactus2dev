//-----------------------------------------------------------------------------
// File: OffPageConnectorItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 9.1.2012
//
// Description:
// Off-page connector for the block diagram.
//-----------------------------------------------------------------------------

#ifndef OFFPAGECONNECTORITEM_H
#define OFFPAGECONNECTORITEM_H

#include <common/graphicsItems/ConnectionEndpoint.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

class BusInterface;
class HWComponentItem;
class HWConnectionEndpoint;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! OffPageConnectorItem class.
//-----------------------------------------------------------------------------
class OffPageConnectorItem : public ConnectionEndpoint
{
    Q_OBJECT

public:
    enum
    {
        Type = GFX_TYPE_DIAGRAM_OFF_PAGE_CONNECTOR
    };

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent connection endpoint.
     */
    explicit OffPageConnectorItem(ConnectionEndpoint* parent);

	/*!
     *  The destructor.
     */
	~OffPageConnectorItem() final = default;

    // Disable copying.
    OffPageConnectorItem(OffPageConnectorItem const& rhs) = delete;
    OffPageConnectorItem& operator=(OffPageConnectorItem const& rhs) = delete;

    /*! 
     *  Updates the graphics to match the IP-XACT bus interface.
     */
    void updateInterface();

	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*! 
     *  Returns the name of the parent bus interface.
     */
    virtual QString name() const;

	/*!
	 *  Set the name of the parent bus interface.
	 *
     *      @param [in] name The name to set for the port.
	 *
	*/
	virtual void setName(const QString& name);

	/*! 
     *  Get the description of the port.
	 *
     *      @return QString containing the description.
	 */
	virtual QString description() const;

	/*!
	 *  Set the description for the port.
     *
	 *      @param [in] description The description to set.
	 */
	virtual void setDescription(const QString& description);

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
     *  Called when a connection between this and another endpoint is done.
     *
     *      @param [in] other The other endpoint of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(ConnectionEndpoint const* other);

    /*!
     *  Called when a connection has been removed from between this and another endpoint.
     */
    virtual void onDisconnect() override final;

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
    virtual bool isExclusive() const;

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
    virtual bool isDirectionFixed() const;

    /*! 
     *  Returns the encompassing component, if it represents a bus interface on a component.
     */
    virtual ComponentItem* encompassingComp() const;

	/*!
	 *  Returns pointer to the top component that owns this interface.
	 *
	 *      @return Pointer to the component to which this interface belongs to.
	*/
	virtual QSharedPointer<Component> getOwnerComponent() const;

    /*! 
     *  Returns the IP-XACT bus interface model of the parent diagram connection endpoint.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*!
     *  Returns the ad-hoc port of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is a bus interface.
     *               Use isBus() function to check for ad-hoc support (isBus() == false).
     */
    virtual QSharedPointer<Port> getPort() const;

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
     *  Returns true if the port represents a hierarchical connection.
     */
    virtual bool isHierarchical() const;

    /*!
     *  Get the bus interface mode of the end point item.
     *
     *      @return The bus interface mode.
     */
    virtual General::InterfaceMode getInterfaceMode() const;

    /*!
     *  Get the type of the end point item.
     *
     *      @return The type of the end point item.
     */
    virtual ConnectionEndpoint::EndpointType getType() const;
    
    /*!
     *  Check if the end point is a bus interface.
     *
     *      @return True, if the end point is a bus interface end point, otherwise false.
     */
    virtual bool isBus() const;

    /*!
     *  Check if the end point is an ad hoc.
     *
     *      @return True if the endpoint is an ad-hoc port, otherwise false.
     */
    virtual bool isAdHoc() const;

    /*!
     *  Check if the end point is a com interface.
     *
     *      @return True if the endpoint is a COM interface endpoint, otherwise false.
     */
    bool isCom() const;

    /*!
     *  Check if the end point is an api interface.
     *
     *      @return True if the endpoint is an API interface endpoint, otherwise false.
     */
    bool isApi() const;

protected:

    /*!
     *  Handle the item changes.
     *
     *      @param [in] change  The change in the item.
     *      @param [in] value   Value for change.
     *
     *      @return The item change.
     */
    virtual QVariant itemChange(GraphicsItemChange change, QVariant const& value);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent diagram connection endpoint.
    ConnectionEndpoint* parent_ = nullptr;
};

//-----------------------------------------------------------------------------

#endif // OFFPAGECONNECTORITEM_H
