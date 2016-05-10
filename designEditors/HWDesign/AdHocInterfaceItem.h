//-----------------------------------------------------------------------------
// File: AdHocInterfaceItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCINTERFACEITEM_H
#define ADHOCINTERFACEITEM_H

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

#include <designEditors/HWDesign/AdHocItem.h>

#include <common/graphicsItems/GraphicsItemTypes.h>

class GraphicsColumn;
class Kactus2Placeholder;
class Port;

//-----------------------------------------------------------------------------
//! AdHocInterfaceItem class.
//-----------------------------------------------------------------------------
class AdHocInterfaceItem : public AdHocItem
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_ADHOC_INTERFACE };

    /*!
     *  The constructor.
     *
     *      @param [in] component   The component containing the ad hoc port.
     *      @param [in] port        The selected port.
     *      @param [in] dataGroup   The visibility data group of the item.
     *      @param [in] parent      The owner of this item.
     */
    AdHocInterfaceItem(QSharedPointer<Component> component, QSharedPointer<Port> port,
        QSharedPointer<Kactus2Placeholder> dataGroup, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~AdHocInterfaceItem();

    /*!
     *  Updates the graphics to match the IP-XACT port.
     */
    virtual void updateInterface();

	/*!
	 *  Get the type of this graphics item.
	 *
     *      @return AdHoc interface item type.
	 */
	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // HWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

	/*!
     *  Sets the name of the ad-hoc port.
     *
     *      @param [in] name The name to set.
     */
	virtual void setName(const QString& name);

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
     *  Returns a pointer to the top component that owns this interface
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const;

    /*! 
     *  Returns true if the port represents a hierarchical connection.
     */
    virtual bool isHierarchical() const;

    /*!
     *  Set the direction for this ad hoc interface item.
     *
     *      @param [in] dir     The new item direction.
     */
    void setDirection(QVector2D const& dir);

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

	/*!
	 *  Set the position of the name label.
	 */
	void setLabelPosition();
    
    /*!
     *  Check if the ad hoc port is valid.
     *
     *      @return True, if the port is valid, otherwise false.
     */
    bool adhocPortIsValid() const;

    /*!
     *  Get the visibility data group of the item.
     *
     *      @return The visibility data group of the item.
     */
    QSharedPointer<Kactus2Placeholder> getDataGroup() const;

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
     *  Get the shape of an in port.
     *
     *      @param [in] squareSize  The size of a square in the design grid.
     *
     *      @return Polygon containing the shape of an in port.
     */
    virtual QPolygonF getInPortShape(const int squareSize) const;

    /*!
     *  Get the shape of an out port.
     *
     *      @param [in] squareSize  The size of a square in the design grid.
     *
     *      @return Polygon containing the shape of an out port.
     */
    virtual QPolygonF getOutPortShape(const int squareSize) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The top-level component.
    QSharedPointer<Component> component_;

    //! The visibility data group of the item.
    QSharedPointer<Kactus2Placeholder> dataGroup_;

    //! The old column from where the mouse drag event began.
    GraphicsColumn* oldColumn_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other interfaces before mouse move.
    QMap<QGraphicsItem*, QPointF> oldInterfacePositions_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCINTERFACEITEM_H
