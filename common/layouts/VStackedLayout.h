//-----------------------------------------------------------------------------
// File: VStackedLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.4.2011
//
// Description:
// Vertical stacked layout.
//-----------------------------------------------------------------------------

#ifndef VSTACKEDLAYOUT_H
#define VSTACKEDLAYOUT_H

#include <common/DiagramUtil.h>

//-----------------------------------------------------------------------------

namespace VStackedLayout
{
    /*!
     *  Updates the vertical stacking of the items when one item is being moved.
     *
     *      @param [in] items    The list of items.
     *      @param [in] item     The item that is being moved.
     *      @param [in] minY     The minimum y coordinate.
     *      @param [in] spacing  The spacing between the items.
     *
     *      @remarks The list of items is assumed to initially have correct left-right ordering and
     *               positioning.
     */
    template <class T>
    void updateItemMove(QList<T*>& items, T* item, qreal minY = 0.0, qreal spacing = 0.0);

    /*!
     *  Sets the position of an item based on its index in the list using vertical stacking.
     *
     *      @param [in] items    The list of items. Assumed to be already in correct order.
     *      @param [in] item     The item to position.
     *      @param [in] x        The x coordinate for the item's position.
     *      @param [in] minY     The minimum y coordinate.
     *      @param [in] spacing  The spacing between the items.
    */
    template <class T>
    void setItemPos(QList<T*> const& items, T* item, qreal x, qreal minY = 0.0, qreal spacing = 0.0);        

    /*!
     *  Updates the positions of all items using vertical stacking.
     *
     *      @param [in] items    The list of items.
     *      @param [in] x        The x coordinate for the items.
     *      @param [in] minY     The minimum y coordinate.
     *      @param [in] spacing  The spacing between the items.
     */
    template <class T>
    void updateItemPositions(QList<T*>& items, qreal x, qreal minY = 0.0, qreal spacing = 0.0);
}

//-----------------------------------------------------------------------------

#include "VStackedLayout.inl"

#endif // VSTACKEDLAYOUT_H
