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

#include "IHGraphicsLayout.h"

//-----------------------------------------------------------------------------

template <class T>
class HCollisionLayout : public IHGraphicsLayout<T>
{
    /*!
     *  Constructor.
     *  
     *      @param [in] spacing The spacing.
     */
    HCollisionLayout(qreal spacing);

    /*!
     *  Destructor.
     */
    ~HCollisionLayout();

    /*!
     *  Updates the positions of the items when one item is being moved.
     *
     *      @param [in] items    The list of items.
     *      @param [in] item     The item that is being moved.
     *      @param [in] minY     The minimum x coordinate.
     *
     *      @remarks The list of items is assumed to initially have correct top-bottom
     *               ordering and positioning.
     */
    void updateItemMove(QList<T*>& items, T* item, qreal minX = 0.0);

    /*!
     *  Sets the position of an item according to the collision-based layout.
     *
     *      @param [in] items    The list of items. Assumed to be already in correct order.
     *      @param [in] item     The item to position.
     *      @param [in] x        The y coordinate for the item's position.
     *      @param [in] minY     The minimum x coordinate.
     */
    void setItemPos(QList<T*> const& items, T* item, qreal y, qreal minX = 0.0);

    /*!
     *  Updates the positions of all items using horizontal stacking.
     *
     *      @param [in] items    The list of items.
     *      @param [in] x        The y coordinate for the items.
     *      @param [in] minX     The minimum x coordinate.
     */
    void updateItemPositions(QList<T*>& items, qreal y, qreal minX = 0.0);
};

//-----------------------------------------------------------------------------

#include "HCollisionLayout.inl"

#endif // HCOLLISIONLAYOUT_H
