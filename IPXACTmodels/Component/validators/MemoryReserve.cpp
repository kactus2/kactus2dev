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
MemoryReserve::MemoryReserve()
{

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
void MemoryReserve::addArea(QString const& newId, qint64 newBegin, qint64 newEnd)
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
    if (reservedArea_.count() < 2)
    {
        return false;
    }

    std::sort(reservedArea_.begin(), reservedArea_.end());

    for (int areaIndex = 0; areaIndex < reservedArea_.size() - 1; ++areaIndex)
    {
        if (reservedArea_.at(areaIndex + 1).begin_ <= reservedArea_.at(areaIndex).end_)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::hasIdDependantOverlap()
//-----------------------------------------------------------------------------
bool MemoryReserve::hasIdDependantOverlap()
{
    if (reservedArea_.count() < 2)
    {
        return false;
    }

    std::sort(reservedArea_.begin(), reservedArea_.end());

    for (int areaIndex = 0; areaIndex < reservedArea_.size() - 1; ++areaIndex)
    {
        MemoryArea const& area = reservedArea_.at(areaIndex);

        for (int nextIndex = areaIndex + 1; nextIndex < reservedArea_.size(); ++nextIndex)
        {
            MemoryArea const& nextArea = reservedArea_.at(nextIndex);

            if (area.id_ == nextArea.id_)
            {
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
    if (reservedArea_.isEmpty())
    {
        return;
	}

	std::sort(reservedArea_.begin(), reservedArea_.end());

	for (int i = 0; i < reservedArea_.size(); ++i)
	{
		MemoryArea area = reservedArea_.at(i);

		for (int j = i + 1; j < reservedArea_.size(); ++j)
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

//-----------------------------------------------------------------------------
// Function: MemoryReserve::findErrorsInIdDependantOverlap()
//-----------------------------------------------------------------------------
void MemoryReserve::findErrorsInIdDependantOverlap(QVector<QString>& errors, QString const& itemIdentifier,
    QString const& context)
{
    if (reservedArea_.isEmpty())
	{
		return;
	}

	std::sort(reservedArea_.begin(), reservedArea_.end());

	for (int i = 0; i < reservedArea_.size(); ++i)
	{
		MemoryArea const& area = reservedArea_.at(i);

		for (int j = i + 1; j < reservedArea_.size(); ++j)
		{
			MemoryArea const& nextArea = reservedArea_.at(j);

			if (area.id_ == nextArea.id_ && nextArea.begin_ <= area.end_)
			{
				errors.append(QObject::tr("Multiple definitions of %1 %2 overlap within %3")
					.arg(itemIdentifier).arg(area.id_).arg(context));
				break;
			}
		}
	}
}
