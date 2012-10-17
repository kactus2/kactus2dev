//-----------------------------------------------------------------------------
// File: VStaticLayout.inl
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.10.2012
//
// Description:
// Vertical static layout.
//-----------------------------------------------------------------------------

#include <common/DiagramUtil.h>

//-----------------------------------------------------------------------------
// Function: VStaticLayout<T>::VStaticLayout()
//-----------------------------------------------------------------------------
template <class T>
VStaticLayout<T>::VStaticLayout(qreal spacing) : spacing_(spacing)
{
}

//-----------------------------------------------------------------------------
// Function: VStaticLayout<T>::~VStaticLayout()
//-----------------------------------------------------------------------------
template <class T>
VStaticLayout<T>::~VStaticLayout()
{
}

//-----------------------------------------------------------------------------
// Function: updateItemMove()
//-----------------------------------------------------------------------------
template <class T>
void VStaticLayout<T>::updateItemMove(QList<T*>& items, T* item, qreal minY)
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

    // Phase 3. Put the moved item to its correct place.
    items.insert(index, item);
}

//-----------------------------------------------------------------------------
// Function: setItemPos()
//-----------------------------------------------------------------------------
template <class T>
void VStaticLayout<T>::setItemPos(QList<T*> const&, T* item, qreal x, qreal)
{
    item->setX(x);
}

//-----------------------------------------------------------------------------
// Function: VStaticLayout<T>::updateItemPositions()
//-----------------------------------------------------------------------------
template <class T>
void VStaticLayout<T>::updateItemPositions(QList<T*>& items, qreal x, qreal minY)
{
    using DiagramUtil::clampCeil;

    if (!items.empty())
    {
        T* firstItem = items.at(0);
        firstItem->setY(qMax(minY - firstItem->boundingRect().top(), firstItem->y()));

        qreal y = firstItem->y() + firstItem->boundingRect().bottom() + spacing_;

        for (int i = 1; i < items.size(); ++i)
        {
            items.at(i)->setPos(x, y + clampCeil(-items.at(i)->boundingRect().top(), GridSize));
            y += clampCeil(items.at(i)->boundingRect().height(), GridSize) + spacing_;
        }
    }
}
