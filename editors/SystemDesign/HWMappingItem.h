//-----------------------------------------------------------------------------
// File: HWMappingItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing the underlying HW in system designs.
//-----------------------------------------------------------------------------

#ifndef HWMAPPINGITEM_H
#define HWMAPPINGITEM_H

#include "SystemComponentItem.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/IGraphicsItemStack.h>
#include <common/layouts/IVGraphicsLayout.h>
#include <common/layouts/VStackedLayout.h>

class SWComponentItem;

//-----------------------------------------------------------------------------
//! Graphics item for visualizing the underlying HW in system designs.
//-----------------------------------------------------------------------------
class HWMappingItem : public SystemComponentItem, public IGraphicsItemStack
{
public:
    enum { Type = GFX_TYPE_HW_MAPPING_ITEM };

    /*!
     *  The constructor.
     *
     *    @param [in] libInterface                The library interface.
     *    @param [in] component                   The contained component.
     *    @param [in] instanceName                The instance name.
     *    @param [in] displayName                 The instance display name.
     *    @param [in] description                 The instance description.
     *    @param [in] uuid                        The instance id.
     *    @param [in] configurableElementValues   The configurable element values.
     */
	HWMappingItem(LibraryInterface* libInterface, QSharedPointer<Component> component,
		QSharedPointer<ComponentInstance> instance);

    /*!
     *  Destructor.
     */
    ~HWMappingItem() final;

    // Disable copying.
    HWMappingItem(HWMappingItem const& rhs) = delete;
    HWMappingItem& operator=(HWMappingItem const& rhs) = delete;

    /*!
     *  Returns the underlying HW linked with this component.
     */
    virtual HWMappingItem const* getLinkedHW() const;

    /*!
     *  Returns the graphics item type.
     */
    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // IGraphicsItemStack implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Adds an item to the system column.
     *
     *    @param [in] item  The item to add.
     *    @param [in] load  If true, the item is being loaded from a design.
     */
    void addItem(QGraphicsItem* item, bool load = false);

    /*!
     *  Removes an item from the system column.
     *
     *    @param [in] item the item to remove.
     */
    void removeItem(QGraphicsItem* item);

    /*!
     *  Called when an item is moved within the column.
     *
     *    @param [in] item       The item that has been moved.
     */
    void onMoveItem(QGraphicsItem* item);

    /*!
     *  Called when an item is released from being moved by mouse.
     *
     *    @param [in] item The item that has been released.
     */
    void onReleaseItem(QGraphicsItem* item);

    /*!
     *  Updates the item positions so that there are no violations of the stacking rule.
     */
    void updateItemPositions();

    /*!
     *  Maps the given local position to scene coordinates.
     */
    QPointF mapStackToScene(QPointF const& pos) const;

    /*!
     *  Maps the given scene position to local coordinates.
     */
    QPointF mapStackFromScene(QPointF const& pos) const;

    /*!
     *  Returns true if the stack is allowed to contain the given item.
     *
     *    @param [in] item The item to test for.
     */
    bool isItemAllowed(QGraphicsItem* item) const;

    /*!
     *  Returns the content type.
     */
    ColumnTypes::ColumnContentType getContentType() const;

    virtual void updateComponent();

protected:
    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    /*!
     *  Returns the height for the component box.
     */
    qreal getHeight() final;

    /*!
     *  Return the width for the component box.
     */
    qreal getWidth() noexcept final;

    /*!
     *  Returns the height of the component stack.
     */
    qreal getComponentStackHeight() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // Constants.
    static constexpr int MIN_HEIGHT = 100;
    static constexpr int TOP_MARGIN = 40;

    static constexpr int WIDTH = COMPONENTWIDTH + 20;

    //! The old column from where the mouse drag event began.
    IGraphicsItemStack* oldStack_= nullptr;

    //! The layout for components.
    QSharedPointer< IVGraphicsLayout<ComponentItem> > layout_ =
        QSharedPointer< IVGraphicsLayout<ComponentItem> >(new VStackedLayout<ComponentItem>(SPACING));

    //! The mapped SW components.
    QList<ComponentItem*> swComponents_;

    //! The mapping component's old position before mouse move.
    QPointF oldPos_;

};

//-----------------------------------------------------------------------------

#endif // HWMAPPINGITEM_H
