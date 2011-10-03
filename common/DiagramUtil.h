//-----------------------------------------------------------------------------
// File: DiagramUtil.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.3.2011
//
// Description:
// Common diagram utility functions.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMUTIL_H
#define DIAGRAMUTIL_H

#include <QGraphicsScene>
#include <QRectF>
#include <QPointF>

//-----------------------------------------------------------------------------

namespace DiagramUtil
{
    /*!
     *  Calculates the squared distance from a point to a bounding rectangle.
     *
     *      @param [in] pt    The point.
     *      @param [in] rect  The bounding rectangle.
     *
     *      @return The squared distance.
     */
    inline qreal getSqDistance(QPointF const& pt, QRectF const& rect);

    /*!
     *  Snaps the given position to a scene item if it is close enough the position.
     *
     *      @param [in] pos           The position against which to snap.
     *      @param [in] scene         The scene whose items are investigated.
     *      @param [in] snapDistance  The snap distance.
     *
     *      @return The snapped item. If null, there was no item close enough
     *              to the given position.
     */
    template <class T>
    T* snapToItem(QPointF const& pos, QGraphicsScene const* scene, qreal snapDistance);

    /*!
     *  Clamps the given value to the closest ceil of the given resolution.
     *
     *      @param [in] value       The value to be clamped.
     *      @param [in] resolution  The clamping resolution.
     */
    inline int clampCeil(int value, int resolution);
}

//-----------------------------------------------------------------------------

#include "DiagramUtil.inl"

#endif // DIAGRAMUTIL_H
