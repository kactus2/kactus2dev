//-----------------------------------------------------------------------------
// File: MemoryReserve.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.11.2015
//
// Description:
// Class for determining overlapping within an area.
//-----------------------------------------------------------------------------

#include "MemoryReserve.h"

//-----------------------------------------------------------------------------
// Function: MemoryReserve::MemoryReserve()
//-----------------------------------------------------------------------------
MemoryReserve::MemoryReserve():
reservedArea_()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::~MemoryReserve()
//-----------------------------------------------------------------------------
MemoryReserve::~MemoryReserve()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::MemoryArea::operator=()
//-----------------------------------------------------------------------------
MemoryReserve::MemoryArea& MemoryReserve::MemoryArea::operator=(const MemoryReserve::MemoryArea& other)
{
    if (&other != this)
    {
        id_ = other.id_;
        begin_ = other.begin_;
        end_ = other.end_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::MemoryArea::operator<()
//-----------------------------------------------------------------------------
bool MemoryReserve::MemoryArea::operator<(const MemoryReserve::MemoryArea& other) const
{
    return begin_ < other.begin_;
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::addArea()
//-----------------------------------------------------------------------------
void MemoryReserve::addArea(QString const& newId, int newBegin, int newEnd)
{
    MemoryArea newArea;
    newArea.id_ = newId;
    newArea.begin_ = newBegin;
    newArea.end_ = newEnd;

    reservedArea_.append(newArea);
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::hasOverlap()
//-----------------------------------------------------------------------------
bool MemoryReserve::hasOverlap()
{
    if (!reservedArea_.isEmpty())
    {
        qSort(reservedArea_);

        for (int i = 0; i < reservedArea_.size(); ++i)
        {
            MemoryArea area = reservedArea_.at(i);

            for (int j = i+1; j < reservedArea_.size(); ++j)
            {
                MemoryArea nextArea = reservedArea_.at(j);

                if (nextArea.begin_ > area.end_)
                {
                    break;
                }
                else
                {
                    return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::findErrorsInOverlap()
//-----------------------------------------------------------------------------
void MemoryReserve::findErrorsInOverlap(QVector<QString>& errors, QString const& itemIdentifier,
    QString const& context)
{
    if (!reservedArea_.isEmpty())
    {
        qSort(reservedArea_);

        for (int i = 0; i < reservedArea_.size(); ++i)
        {
            MemoryArea area = reservedArea_.at(i);

            for (int j = i+1; j < reservedArea_.size(); ++j)
            {
                MemoryArea nextArea = reservedArea_.at(j);

                if (nextArea.begin_ > area.end_)
                {
                    break;
                }
                else
                {
                    errors.append(QObject::tr("%1 %2 and %3 overlap within %4")
                        .arg(itemIdentifier).arg(area.id_).arg(nextArea.id_).arg(context));
                }
            }
        }
    }
}