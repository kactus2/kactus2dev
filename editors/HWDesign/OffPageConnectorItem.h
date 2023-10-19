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
    QString name() const override;

	/*!
	 *  Set the name of the parent bus interface.
	 *
     *      @param [in] name The name to set for the port.
	 *
	*/
	void setName(QString const& name) override;

	/*! 
     *  Get the description of the port.
	 *
     *      @return QString containing the description.
	 */
	QString description() const override;

	/*!
	 *  Set the description for the port.
     *
	 *      @param [in] description The description to set.
	 */
	void setDescription(QString const& description) override;

    /*!
     *  Attaches the endpoint to a connection.
     *
     *      @param [in] connection The connection.
     */
    void addConnection(GraphicsConnection* connection) override;

    /*!
     *  Unattaches the endpoint from a connection.
     *
     *      @param [in] connection The connection.
     */
    void removeConnection(GraphicsConnection* connection) override;

    /*!
     *  Called when a connection between this and another endpoint is done.
     *
     *      @param [in] other The other endpoint of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    bool onConnect(ConnectionEndpoint const* other) override;

    /*!
     *  Called when a connection has been removed from between this and another endpoint.
     */
    void onDisconnect() final;

    /*!
     *  Returns true if a connection is valid between the two endpoints.
     *
     *      @param [in] other The other endpoint.
     *
     *      @remarks Does not take existing connections into account but simply
     *               validates whether a connection between the endpoints would be valid
     *               in a general case.
     */
    bool isConnectionValid(ConnectionEndpoint const* other) const override;

    /*!
     *  Returns true if the endpoint is exclusive, i.e. can only have one connection.
     */
    bool isExclusive() const override;

    /*!
     *  Sets the draw direction of the endpoint.
     *
     *      @param [in] dir The draw direction to set.
     *
     *      @remarks The direction can be changed only if isDirectionFixed() returns false.
     */
    void setDirection(QVector2D const& dir) override;

    /*!
	 *	Returns the draw direction of the endpoint.
	 */
	virtual QVector2D getDirection() const;
      
    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    bool isDirectionFixed() const noexcept override;

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
    bool isHierarchical() const noexcept override;

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
    ConnectionEndpoint::EndpointType getType() const noexcept override;
    
    /*!
     *  Check if the end point is a bus interface.
     *
     *      @return True, if the end point is a bus interface end point, otherwise false.
     */
    bool isBus() const noexcept override;

    /*!
     *  Check if the end point is an ad hoc.
     *
     *      @return True if the endpoint is an ad-hoc port, otherwise false.
     */
    bool isAdHoc() const noexcept override;

    /*!
     *  Check if the end point is a com interface.
     *
     *      @return True if the endpoint is a COM interface endpoint, otherwise false.
     */
    bool isCom() const noexcept override;

    /*!
     *  Check if the end point is an api interface.
     *
     *      @return True if the endpoint is an API interface endpoint, otherwise false.
     */
    bool isApi() const noexcept override;

protected:

    /*!
     *  Handle the item changes.
     *
     *      @param [in] change  The change in the item.
     *      @param [in] value   Value for change.
     *
     *      @return The item change.
     */
    QVariant itemChange(GraphicsItemChange change, QVariant const& value) override;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent diagram connection endpoint.
    ConnectionEndpoint* parent_ = nullptr;
};

//-----------------------------------------------------------------------------

#endif // OFFPAGECONNECTORITEM_H
