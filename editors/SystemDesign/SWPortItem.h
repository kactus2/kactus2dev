//-----------------------------------------------------------------------------
// File: SWPortItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 2.5.2012
//
// Description:
// Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------

#ifndef SWPORTITEM_H
#define SWPORTITEM_H

#include "SWConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QSharedPointer>
#include <QPen>

//-----------------------------------------------------------------------------
//! Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------
class SWPortItem : public SWConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SW_PORT_ITEM };

    /*!
     *  Constructor which creates a graphics item for an undefined interface.
     *
     *    @param [in] name                    Name of the item.
     *    @param [in] containingComponent     Component containing the item.
     *    @param [in] parent                  The parent item.
     */
    SWPortItem(QString const& name, QSharedPointer<Component> containingComponent, QGraphicsItem *parent);

    /*!
     *  Constructor which creates a graphics item for an API interface.
     *
     *    @param [in] apiIf                   The API interface.
     *    @param [in] containingComponent     Component containing the item.
     *    @param [in] parent                  The parent item.
     */
    SWPortItem(QSharedPointer<ApiInterface> apiIf,
        QSharedPointer<Component> containingComponent = QSharedPointer<Component>(), QGraphicsItem *parent = 0);

    /*!
     *  Constructor which creates a graphics item for a COM interface.
     *
     *    @param [in] comIf                   The COM interface.
     *    @param [in] containingComponent     Component containing the item.
     *    @param [in] parent                  The parent item.
     */
    SWPortItem(QSharedPointer<ComInterface> comIf,
        QSharedPointer<Component> containingComponent = QSharedPointer<Component>(), QGraphicsItem *parent = 0);
    
    /*!
     *  Destructor.
     */
	~SWPortItem() final = default;

    /*!
     *  Called when creating of a connection for this port has begun.
     */
    virtual void onBeginConnect();

    /*!
     *  Called when creating of a connection has ended.
     */
    virtual void onEndConnect();

    /*!
     *  Sets the selection highlight on/off.
     *
     *    @param [in] on If true, the selection highlight is turned on. Otherwise it is turned off.
     */
    virtual void setSelectionHighlight(bool on);

    /*!
     *  Sets the COM/API type. The type of the VLNV determines the type of the endpoint.
     *
     *    @param [in] type The VLNV of the COM/API definition.
     */
    virtual void setTypeDefinition(VLNV const& type);

    /*!
     *  Returns the currently set COM/API definition.
     */
    virtual VLNV getTypeDefinition() const;

    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // SWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Attaches the endpoint to a connection.
     *
     *    @param [in] connection The connection.
     */
    virtual void addConnection(GraphicsConnection* connection);

    /*!
     *  Unattaches the endpoint from a connection.
     *
     *    @param [in] connection The connection.
     */
    virtual void removeConnection(GraphicsConnection* connection);

    /*!
     *  Updates the endpoint interface.
     */
    virtual void updateInterface();

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    bool isDirectionFixed() const noexcept override;

    /*!
     *  Returns the name of this port.
     */
    virtual QString name() const;

	/*!
     *  Sets the name for the port.
	 *
	 *    @param [in] name The name to set for the port.
	 */
	virtual void setName(QString const& name);

	/*!
     *  Returns the description of the port.
	 */
	virtual QString description() const;

	/*! 
     *  Sets the description for the port.
	 *
	 *    @param [in] description The description to set.
	 */
	virtual void setDescription(QString const& description);

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *    @param [in] other The other end point of the connection.
     *
     *    @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(ConnectionEndpoint const* other);

    /*!
     *  Called when a connection has been removed from between this and another end point.
     */
    virtual void onDisconnect() override final;

    /*!
     *  Returns true if a connection is valid between the two endpoints.
     *
     *    @param [in] other The other endpoint.
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
	 *  Set the position of the name label.
	 */
	void setLabelPosition();

	/*!
	 *  Check the direction of the port and change it if necessary.
	 */
	void checkDirection();

	/*!
	 *  Return the correct length of the name label.
	 */
	virtual qreal getNameLength();

	/*!
	 *  Shorten the name label to better fit the component.
	 *  
	 *    @param [in] width   The width of the shortened name.
	 */
	void shortenNameLabel( qreal width ) final;

    /*!
     *  Returns true if the endpoint is a COM interface endpoint.
     */
    bool isCom() const noexcept override;

    /*!
     *  Returns true if the endpoint is an API interface endpoint.
     */
    bool isApi() const noexcept override;

protected:
    
    /*!
     *  Item changes.
     *
     *    @param [in] change  The changed graphics item.
     *    @param [in] value   The new value.
     */
    virtual QVariant itemChange(GraphicsItemChange change, QVariant const& value);
    
    /*!
     *  Handler for mouse press events.
     *
     *    @param [in] event   The caught mouse press event.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    
    /*!
     *  Handler for mouse move events.
     *
     *    @param [in] event   The mouse movement event.
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Handler for mouse release event.
     *
     *    @param [in] event   The mouse release event.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Initializes the port graphics.
     */
    virtual void initialize();

private:

    /*!
     *  Returns true if the port has invalid connections.
     */
    bool hasInvalidConnections();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The COM interface, or null if not set.
    QSharedPointer<ComInterface> comInterface_ = nullptr;

    //! The API interface, or null if not set.
    QSharedPointer<ApiInterface> apiInterface_ = nullptr;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<SWPortItem*, QPointF> oldPortPositions_;

    //! Stub line for fixing the overdraw issue.
    QGraphicsLineItem stubLine_;

    //! Default pen for the stub line.
    QPen stubLineDefaultPen_;
};

//-----------------------------------------------------------------------------

#endif // SWPORTITEM_H
