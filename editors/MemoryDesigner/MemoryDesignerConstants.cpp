//-----------------------------------------------------------------------------
// File: MemoryDesignerConstants.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.12.2016
//
// Description:
// Common declarations for memory designer.
//-----------------------------------------------------------------------------

#include "MemoryDesignerConstants.h"

#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>
#include <editors/MemoryDesigner/MemoryItem.h>

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

//-----------------------------------------------------------------------------
// Function: MemoryDesignerConstants::getAmountOfNumbersInRange()
//-----------------------------------------------------------------------------
int MemoryDesignerConstants::getAmountOfNumbersInRange(QString const& rangeStart, QString const& rangeEnd)
{
    int startSize = rangeStart.size();
    int endSize = rangeEnd.size();

    int amountOfNumbers = endSize;
    if (startSize > endSize)
    {
        amountOfNumbers = startSize;
    }

    while (amountOfNumbers % 4)
    {
        amountOfNumbers++;
    }

    return amountOfNumbers;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerConstants::getValueWithZerosAdded()
//-----------------------------------------------------------------------------
QString MemoryDesignerConstants::getValueWithZerosAdded(QString const& value, int amountOfNumbers)
{
    QString zeroedValue = value;

    int rangeSize = zeroedValue.size();
    int amountOfZeros = amountOfNumbers - rangeSize;
    while (amountOfZeros > 0)
    {
        zeroedValue.prepend('0');
        amountOfZeros--;
    }

    return zeroedValue;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerConstants::getAreaSizeForRange()
//-----------------------------------------------------------------------------
qreal MemoryDesignerConstants::getAreaSizeForRange(quint64 addressRange)
{
    int areaMultiplication = getRequiredRowsForRange(addressRange);

    return areaMultiplication * RANGEINTERVAL;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerConstants::getRequiredRowsForRange()
//-----------------------------------------------------------------------------
int MemoryDesignerConstants::getRequiredRowsForRange(quint64 addressRange)
{
    int requiredRows = 0;

    if (addressRange == 1 || addressRange == 2)
    {
        requiredRows = addressRange;
    }
    else if (addressRange <= 16)
    {
        requiredRows = 3;
    }
    else if (addressRange <= 128)
    {
        requiredRows = 6;
    }
    else if (addressRange <= 1024)
    {
        requiredRows = 10;
    }
    else if (addressRange <= 16384)
    {
        requiredRows = 16;
    }
    else if (addressRange <= 1048576)
    {
        requiredRows = 26;
    }
    else if (addressRange <= 67108864)
    {
        requiredRows = 42;
    }
    else
    {
        requiredRows = 68;
    }

    return requiredRows;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerConstants::getRequiredAreaForUsedArea()
//-----------------------------------------------------------------------------
qreal MemoryDesignerConstants::getRequiredAreaForUsedArea(qreal usedArea)
{
    int addressRange = qRound(usedArea / RANGEINTERVAL);
    qreal requiredArea = getAreaSizeForRange(addressRange);

    return requiredArea;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerConstants::getMapItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> MemoryDesignerConstants::getMapItem(
    QSharedPointer<ConnectivityInterface const> startInterface,
    QSharedPointer<ConnectivityInterface const> endInterface)
{
    if (startInterface == endInterface)
    {
        return getMemoryItemForLocalMap(startInterface);
    }
    else
    {
        return endInterface->getConnectedMemory();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerConstants::getMemoryItemForLocalMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> MemoryDesignerConstants::getMemoryItemForLocalMap(
    QSharedPointer<ConnectivityInterface const> spaceInterface)
{
    QSharedPointer<MemoryItem const> spaceMemoryItem = spaceInterface->getConnectedMemory();
    QSharedPointer<ConnectivityComponent const> connectionInstance = spaceInterface->getInstance();
    if (spaceMemoryItem && connectionInstance)
    {
        foreach(QSharedPointer<MemoryItem> subSpaceItem, spaceMemoryItem->getChildItems())
        {
            if (subSpaceItem->getType().compare(MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
            {
                return subSpaceItem;
            }
        }
    }

    return QSharedPointer<MemoryItem>();
}
