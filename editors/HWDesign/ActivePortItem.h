//-----------------------------------------------------------------------------
// File: ActivePortItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#ifndef ACTIVEPORTITEM_H
#define ACTIVEPORTITEM_H

#include <QSharedPointer>
#include <QVector2D>

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <editors/HWDesign/AdHocItem.h>

#include <QPolygonF>

class HWComponentItem;
class Port;

//-----------------------------------------------------------------------------
//! ActivePortItem class.
//-----------------------------------------------------------------------------
class ActivePortItem : public AdHocItem
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
    ActivePortItem(QSharedPointer<Port> port, HWComponentItem* parent);

	/*!
     *  The destructor.
     */
	virtual ~ActivePortItem();

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
     *  Check if the port is hierarchical.
     *
     *      @return False, port items are always instance items and thus non-hierarchical.
     */
    virtual bool isHierarchical() const;

	/*!
	 *  Set the position of the name label.
	 */
	virtual void setLabelPosition();

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

protected:

    /*!
     *  Notifies item state changes.
     *
     *      @param [in] change  The change.
     *      @param [in] value   The new value.
     */
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /*!
     *  Event for mouse button release.
     *
     *      @param [in] event   The release event.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    /*!
     *  Check whether the label should be drawn on the left side of the interface.
     *
     *      @return True, if the label should be drawn to the left side.
     */
    virtual bool labelShouldBeDrawnLeft() const;

    /*!
     *  Move the ad hoc item by dragging with the mouse.
     */
    virtual void moveItemByMouse();

    /*!
     *  Save the positions of the other associated ports.
     */
    virtual void saveOldPortPositions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<ConnectionEndpoint*, QPointF> oldPortPositions_;
};

//-----------------------------------------------------------------------------

#endif // ACTIVEPORTITEM_H
