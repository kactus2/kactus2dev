//-----------------------------------------------------------------------------
// File: VNoCollisionNoMovementLayout.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.04.2025
//
// Description:
// Vertical layout. No collision and no movement allowed. Used in memory design columns.
//-----------------------------------------------------------------------------

#ifndef VNOCOLLISIONNOMOVEMENTLAYOUT_H
#define VNOCOLLISIONNOMOVEMENTLAYOUT_H

#include "IVGraphicsLayout.h"

//-----------------------------------------------------------------------------
//! Vertical collision layout.
//-----------------------------------------------------------------------------
template <class T>
class VNoCollisionNoMovementLayout : public IVGraphicsLayout<T>
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] spacing The spacing between items.
     */
    VNoCollisionNoMovementLayout(qreal spacing = 0.0);

    /*!
     *  Destructor.
     */
    ~VNoCollisionNoMovementLayout();

    /*!
     *  Updates the positions of the items when one item is being moved.
     *
     *    @param [in] items    The list of items.
     *    @param [in] item     The item that is being moved.
     *    @param [in] minY     The minimum y coordinate.
     *
     *      @remarks The list of items is assumed to initially have correct top-bottom
     *               ordering and positioning.
     */
    void updateItemMove(QList<T*>& items, T* item, qreal minY = 0.0);

    /*!
     *  Sets the position of an item according to the collision-based layout.
     *
     *    @param [in] items    The list of items. Assumed to be already in correct order.
     *    @param [in] item     The item to position.
     *    @param [in] x        The x coordinate for the item's position.
     *    @param [in] minY     The minimum y coordinate.
     */
    void setItemPos(QList<T*> const& items, T* item, qreal x, qreal minY = 0.0);

    /*!
     *  Updates the positions of all items using vertical stacking.
     *
     *    @param [in] items    The list of items.
     *    @param [in] x        The x coordinate for the items.
     *    @param [in] minY     The minimum y coordinate.
     */
    void updateItemPositions(QList<T*>& items, qreal x, qreal minY = 0.0);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The spacing between items.
    qreal spacing_;
};

//-----------------------------------------------------------------------------

#include "VNoCollisionNoMovementLayout.inl"

#endif // VNOCOLLISIONNOMOVEMENTLAYOUT_H
