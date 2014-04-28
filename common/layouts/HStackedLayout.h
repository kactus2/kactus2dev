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

#include "IHGraphicsLayout.h"

#include <designEditors/common/DiagramUtil.h>

//-----------------------------------------------------------------------------
//! Horizontal stacked layout.
//-----------------------------------------------------------------------------
template <class T>
class HStackedLayout : public IHGraphicsLayout<T>
{
public:
    /*!
     *  Constructor.
     *  
     *  @param [in] spacing The spacing.
     */
    HStackedLayout(qreal spacing);

    /*!
     *  Destructor.
     */
    ~HStackedLayout();

    /*!
     *  Updates the horizontal stacking of the items when one item is being moved.
     *
     *      @param [in] items    The list of items.
     *      @param [in] item     The item that is being moved.
     *      @param [in] minX     The minimum x coordinate.
     *
     *      @remarks The list of items is assumed to initially have correct left-right ordering and
     *               positioning.
     */
    void updateItemMove(QList<T*>& items, T* item, qreal minX = 0.0);

    /*!
     *  Sets the position of an item based on its index in the list using horizontal stacking.
     *
     *      @param [in] items    The list of items. Assumed to be already in correct order.
     *      @param [in] item     The item to position.
     *      @param [in] y        The y coordinate for the item's position.
     *      @param [in] minX     The minimum x coordinate.
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

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The spacing between items.
    qreal spacing_;
};

//-----------------------------------------------------------------------------

#include "HStackedLayout.inl"

#endif // HSTACKEDLAYOUT_H
