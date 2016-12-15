//-----------------------------------------------------------------------------
// File: MemoryDesignerConstants.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.12.2016
//
// Description:
// Common declarations for memory designer.
//-----------------------------------------------------------------------------

#include "MemoryDesignerConstants.h"

//-----------------------------------------------------------------------------
// Function: MemoryDesignerConstants::itemOverlapsAnotherItem()
//-----------------------------------------------------------------------------
bool MemoryDesignerConstants::itemOverlapsAnotherItem(QRectF firstRectangle, int firstLineWidth,
    QRectF secondRectangle, int secondLineWidth)
{
    qreal firstItemTop = firstRectangle.topLeft().y() + firstLineWidth;
    qreal firstItemLow = firstRectangle.bottomLeft().y() - firstLineWidth;
    qreal secondItemTop = secondRectangle.topLeft().y() + secondLineWidth;
    qreal secondItemLow = secondRectangle.bottomLeft().y() - secondLineWidth;

    if ((firstItemTop > secondItemTop && firstItemTop < secondItemLow) ||
        (firstItemLow < secondItemLow && firstItemLow > secondItemTop) ||

        (secondItemTop > firstItemTop && secondItemTop < firstItemLow) ||
        (secondItemLow < firstItemLow && secondItemLow > firstItemTop) ||

        (firstItemTop == secondItemTop && firstItemLow == secondItemLow))
    {
        return true;
    }

    return false;
}