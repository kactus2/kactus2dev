//-----------------------------------------------------------------------------
// File: BusPortItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT bus interface in a component instance.
//-----------------------------------------------------------------------------
#ifndef BUSPORTITEM_H
#define BUSPORTITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <designEditors/HWDesign/BusInterfaceEndPoint.h>

#include <IPXACTmodels/Component/PortMap.h>

#include <QSharedPointer>
#include <QPolygonF>

class BusInterface;
class HWComponentItem;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! HWConnection represents graphically an IP-XACT bus interface in a component instance
//-----------------------------------------------------------------------------
class BusPortItem : public BusInterfaceEndPoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_PORT };

    
    /*!
     *  The constructor.
     *
     *      @param [in] busIf       The bus interface represented by the item.
     *      @param [in] library     The IP-XACT library in use.
     *      @param [in] parent      The parent object.
     */
    BusPortItem(QSharedPointer<BusInterface> busIf, LibraryInterface* library, HWComponentItem* parent);

	/*!
     *  The destructor.
     */
	virtual ~BusPortItem();

	/*!
	 *  Get the graphics item type.
	 *
     *      @return The item type.
	 */
	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

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
     *  Returns the encompassing component.
     */
    virtual ComponentItem* encompassingComp() const;

	/*!
     *  Returns pointer to the top component that owns this interface.
	 *	 
	 *      @return The component to which this interface belongs to.
     */
	virtual QSharedPointer<Component> getOwnerComponent() const;

    /*!
     *  Returns true if the port represents a hierarchical connection
     */
    virtual bool isHierarchical() const;

    /*!
     *  Sets the endpoint temporary or not temporary. Temporary endpoints can be deleted.
     *
     *      @param [in] temp True if temporary; false if not temporary.
     */
    virtual void setTemporary(bool temp);

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

	/*!
	 *  Set the position of the name label.
	 */
	virtual void setLabelPosition();

	/*!
	 *  Check the direction of the port and change it if necessary.
	 */
	void checkDirection();

	/*!
	 *  Return the correct length of the name label.
	 */
	qreal getNameLength();

	/*!
	 *  Shorten the name label to better fit the component.
	 *  
	 *      @param [in] width   The width of the shortened name.
	 */
	void shortenNameLabel( qreal width );

protected:

    /*!
     *  Notifies item state changes.
     *
     *      @param [in] change  The change.
     *      @param [in] value   The new value.
     */
    virtual QVariant itemChange(GraphicsItemChange change, QVariant const& value);

    /*!
     *  Event for mouse press.
     *
     *      @param [in] event   The pressed mouse button.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Event for mouse move.
     *
     *      @param [in] event   The movement event.
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /*!
     *  Event for mouse button release.
     *
     *      @param [in] event   The release event.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    
    /*!
     *  Update the name change to the relevant contained variables.
     *
     *      @param [in] previousName    The previous name of the bus interface.
     *      @param [in] newName         The new name of the bus interface.
     */
    virtual void updateName(QString const& previousName, QString const& newName);

    /*!
     *  Finds the possible opposing modes for a bus interface.
     *
     *      @param [in] busIf   The interface for which the opposing mode is searched.
     *    
     *      @return The possible opposing modes.
     */
    QList<General::InterfaceMode> getOpposingModes(QSharedPointer<BusInterface> busIf) const;

    /*!
     *  Get the polygon shape of the in direction.
     *
     *      @return The in direction shape of the interface.
     */
    virtual QPolygonF getDirectionInShape() const;

    /*!
     *  Get the polygon shape of the out direction.
     *
     *      @return The out direction shape of the interface.
     */
    virtual QPolygonF getDirectionOutShape() const;

    /*!
     *  Get the polygon shape of the in/out direction.
     *
     *      @return The in/out direction shape of the interface.
     */
    virtual QPolygonF getDirectionInOutShape() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component item containing the bus port item.
    HWComponentItem* parentComponentItem_;

	//! The library interface.
    LibraryInterface* library_;
};

#endif // BUSPORTITEM_H
