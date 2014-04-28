//-----------------------------------------------------------------------------
// File: DiagramUtil.inl
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.3.2011
//
// Description:
// Common diagram utility functions.
//-----------------------------------------------------------------------------

#include "diagramgrid.h"

namespace DiagramUtil
{
    //-----------------------------------------------------------------------------
    // Function: getSqDistance()
    //-----------------------------------------------------------------------------
    qreal getSqDistance(QPointF const& pt, QRectF const& rect)
    {
        qreal sqDist = 0.0;

        // Determine the distance component in x direction.
        if (pt.x() < rect.left())
        {
            qreal delta = (rect.left() - pt.x());
            sqDist += delta * delta;
        }
        else if (pt.x() > rect.right())
        {
            qreal delta = (pt.x() - rect.right());
            sqDist += delta * delta;
        }

        // Determine the distance component in y direction.
        if (pt.y() < rect.top())
        {
            qreal delta = (rect.top() - pt.y());
            sqDist += delta * delta;
        }
        else if (pt.y() > rect.bottom())
        {
            qreal delta = (pt.y() - rect.bottom());
            sqDist += delta * delta;
        }

        return sqDist;
    }

    //-----------------------------------------------------------------------------
    // Function: snapToItem()
    //-----------------------------------------------------------------------------
    template <class T>
    T* snapToItem(QPointF const& pos, QGraphicsScene const* scene, qreal snapDistance)
    {
        qreal sqSnapDistance = snapDistance * snapDistance;
        T* closest = 0;
        qreal closestDist = 0.0;

        // Go through all items that are of type T.
        foreach(QGraphicsItem* item, scene->items())
        {
            T* obj = dynamic_cast<T*>(item);

            if (obj != 0)
            {
                // Calculate the distance from the point to the bounding rectangle.
                qreal dist = getSqDistance(pos, obj->sceneBoundingRect());

                // Check if the item was close enough and if it is the closest one up to this point.
                if (dist <= sqSnapDistance && (closest == 0 || dist < closestDist))
                {
                    closest = obj;
                    closestDist = dist;
                }
            }
        }

        return closest;
    }

    //-----------------------------------------------------------------------------
    // Function: clampCeil()
    //-----------------------------------------------------------------------------
    int clampCeil(int value, int resolution)
    {
        bool negative = value < 0;

        value = abs(value);

        // Check if the value needs to be clamped.
        if (value % resolution != 0)
        {
            value = ((value / resolution) + 1) * resolution;
        }

        if (negative)
        {
            return -value;
        }
        else
        {
            return value;
        }
    }
}
