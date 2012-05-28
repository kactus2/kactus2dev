//-----------------------------------------------------------------------------
// File: IGraphicsItemStack.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.4.2012
//
// Description:
// Interface for managing a component stack.
//-----------------------------------------------------------------------------

#ifndef IGRAPHICSITEMSTACK_H
#define IGRAPHICSITEMSTACK_H

#include <models/ColumnDesc.h>

#include <QPointF>
#include <QGraphicsItem>

//-----------------------------------------------------------------------------
//! Interface for managing a component stack.
//-----------------------------------------------------------------------------
class IGraphicsItemStack
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IGraphicsItemStack() {}

    /*!
     *  Adds an item to stack.
     *
     *      @param [in] item  The item to add.
     *      @param [in] load  If true, the item is being loaded from a design.
     */
    virtual void addItem(QGraphicsItem* item, bool load = false) = 0;

    /*!
     *  Removes an item from the stack.
     *
     *      @param [in] item the item to remove.
     */
    virtual void removeItem(QGraphicsItem* item) = 0;

    /*!
     *  Called when an item is moved within the stack.
     *
     *      @param [in] item     The item that has been moved.
     */
    virtual void onMoveItem(QGraphicsItem* item) = 0;

    /*!
     *  Called when an item is released from being moved by mouse.
     *
     *      @param [in] item The item that has been released.
     */
    virtual void onReleaseItem(QGraphicsItem* item) = 0;

    /*!
     *  Updates the item positions so that there are no violations of the stacking rule.
     */
    virtual void updateItemPositions() = 0;

    /*!
     *  Maps the given local position to scene coordinates.
     */
    virtual QPointF mapStackToScene(QPointF const& pos) const = 0;

    /*!
     *  Maps the given scene position to local coordinates.
     */
    virtual QPointF mapStackFromScene(QPointF const& pos) const = 0;

    /*!
     *  Returns true if the stack is allowed to contain the given item.
     *
     *      @param [in] item The item to test for.
     */
    virtual bool isItemAllowed(QGraphicsItem* item) const = 0;

    /*!
     *  Returns the content type.
     */
    virtual ColumnContentType getContentType() const = 0;
};

//-----------------------------------------------------------------------------

#endif // IGRAPHICSITEMSTACK_H
