//-----------------------------------------------------------------------------
// File: HStackedLayout.inl
//-----------------------------------------------------------------------------
// Project: Endpoint Designer
// Author: Joni-Matti M‰‰tt‰
// Date: 22.4.2011
//
// Description:
// Horizontal stacked layout.
//-----------------------------------------------------------------------------

#include <limits>
#include <complex>

//-----------------------------------------------------------------------------
// Function: HStackedLayout<T>::HStackedLayout()
//-----------------------------------------------------------------------------
template <class T>
HStackedLayout<T>::HStackedLayout(qreal spacing) : spacing_(spacing)
{
}

//-----------------------------------------------------------------------------
// Function: HStackedLayout<T>::~HStackedLayout()
//-----------------------------------------------------------------------------
template <class T>
HStackedLayout<T>::~HStackedLayout()
{
}

//-----------------------------------------------------------------------------
// Function: updateItemMove()
//-----------------------------------------------------------------------------
template <class T>
void HStackedLayout<T>::updateItemMove(QList<T*>& items, T* item, qreal minX)
{
    using DiagramUtil::clampCeil;

    // Move the item to the last position (this simplifies the for loop).
    items.removeAt(items.indexOf(item));
    items.append(item);

    // Determine the best position for the item (position where the item is closest to
    // its resulting stacked position).
    qreal refX = item->pos().x() + item->boundingRect().left();
    qreal dist = std::numeric_limits<double>::max();
    qreal x = minX;
    int index = 0;

    for (int i = 0; i < items.size(); ++i)
    {
        qreal newDist = std::fabs(x - refX);

        // We can stop when the distance begins to grow.
        if (newDist > dist)
        {
            break;
        }

        dist = newDist;
        index = i;

        // Otherwise increment the bookkeeping x.
        x += clampCeil(items.at(index)->boundingRect().width(), GridSize) + spacing_;
    }

    // Change the item's position in the list.
    items.removeAt(items.indexOf(item));
    items.insert(index, item);

    // Move the other columns accordingly.
    x = minX;

    for (int i = 0; i < items.size(); ++i)
    {
        if (i != index)
        {
            items.at(i)->setX(x + clampCeil(-items.at(i)->boundingRect().left(), GridSize));
        }

        x += clampCeil(items.at(i)->boundingRect().width(), GridSize) + spacing_;
    }
}

//-----------------------------------------------------------------------------
// Function: setItemPos()
//-----------------------------------------------------------------------------
template <class T>
void HStackedLayout<T>::setItemPos(QList<T*> const& items, T* item, qreal y, qreal minX)
{
    using DiagramUtil::clampCeil;

    int index = items.indexOf(item);
    qreal x = minX;

    for (int i = 0; i < index; ++i)
    {
        x += clampCeil(items.at(i)->boundingRect().width(), GridSize) + spacing_;
    }

    item->setPos(x + clampCeil(-item->boundingRect().left(), GridSize), y);
}

//-----------------------------------------------------------------------------
// Function: updateItemPositions()
//-----------------------------------------------------------------------------
template <class T>
void HStackedLayout<T>::updateItemPositions(QList<T*>& items, qreal y, qreal minX)
{
    using DiagramUtil::clampCeil;

    qreal x = minX;

    for (int i = 0; i < items.size(); ++i)
    {
        items.at(i)->setPos(x + clampCeil(-items.at(i)->boundingRect().left(), GridSize), y);
        x += clampCeil(items.at(i)->boundingRect().width(), GridSize) + spacing_;
    }
}
