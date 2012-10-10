//-----------------------------------------------------------------------------
// File: IHGraphicsLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.10.2012
//
// Description:
// Declares the vertical graphics layout interface.
//-----------------------------------------------------------------------------

#ifndef IHGRAPHICSLAYOUT_H
#define IHGRAPHICSLAYOUT_H

#include <QList>

//-----------------------------------------------------------------------------
//! Vertical graphics layout interface.
//-----------------------------------------------------------------------------
template <class T>
class IHGraphicsLayout
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IHGraphicsLayout() {}

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
    virtual void updateItemMove(QList<T*>& items, T* item, qreal minX = 0.0) = 0;

    /*!
     *  Sets the position of an item based on its index in the list using horizontal stacking.
     *
     *      @param [in] items    The list of items. Assumed to be already in correct order.
     *      @param [in] item     The item to position.
     *      @param [in] y        The y coordinate for the item's position.
     *      @param [in] minX     The minimum x coordinate.
     */
    virtual void setItemPos(QList<T*> const& items, T* item, qreal y, qreal minX = 0.0) = 0;

    /*!
     *  Updates the positions of all items using horizontal stacking.
     *
     *      @param [in] items    The list of items.
     *      @param [in] x        The y coordinate for the items.
     *      @param [in] minX     The minimum x coordinate.
     */
    virtual void updateItemPositions(QList<T*>& items, qreal y, qreal minX = 0.0) = 0;
};

//-----------------------------------------------------------------------------

#endif // IHGRAPHICSLAYOUT_H
