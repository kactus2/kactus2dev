//-----------------------------------------------------------------------------
// File: SWInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.5.2012
//
// Description:
// Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------

#ifndef SWINTERFACEITEM_H
#define SWINTERFACEITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

#include "SWConnectionEndpoint.h"

class IGraphicsItemStack;
class SWOffPageConnectorItem;

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
     */
    SWInterfaceItem(QSharedPointer<Component> component, QString const& name, QGraphicsItem *parent);

    /*!
     *  Constructor which creates a graphics item for an API interface.
     */
    SWInterfaceItem(QSharedPointer<Component> component,
                    QSharedPointer<ApiInterface> apiIf, QGraphicsItem *parent = 0);

    /*!
     *  Constructor which creates a graphics item for a COM interface.
     */
    SWInterfaceItem(QSharedPointer<Component> component,
                    QSharedPointer<ComInterface> comIf, QGraphicsItem *parent = 0);
    
    /*!
     *  Destructor.
     */
	virtual ~SWInterfaceItem();

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
     *      @param [in] apiIf The API interface to set.
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
     *      @param [in] type The VLNV of the COM/API definition.
     */
    virtual void setTypeDefinition(VLNV const& type);

    /*!
     *  Returns the currently set COM/API definition.
     */
    virtual VLNV getTypeDefinition() const;

    /*!
     *  Sets the draw direction of the endpoint.
     *
     *      @param [in] dir The draw direction to set.
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
    virtual bool isDirectionFixed() const;

    /*!
     *  Returns the name of the interface.
     */
    virtual QString name() const;

	/*!
     *  Sets the name for the interface.
	 *
	 *      @param [in] name The name to set for the interface.
	 */
	virtual void setName(const QString& name);

	/*!
     *  Returns the description of the interface.
	 */
	virtual QString description() const;

	/*! 
     *  Sets the description for the interface.
	 *
	 *      @param [in] description The description to set.
	 */
	virtual void setDescription(const QString& description);

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *      @param [in] other The other end point of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(ConnectionEndpoint const* other);

    /*!
     *  Called when a connection has been removed from between this and another end point.
     *
     *      @param [in] other The other end point of the connection.
     */
    virtual void onDisconnect(ConnectionEndpoint const* other);

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
     *  Returns the encompassing component.
     */
    virtual ComponentItem* encompassingComp() const;

	/*!
     *  Returns a pointer to the top component that owns this interface.
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const;

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
     *  Returns the corresponding off-page connector or a null pointer if the end point does not have one.
     */
    virtual ConnectionEndpoint* getOffPageConnector();

	/*!
	 *  Set the position of the name label.
	 */
	void setLabelPosition();

protected:
    virtual QVariant itemChange(GraphicsItemChange change, QVariant const& value);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    /*!
     *  Makes basic initializations common to all constructors.
     */
    void initialize();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name label.
    QGraphicsTextItem nameLabel_;

    //! The top-level component.
    QSharedPointer<Component> component_;

    //! The COM interface, or null if not set.
    QSharedPointer<ComInterface> comInterface_;

    //! The API interface, or null if not set.
    QSharedPointer<ApiInterface> apiInterface_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old stack from where the mouse drag event began.
    IGraphicsItemStack* oldStack_;

    //! The old positions of the other interfaces before mouse move.
    QMap<SWInterfaceItem*, QPointF> oldInterfacePositions_;

    //! The off-page connector.
    SWOffPageConnectorItem* offPageConnector_;
};

//-----------------------------------------------------------------------------

#endif // SWINTERFACEITEM_H
