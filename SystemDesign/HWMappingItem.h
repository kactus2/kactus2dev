//-----------------------------------------------------------------------------
// File: HWMappingItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing the underlying HW in system designs.
//-----------------------------------------------------------------------------

#ifndef HWMAPPINGITEM_H
#define HWMAPPINGITEM_H

#include "SystemComponentItem.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/IGraphicsItemStack.h>
#include <common/layouts/IVGraphicsLayout.h>

class SWComponentItem;

//-----------------------------------------------------------------------------
//! Graphics item for visualizing the underlying HW in system designs.
//-----------------------------------------------------------------------------
class HWMappingItem : public SystemComponentItem, public IGraphicsItemStack
{
public:
    enum { Type = GFX_TYPE_HW_MAPPING_ITEM };

    /*!
     *  Constructor.
     */
    HWMappingItem(LibraryInterface* libInterface,
                  QSharedPointer<Component> component,
                  QString const& instanceName,
                  QString const& displayName = QString(),
                  QString const& description = QString(),
                  QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>());

    /*!
     *  Destructor.
     */
    ~HWMappingItem();

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
     *      @param [in] item  The item to add.
     *      @param [in] load  If true, the item is being loaded from a design.
     */
    void addItem(QGraphicsItem* item, bool load = false);

    /*!
     *  Removes an item from the system column.
     *
     *      @param [in] item the item to remove.
     */
    void removeItem(QGraphicsItem* item);

    /*!
     *  Called when an item is moved within the column.
     *
     *      @param [in] item       The item that has been moved.
     */
    void onMoveItem(QGraphicsItem* item);

    /*!
     *  Called when an item is released from being moved by mouse.
     *
     *      @param [in] item The item that has been released.
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
     *      @param [in] item The item to test for.
     */
    bool isItemAllowed(QGraphicsItem* item) const;

    /*!
     *  Returns the content type.
     */
    ColumnContentType getContentType() const;

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
    qreal getHeight() const;

    /*!
     *  Returns the height of the component stack.
     */
    qreal getComponentStackHeight() const;

private:
    // Disable copying.
    HWMappingItem(HWMappingItem const& rhs);
    HWMappingItem& operator=(HWMappingItem const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // Constants.
    enum
    {
        MIN_HEIGHT = 100,
        WIDTH = 180,
        TOP_MARGIN = 40,
        BOTTOM_MARGIN = 20,
        SPACING = 10,
    };

    //! The old column from where the mouse drag event began.
    IGraphicsItemStack* oldStack_;

    //! The layout for components.
    QSharedPointer< IVGraphicsLayout<ComponentItem> > layout_;

    //! The mapped SW components.
    QList<ComponentItem*> swComponents_;

    //! The mapping component's old position before mouse move.
    QPointF oldPos_;
};

//-----------------------------------------------------------------------------

#endif // HWMAPPINGITEM_H
