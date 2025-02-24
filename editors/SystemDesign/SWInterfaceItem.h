//-----------------------------------------------------------------------------
// File: SWInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 24.5.2012
//
// Description:
// Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------

#ifndef SWINTERFACEITEM_H
#define SWINTERFACEITEM_H

#include "SWConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QSharedPointer>
#include <QVector2D>

class IGraphicsItemStack;
class InterfaceGraphicsData;
class Design;

//-----------------------------------------------------------------------------
//! Graphics item for visualizing API/COM interfaces for the top-level
//! component.
//-----------------------------------------------------------------------------
class SWInterfaceItem : public SWConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SW_INTERFACE_ITEM };

    /*!
     *  Constructor which creates a graphics item for an undefined interface.
     *
     *    @param [in] component           The selected component.
     *    @param [in] name                The name of the item.
     *    @param [in] interfaceGraphics   Container for the interface graphics data.
     *    @param [in] parent              Owner of this item.
     */
    SWInterfaceItem(QSharedPointer<Component> component, QString const& name,
        QSharedPointer<InterfaceGraphicsData> interfaceGraphics, QGraphicsItem *parent);

    /*!
     *  Constructor which creates a graphics item for an API interface.
     *
     *    @param [in] component           The selected component.
     *    @param [in] apiIf               The API interface.
     *    @param [in] interfaceGraphics   Container for the interface graphics data.
     *    @param [in] parent              Owner of this item.
     */
    SWInterfaceItem(QSharedPointer<Component> component, QSharedPointer<ApiInterface> apiIf,
        QSharedPointer<InterfaceGraphicsData> interfaceGraphics, QGraphicsItem *parent = 0);

    /*!
     *  Constructor which creates a graphics item for a COM interface.
     *
     *    @param [in] component           The selected component.
     *    @param [in] comIf               The COM interface.
     *    @param [in] interfaceGraphics   Container for the interface graphics data.
     *    @param [in] parent              Owner of this item.
     */
    SWInterfaceItem(QSharedPointer<Component> component, QSharedPointer<ComInterface> comIf,
        QSharedPointer<InterfaceGraphicsData> interfaceGraphics, QGraphicsItem *parent = 0);
    
    /*!
     *  Destructor.
     */
	~SWInterfaceItem() override = default;

    /*!
     *  Called when creating of a connection for this port has begun.
     */
    virtual void onBeginConnect();

    /*!
     *  Called when creating of a connection has ended.
     */
    virtual void onEndConnect();

    /*!
     *  Defines the interface as an API interface.
     *
     *    @param [in] apiIf The API interface to set.
     */
    void define(QSharedPointer<ApiInterface> apiIf);

    /*!
     *  Defines the interface as a COM interface.
     */
    void define(QSharedPointer<ComInterface> comIf);

    /*!
     *  Undefines the interface.
     */
    void undefine();

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

    /*!
     *  Sets the draw direction of the endpoint.
     *
     *    @param [in] dir The draw direction to set.
     *
     *      @remarks The direction can be changed only if isDirectionFixed() returns false.
     */
    virtual void setDirection(QVector2D const& dir);

    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // SWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Updates the endpoint interface.
     */
    virtual void updateInterface();

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    bool isDirectionFixed() const noexcept override;

    /*!
     *  Returns the name of the interface.
     */
    virtual QString name() const;

	/*!
     *  Sets the name for the interface.
	 *
	 *    @param [in] name The name to set for the interface.
	 */
	virtual void setName(QString const& name);

	/*!
     *  Returns the description of the interface.
	 */
	virtual QString description() const;

	/*! 
     *  Sets the description for the interface.
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
     *  Get the graphics data.
     *
     *    @return The container containing graphical data.
     */
    QSharedPointer<InterfaceGraphicsData> getInterfaceGraphicsData() const;

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
     *  Handles item changes.
     *
     *    @param [in] change  The change.
     *    @param [in] value   Value for change.
     *
     *    @return The item change.
     */
    virtual QVariant itemChange(GraphicsItemChange change, QVariant const& value);

    /*!
     *  Handles mouse press events.
     *
     *    @param [in] event   The mouse press event.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Handles mouse move events.
     *
     *    @param [in] event   The mouse move event.
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Handles mouse release events.
     *
     *    @param [in] event   The mouse release event.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
    /*!
     *  Makes basic initializations common to all constructors.
     */
    virtual void initialize();

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The top-level component.
    QSharedPointer<Component> component_ = nullptr;

    //! The COM interface, or null if not set.
    QSharedPointer<ComInterface> comInterface_ = nullptr;

    //! The API interface, or null if not set.
    QSharedPointer<ApiInterface> apiInterface_ = nullptr;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old stack from where the mouse drag event began.
    IGraphicsItemStack* oldStack_ = nullptr;

    //! The old positions of the other interfaces before mouse move.
    QMap<SWInterfaceItem*, QPointF> oldInterfacePositions_;

    //! The container for the graphical data.
    QSharedPointer<InterfaceGraphicsData> graphicsData_ = nullptr;
};

//-----------------------------------------------------------------------------

#endif // SWINTERFACEITEM_H
