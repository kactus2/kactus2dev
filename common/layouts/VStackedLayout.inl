//-----------------------------------------------------------------------------
// File: VStackedLayout.inl
//-----------------------------------------------------------------------------
// Project: Endpoint Designer
// Author: Joni-Matti M‰‰tt‰
// Date: 22.4.2011
//
// Description:
// Vertical stacked layout
//-----------------------------------------------------------------------------

#include <limits>
#include <complex>

namespace VStackedLayout
{
    //-----------------------------------------------------------------------------
    // Function: updateItemMove()
    //-----------------------------------------------------------------------------
    template <class T>
    void updateItemMove(QList<T*>& items, T* item, qreal minY, qreal spacing)
    {
        using DiagramUtil::clampCeil;

        // Move the item to the last position (this simplifies the for loop).
        items.removeAt(items.indexOf(item));
        items.append(item);

        // Determine the best position for the item (position where the item is closest to
        // its resulting stacked position).
        qreal refY = item->pos().y() + item->boundingRect().top();
        qreal dist = std::numeric_limits<double>::max();
        qreal y = minY;
        int index = 0;

        for (int i = 0; i < items.size(); ++i)
        {
            qreal newDist = std::fabs(y - refY);

            // We can stop when the distance begins to grow.
            if (newDist > dist)
            {
                break;
            }

            dist = newDist;
            index = i;

            // Otherwise increment the bookkeeping x.
            y += clampCeil(items.at(index)->boundingRect().height(), GridSize) + spacing;
        }

        // Change the item's position in the list.
        items.removeAt(items.indexOf(item));
        items.insert(index, item);

        // Move the other columns accordingly.
        y = minY;

        for (int i = 0; i < items.size(); ++i)
        {
            if (i != index)
            {
                items.at(i)->setY(y + clampCeil(-items.at(i)->boundingRect().top(), GridSize));
            }

            y += clampCeil(items.at(i)->boundingRect().height(), GridSize) + spacing;
        }
    }

    //-----------------------------------------------------------------------------
    // Function: setItemPos()
    //-----------------------------------------------------------------------------
    template <class T>
    void setItemPos(QList<T*> const& items, T* item, qreal x, qreal minY, qreal spacing)
    {
        using DiagramUtil::clampCeil;

        int index = items.indexOf(item);
        qreal y = minY;

        for (int i = 0; i < index; ++i)
        {
            y += clampCeil(items.at(i)->boundingRect().height(), GridSize) + spacing;
        }

        item->setPos(x, y + clampCeil(-item->boundingRect().top(), GridSize));
        //item->setPos(x, std::max(item->y(), minY - item->boundingRect().top()));
    }

    //-----------------------------------------------------------------------------
    // Function: updateItemPositions()
    //-----------------------------------------------------------------------------
    template <class T>
    void updateItemPositions(QList<T*>& items, qreal x, qreal minY, qreal spacing)
    {
        using DiagramUtil::clampCeil;

        qreal y = minY;

        for (int i = 0; i < items.size(); ++i)
        {
            items.at(i)->setPos(x, y + clampCeil(-items.at(i)->boundingRect().top(), GridSize));
            y += clampCeil(items.at(i)->boundingRect().height(), GridSize) + spacing;
        }
    }
}
