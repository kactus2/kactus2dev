//-----------------------------------------------------------------------------
// File: HierarchicalPortItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICALPORTITEM_H
#define HIERARCHICALPORTITEM_H

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

#include <editors/HWDesign/AdHocItem.h>

#include <common/graphicsItems/GraphicsItemTypes.h>

class GraphicsColumn;
class Kactus2Placeholder;
class Port;

//-----------------------------------------------------------------------------
//! HierarchicalPortItem class.
//-----------------------------------------------------------------------------
class HierarchicalPortItem : public AdHocItem
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
    HierarchicalPortItem(QSharedPointer<Component> component, QSharedPointer<Port> port,
        QSharedPointer<Kactus2Placeholder> dataGroup, QGraphicsItem* parent = 0);

	/*!
     *  The destructor.
     */
	~HierarchicalPortItem() final = default;

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
     *  Returns true if the port represents a hierarchical connection.
     */
    bool isHierarchical() const noexcept final;

    /*!
     *  Set the direction for this ad hoc interface item.
     *
     *      @param [in] dir     The new item direction.
     */
    void setDirection(QVector2D const& dir);

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    bool isDirectionFixed() const noexcept final;

	/*!
	 *  Set the position of the name label.
	 */
	void setLabelPosition();
    
    /*!
     *  Get the visibility data group of the item.
     *
     *      @return The visibility data group of the item.
     */
    QSharedPointer<Kactus2Placeholder> getDataGroup() const;

    /*!
     *  Set a new visibility and position data group.
     *
     *      @param [in] newDataGroup    The new visibility and position data group.
     */
    void setDataGroup(QSharedPointer<Kactus2Placeholder> newDataGroup);

protected:

    /*!
     *  Notifies item state changes.
     *
     *      @param [in] change  The change.
     *      @param [in] value   The new value.
     */
    QVariant itemChange(GraphicsItemChange change, QVariant const& value) final;

    /*!
     *  Event for mouse button release.
     *
     *      @param [in] event   The release event.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) final;

private:

    /*!
     *  Check whether the label should be drawn on the left side of the interface.
     *
     *      @return True, if the label should be drawn to the left side.
     */
    bool labelShouldBeDrawnLeft() const final;

    /*!
     *  Move the ad hoc item by dragging with the mouse.
     */
    void moveItemByMouse() final;

    /*!
     *  Save the positions of the other associated ports.
     */
    void saveOldPortPositions() final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The visibility data group of the item.
    QSharedPointer<Kactus2Placeholder> dataGroup_ = nullptr;

    //! The old column from where the mouse drag event began.
    GraphicsColumn* oldColumn_ = nullptr;

    //! The old positions of the other interfaces before mouse move.
    QMap<QGraphicsItem*, QPointF> oldInterfacePositions_;

    //! The position of the port before mouse move.
    QPointF oldPos_;
};

//-----------------------------------------------------------------------------

#endif // HIERARCHICALPORTITEM_H
