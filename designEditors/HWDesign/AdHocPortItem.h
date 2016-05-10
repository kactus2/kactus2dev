//-----------------------------------------------------------------------------
// File: AdHocPortItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCPORTITEM_H
#define ADHOCPORTITEM_H

#include <QSharedPointer>
#include <QVector2D>

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <designEditors/HWDesign/AdHocItem.h>

#include <QPolygonF>

class HWComponentItem;
class Port;

//-----------------------------------------------------------------------------
//! AdHocPortItem class.
//-----------------------------------------------------------------------------
class AdHocPortItem : public AdHocItem
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_ADHOC_PORT };

    /*!
     *  The constructor.
     *
     *      @param [in] port    The selected port.
     *      @param [in] parent  The owner of this item.
     */
    AdHocPortItem(QSharedPointer<Port> port, HWComponentItem* parent);

	/*!
     *  The destructor.
     */
	virtual ~AdHocPortItem();

    /*!
     *  Updates the graphics to match the IP-XACT port.
     */
    void updateInterface();

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
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

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
     *  Returns the encompassing component. if this port represents a bus interface on a component.
     */
    virtual ComponentItem* encompassingComp() const;

	/*!
     *  Returns a pointer to the component referenced by the instance that owns this item.
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const;

    /*!
     *  Check if the port is hierarchical.
     *
     *      @return False, port items are always instance items and thus non-hierarchical.
     */
    virtual bool isHierarchical() const;

	/*!
	 *  Set the position of the name label.
	 */
	void setLabelPosition();

	/*!
	 *  Check the direction of the port and change it if necessary.
	 */
	void checkDirection();

	/*!
	 *  Gives the length of the text in the name label.
	 */
	qreal getNameLength();
	
	/*!
	 *  Shortens the name label to better fit in the component.
	 *
	 *      @param [in] width   The width of the shortened name.
	 */
    void shortenNameLabel(qreal width);

    /*!
     *  Check if an ad hoc port exists in the containing component.
     *
     *      @return True, if the port exists in the containing component, otherwise false.
     */
    bool adHocPortExists() const;

protected:

    /*!
     *  Notifies item state changes.
     *
     *      @param [in] change  The change.
     *      @param [in] value   The new value.
     */
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

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

    //! The component item containing this port item.
    HWComponentItem* parentComponentItem_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<ConnectionEndpoint*, QPointF> oldPortPositions_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCPORTITEM_H
