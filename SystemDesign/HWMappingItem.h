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

#include "IComponentStack.h"

#include <common/graphicsItems/ComponentItem.h>

class SWCompItem;

//-----------------------------------------------------------------------------
//! Graphics item for visualizing the underlying HW in system designs.
//-----------------------------------------------------------------------------
class HWMappingItem : public ComponentItem, public IComponentStack
{
public:
    enum { Type = UserType + 30 };

    /*!
     *  Constructor.
     */
    HWMappingItem(QSharedPointer<Component> component, QString const& instanceName,
                  QString const& displayName = QString(),
                  QString const& description = QString(),
                  QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                  unsigned int id = 0);

    /*!
     *  Destructor.
     */
    ~HWMappingItem();

    /*!
     *  Maps the given SW component item to this HW.
     *
     *      @param [in] item The SW component to map.
     */
    void mapComponent(SWCompItem* item);

    /*!
     *  Removes a mapping of the given SW component item from this HW.
     *
     *      @param [in] item The SW component to unmap.
     */
    void unmapComponent(SWCompItem* item);

    void updateSize();

    /*!
     *  Returns the graphics item type.
     */
    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // IComponentStack implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Adds an item to the system column.
     *
     *      @param [in] item  The item to add.
     *      @param [in] load  If true, the item is being loaded from a design.
     */
    void addItem(ComponentItem* item, bool load = false);

    /*!
     *  Removes an item from the system column.
     *
     *      @param [in] item the item to remove.
     */
    void removeItem(ComponentItem* item);

    /*!
     *  Called when an item is moved within the column.
     *
     *      @param [in] item       The item that has been moved.
     */
    void onMoveItem(ComponentItem* item);

    /*!
     *  Called when an item is released from being moved by mouse.
     *
     *      @param [in] item The item that has been released.
     */
    void onReleaseItem(ComponentItem* item);

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
    bool isItemAllowed(ComponentItem* item) const;

protected:
    // Called when the user presses the mouse button.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

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
        WIDTH = 240,
        TOP_MARGIN = 40,
        BOTTOM_MARGIN = 60,
        SPACING = 10,
    };

    //! The node id.
    unsigned int id_;

    //! The old column from where the mouse drag event began.
    IComponentStack* oldStack_;

    //! The mapped SW components.
    QList<ComponentItem*> swComponents_;

    //! The mapping component's old position before mouse move.
    QPointF oldPos_;
};

//-----------------------------------------------------------------------------

#endif // HWMAPPINGITEM_H
