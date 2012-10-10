//-----------------------------------------------------------------------------
// File: IVGraphicsLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.10.2012
//
// Description:
// Declares the vertical graphics layout interface.
//-----------------------------------------------------------------------------

#ifndef IVGRAPHICSLAYOUT_H
#define IVGRAPHICSLAYOUT_H

#include <QList>

//-----------------------------------------------------------------------------
//! Vertical graphics layout interface.
//-----------------------------------------------------------------------------
template <class T>
class IVGraphicsLayout
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IVGraphicsLayout() {}

    /*!
     *  Updates the vertical stacking of the items when one item is being moved.
     *
     *      @param [in] items    The list of items.
     *      @param [in] item     The item that is being moved.
     *      @param [in] minY     The minimum y coordinate.
     *
     *      @remarks The list of items is assumed to initially have correct left-right ordering and
     *               positioning.
     */
    virtual void updateItemMove(QList<T*>& items, T* item, qreal minY = 0.0) = 0;

    /*!
     *  Sets the position of an item based on its index in the list using vertical stacking.
     *
     *      @param [in] items    The list of items. Assumed to be already in correct order.
     *      @param [in] item     The item to position.
     *      @param [in] x        The x coordinate for the item's position.
     *      @param [in] minY     The minimum y coordinate.
     */
    virtual void setItemPos(QList<T*> const& items, T* item, qreal x, qreal minY = 0.0) = 0;

    /*!
     *  Updates the positions of all items using vertical stacking.
     *
     *      @param [in] items    The list of items.
     *      @param [in] x        The x coordinate for the items.
     *      @param [in] minY     The minimum y coordinate.
     */
    virtual void updateItemPositions(QList<T*>& items, qreal x, qreal minY = 0.0) = 0;
};

//-----------------------------------------------------------------------------

#endif // IVGRAPHICSLAYOUT_H
