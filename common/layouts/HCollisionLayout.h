//-----------------------------------------------------------------------------
// File: HCollisionLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.4.2011
//
// Description:
// Horizontal collision-based layout.
//-----------------------------------------------------------------------------

#ifndef HCOLLISIONLAYOUT_H
#define HCOLLISIONLAYOUT_H

//-----------------------------------------------------------------------------

namespace HCollisionLayout
{
    /*!
     *  Updates the positions of the items when one item is being moved.
     *
     *      @param [in] items    The list of items.
     *      @param [in] item     The item that is being moved.
     *      @param [in] minY     The minimum x coordinate.
     *      @param [in] spacing  The spacing between the items.
     *
     *      @remarks The list of items is assumed to initially have correct top-bottom
     *               ordering and positioning.
     */
    template <class T>
    void updateItemMove(QList<T*>& items, T* item, qreal minX = 0.0, qreal spacing = 0.0);

    /*!
     *  Sets the position of an item according to the collision-based layout.
     *
     *      @param [in] items    The list of items. Assumed to be already in correct order.
     *      @param [in] item     The item to position.
     *      @param [in] x        The y coordinate for the item's position.
     *      @param [in] minY     The minimum x coordinate.
     *      @param [in] spacing  The spacing between the items.
    */
    template <class T>
    void setItemPos(QList<T*> const& items, T* item, qreal y, qreal minX = 0.0, qreal spacing = 0.0);
}

//-----------------------------------------------------------------------------

#include "HCollisionLayout.inl"

#endif // HCOLLISIONLAYOUT_H
