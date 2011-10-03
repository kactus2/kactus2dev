//-----------------------------------------------------------------------------
// File: HStackedLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.4.2011
//
// Description:
// Horizontal stacked layout.
//-----------------------------------------------------------------------------

#ifndef HSTACKEDLAYOUT_H
#define HSTACKEDLAYOUT_H

#include <common/DiagramUtil.h>

//-----------------------------------------------------------------------------

namespace HStackedLayout
{
    /*!
     *  Updates the horizontal stacking of the items when one item is being moved.
     *
     *      @param [in] items    The list of items.
     *      @param [in] item     The item that is being moved.
     *      @param [in] minX     The minimum x coordinate.
     *      @param [in] spacing  The spacing between the items.
     *
     *      @remarks The list of items is assumed to initially have correct left-right ordering and
     *               positioning.
     */
    template <class T>
    void updateItemMove(QList<T*>& items, T* item, qreal minX = 0.0, qreal spacing = 0.0);

    /*!
     *  Sets the position of an item based on its index in the list using horizontal stacking.
     *
     *      @param [in] items    The list of items. Assumed to be already in correct order.
     *      @param [in] item     The item to position.
     *      @param [in] y        The y coordinate for the item's position.
     *      @param [in] minX     The minimum x coordinate.
     *      @param [in] spacing  The spacing between the items.
     */
    template <class T>
    void setItemPos(QList<T*> const& items, T* item, qreal y, qreal minX = 0.0, qreal spacing = 0.0);

    /*!
     *  Updates the positions of all items using vertical stacking.
     *
     *      @param [in] items    The list of items.
     *      @param [in] x        The y coordinate for the items.
     *      @param [in] minX     The minimum x coordinate.
     *      @param [in] spacing  The spacing between the items.
     */
    template <class T>
    void updateItemPositions(QList<T*>& items, qreal y, qreal minX = 0.0, qreal spacing = 0.0);
}

//-----------------------------------------------------------------------------

#include "HStackedLayout.inl"

#endif // HSTACKEDLAYOUT_H
