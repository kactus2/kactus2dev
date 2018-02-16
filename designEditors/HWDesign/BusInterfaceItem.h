//-----------------------------------------------------------------------------
// File: BusInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.3.2011
//
// Description:
// Diagram interface graphics item.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEITEM_H
#define BUSINTERFACEITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <designEditors/HWDesign/BusInterfaceEndPoint.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <QGraphicsPolygonItem>
#include <QSharedPointer>
#include <QVector2D>

class Port;
class BusInterface;
class OffPageConnectorItem;
class HWComponentItem;
class HWColumn;
class Component;
class LibraryInterface;
class InterfaceGraphicsData;

//-----------------------------------------------------------------------------
//! BusInterfaceItem class.
//-----------------------------------------------------------------------------
class BusInterfaceItem : public BusInterfaceEndPoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_INTERFACE };

    /*!
     *  The constructor.
     *
     *      @param [in] component   The component containing the bus interface this item represents.
     *      @param [in] busIf       The bus interface this item represents.
     *      @param [in] dataGroup   The container for the item data.
     *      @param [in] parent      The parent object.
     */
    BusInterfaceItem(QSharedPointer<Component> component,
                     QSharedPointer<BusInterface> busIf,
                     QSharedPointer<InterfaceGraphicsData> dataGroup,
                     QGraphicsItem *parent = 0);

	//! The destructor
	virtual ~BusInterfaceItem();

    /*!
     *  Defines the interface.
     *
     *      @param [in] busIf     The bus interface.
     *      @param [in] addPorts  If true, the given ports are added to the parent component.
     *      @param [in] ports     The related ports.
     */
    void define(QSharedPointer<BusInterface> busIf);

    /*!
     *  Returns the ports in the top-level component that are related to view.
     *
     *      @param [in] activeView  The currently active view.
     *
     *      @return List of ports related to the selected view.
     */
    QList<QSharedPointer<Port> > getPortsForView(QString const& activeView) const;

    /*!
     *  Get all of the ports mapped within the bus interface.
     *
     *      @return A list of ports mapped within the bus interface.
     */
    QList<QSharedPointer<Port> > getAllPorts() const;

    /*!
     *  Get the graphics item type of this item.
     *
     *      @return The graphics item type of this item.
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
     *      @remarks Does not take existing connections into account but simply validates whether a connection 
     *               between the endpoints would be valid in a general case.
     */
    virtual bool isConnectionValid(ConnectionEndpoint const* other) const;

    /*! 
     *  Returns the encompassing component. if this port represents a bus interface on a component.
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
     *  Set the direction for the bus interface item.
     *
     *      @param [in] dir     The new direction.
     */
    void setDirection(QVector2D const& dir);

	/*!
	 * Set the position of the name label.
	 */
	virtual void setLabelPosition();

    /*!
     *  Gets the data extension for the bus interface.
     *
     *      @return The data vendor extension.
     */
    QSharedPointer<VendorExtension> getDataExtension() const;

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
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    /*!
     *  Event for mouse button release.
     *
     *      @param [in] event   The release event.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);


private:

    /*!
     *  Update the name change to the relevant contained variables.
     *
     *      @param [in] previousName    The previous name of the bus interface.
     *      @param [in] newName         The new name of the bus interface.
     */
    virtual void updateName(QString const& previousName, QString const& newName);

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

    //! The top-level component.
    QSharedPointer<Component> component_;

    //! The data extension for the item.
    QSharedPointer<InterfaceGraphicsData> dataGroup_;

    //! The old column from where the mouse drag event began.
    HWColumn* oldColumn_;
};

#endif // BUSINTERFACEITEM_H
