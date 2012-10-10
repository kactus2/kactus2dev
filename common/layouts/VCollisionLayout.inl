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

//-----------------------------------------------------------------------------
// Function: VCollisionLayout<T>::VCollisionLayout()
//-----------------------------------------------------------------------------
template <class T>
VCollisionLayout<T>::VCollisionLayout(qreal spacing) : spacing_(spacing)
{
}

//-----------------------------------------------------------------------------
// Function: VCollisionLayout<T>::~VCollisionLayout()
//-----------------------------------------------------------------------------
template <class T>
VCollisionLayout<T>::~VCollisionLayout()
{
}

//-----------------------------------------------------------------------------
// Function: updateItemMove()
//-----------------------------------------------------------------------------
template <class T>
void VCollisionLayout<T>::updateItemMove(QList<T*>& items, T* item, qreal minY)
{
    item->setY(qMax(minY - item->boundingRect().top(), item->y()));
    QRectF itemRect = item->mapRectToParent(item->boundingRect());

    // Remove the moved item from the list.
    items.removeAt(items.indexOf(item));

    int index = 0;
    qreal y = minY;

    // Phase 1. Skip over the items that are fully above the moved item.
    for (index = 0; index < items.size(); ++index)
    {
        T* curItem = items.at(index);
        QRectF curItemRect = curItem->mapRectToParent(curItem->boundingRect());

        if (curItemRect.bottom() + spacing_ > itemRect.top())
        {
            break;
        }

        y = curItemRect.bottom() + spacing_;
    }

    // Phase 2. Move the first colliding item above the moved item if it is possible
    // and the moved item is at least slightly over the colliding item.
    if (index < items.size())
    {
        T* curItem = items.at(index);
        QRectF curItemRect = curItem->mapRectToParent(curItem->boundingRect());

        if (item->y() < curItem->y() &&
            curItemRect.top() < itemRect.bottom() + spacing_ &&
            y + curItemRect.height() + spacing_ <= itemRect.top() &&
            itemRect.top() - spacing_ - (curItemRect.bottom() - curItem->y()) >= minY)
        {
            curItem->setY(itemRect.top() - spacing_ - (curItemRect.bottom() - curItem->y()));
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

        QRectF itemRect1 = item1->mapRectToParent(item1->boundingRect());
        QRectF itemRect2 = item2->mapRectToParent(item2->boundingRect());

        if (itemRect1.bottom() + spacing_ > itemRect2.top())
        {
            item2->setY(itemRect1.bottom() + spacing_ - (itemRect2.top() - item2->y()));
        }

        ++index;
    }
}

//-----------------------------------------------------------------------------
// Function: setItemPos()
//-----------------------------------------------------------------------------
template <class T>
void VCollisionLayout<T>::setItemPos(QList<T*> const&, T* item, qreal x, qreal)
{
    item->setX(x);
}

//-----------------------------------------------------------------------------
// Function: VCollisionLayout<T>::updateItemPositions()
//-----------------------------------------------------------------------------
template <class T>
void VCollisionLayout<T>::updateItemPositions(QList<T*>&, qreal, qreal)
{
    // Nothing to do.
}

