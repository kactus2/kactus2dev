//-----------------------------------------------------------------------------
// File: VCollisionLayout.inl
//-----------------------------------------------------------------------------
// Project: Endpoint Designer
// Author: Joni-Matti M‰‰tt‰
// Date: 22.4.2011
//
// Description:
// Vertical collision-based layout.
//-----------------------------------------------------------------------------

namespace VCollisionLayout
{
    //-----------------------------------------------------------------------------
    // Function: updateItemMove()
    //-----------------------------------------------------------------------------
    template <class T>
    void updateItemMove(QList<T*>& items, T* item, qreal minY, qreal spacing)
    {
        item->setY(std::max(minY - item->boundingRect().top(), item->y()));

        // Remove the moved item from the list.
        items.removeAt(items.indexOf(item));

        int index = 0;
        qreal y = minY;

        // Phase 1. Skip over the items that are fully above the moved item.
        for (index = 0; index < items.size(); ++index)
        {
            T* curItem = items.at(index);

            if (curItem->y() + curItem->boundingRect().bottom() + spacing >
                item->y() + item->boundingRect().top())
            {
                break;
            }

            y = curItem->y() + curItem->boundingRect().bottom() + spacing;
        }

        // Phase 2. Move the first colliding item above the moved item if it is possible
        // and the moved item is at least slightly over the colliding item.
        if (index < items.size())
        {
            T* curItem = items.at(index);

            if (item->y() < curItem->y() && curItem->y() + curItem->boundingRect().top() <
                item->y() + item->boundingRect().bottom() + spacing &&
                y + curItem->boundingRect().height() + spacing <= item->y() + item->boundingRect().top() &&
                item->y() + item->boundingRect().top() - spacing - curItem->boundingRect().bottom() >= minY)
            {
                curItem->setY(item->y() + item->boundingRect().top() - spacing -
                              curItem->boundingRect().bottom());

                ++index;
            }
        }

        // Phase 3. Put the moved item to its correct place.
        items.insert(index, item);

        // Phase 4. Move the rest of the items below the moved item so that they do not collide.
        while (index < items.size() - 1)
        {
            T* item1 = items.at(index);
            T* item2 = items.at(index + 1);

            if (item1->y() + item1->boundingRect().bottom() + spacing >
                item2->y() + item2->boundingRect().top())
            {
                item2->setY(item1->y() + item1->boundingRect().bottom() + spacing -
                                item2->boundingRect().top());
            }

            ++index;
        }
    }

    //-----------------------------------------------------------------------------
    // Function: setItemPos()
    //-----------------------------------------------------------------------------
    template <class T>
    void setItemPos(QList<T*> const& /*items*/, T* item, qreal x, qreal /*minY*/, qreal /*spacing*/)
    {
        item->setX(x);
    }
}
