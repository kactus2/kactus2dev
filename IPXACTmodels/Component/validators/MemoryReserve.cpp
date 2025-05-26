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

#include <QHash>
#include <QSet>

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
    if (start_ && other.start_)
        return start_->coord_ < other.start_->coord_;

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::addArea()
//-----------------------------------------------------------------------------
void MemoryReserve::addArea(QString const& newId, quint64 newBegin, quint64 newEnd)
{
    QSharedPointer<MemoryArea> newArea(new MemoryArea());
    newArea->id_ = newId;
    newArea->start_ = QSharedPointer<MemoryEndPoint>(new MemoryEndPoint{ newBegin, true, newArea.data() });
    newArea->end_ = QSharedPointer<MemoryEndPoint>(new MemoryEndPoint{ newEnd, false, newArea.data() });

    if (!addedIds_.contains(newId))
    {
        addedIds_.insert(newId, 0);
        newArea->index_ = 0;
    }
    else
    {
        addedIds_[newId] += 1;
        newArea->index_ = addedIds_[newId];
    }
    
    // Area endpoints stored seperately as well to make overlap check easier
    reservedArea_.append(newArea);
    endPoints_.append(newArea->start_);
    endPoints_.append(newArea->end_);
    areaIsSorted_ = false;
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

    sortIfNotSorted();

    for (int areaIndex = 0; areaIndex < reservedArea_.size() - 1; ++areaIndex)
    {
        if (reservedArea_.at(areaIndex + 1)->start_->coord_ <= reservedArea_.at(areaIndex)->end_->coord_)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::checkOverlapAndContainment()
//-----------------------------------------------------------------------------
void MemoryReserve::checkOverlapAndContainment(QSet<QString>& overlaps, quint64 rangeMax)
{
    // At least two items needed for them to overlap
    if (reservedArea_.count() < 2)
    {
        if (reservedArea_.isEmpty())
            return;

        // Check still containment, if one item exists
        auto area = reservedArea_.first();
        if (area->start_->coord_ > rangeMax - 1 || area->end_->coord_ > rangeMax - 1)
        {
            overlaps.insert(area->id_);
        }
        
        return;
    }

    sortIfNotSorted();
    
    QSharedPointer<MemoryEndPoint> currentlyOpen = nullptr; // Start point of currently open memory range

    // sweep-line algorithm to find overlaps and out of bounds ranges
    for (int i = 0; i < endPoints_.size(); ++i)
    {
        auto indexedPoint = endPoints_.at(i);

        // Check out of bounds
        if (indexedPoint->coord_ > rangeMax - 1)
        {
            overlaps.insert(indexedPoint->parentArea_->id_);
        }

        if (indexedPoint->isBegin_)
        {
            if (!currentlyOpen)
            {
                currentlyOpen = indexedPoint;
            }
            else
            {
                // Overlap, if there is already an open range and indexed point is the start of a new range
                overlaps.insert(indexedPoint->parentArea_->id_);
                overlaps.insert(currentlyOpen->parentArea_->id_);
                
                if (indexedPoint->parentArea_->end_->coord_ > currentlyOpen->parentArea_->end_->coord_)
                {
                    currentlyOpen = indexedPoint;
                }
            }
        }
        else
        {
            if (currentlyOpen && indexedPoint->parentArea_ == currentlyOpen->parentArea_)
            {
                currentlyOpen = nullptr;
            }
        }
    }
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

    sortIfNotSorted();

    for (int areaIndex = 0; areaIndex < reservedArea_.size() - 1; ++areaIndex)
    {
        auto area = reservedArea_.at(areaIndex);

        for (int nextIndex = areaIndex + 1; nextIndex < reservedArea_.size(); ++nextIndex)
        {
            auto nextArea = reservedArea_.at(nextIndex);

            if (area->id_ == nextArea->id_)
            {
                if (nextArea->start_->coord_ > area->end_->coord_)
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

    sortIfNotSorted();

    // Keeps track of open ranges with no enpoint yet
    QSet<MemoryArea* > currentlyOpen;

    // modified sweep line to find overlap and print error
    for (auto const& endpoint : endPoints_)
    {
        if (endpoint->isBegin_)
        {
            for (auto const& openRange : currentlyOpen)
            {
                // Only display area id (name) by default, if no replicas.
                QString overlapArea1 = openRange->id_;
                QString overlapArea2 = endpoint->parentArea_->id_;

                // First area has replicas => display index
                if (addedIds_[openRange->id_] > 0)
                {
                    overlapArea1 = QObject::tr("%1 (%2)").arg(openRange->id_)
                        .arg(openRange->index_);
                }

                // Second area has replicas => display index
                if (addedIds_[endpoint->parentArea_->id_] > 0)
                {
                    overlapArea2 = QObject::tr("%1 (%2)").arg(endpoint->parentArea_->id_).arg(endpoint->parentArea_->index_);
                }

                errors.append(QObject::tr("%1 %2 and %3 overlap within %4")
                    .arg(itemIdentifier).arg(overlapArea1).arg(overlapArea2).arg(context));
            }

            currentlyOpen.insert(endpoint->parentArea_);
        }
        else
        {
            currentlyOpen.remove(endpoint->parentArea_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::findErrorsInContainment()
//-----------------------------------------------------------------------------
void MemoryReserve::findErrorsInContainment(QList<QString>& errors, QString const& itemId, 
    QString const& itemType, QString const& context, quint64 areaStart, quint64 areaEnd)
{
    // Find items matching id and check if they are within given area
    for (auto const& memArea : reservedArea_)
    {
        if (memArea->id_ != itemId)
        {
            continue;
        }

        if (memArea->start_->coord_ < areaStart || memArea->end_->coord_ > areaEnd)
        {
            QString displayedId = itemId;
            if (addedIds_[itemId] > 1)
            {
                displayedId = QObject::tr("%1 (%2)").arg(itemId).arg(memArea->index_);
            }

            errors.append(QObject::tr("%1 %2 is not contained within %3")
                .arg(itemType).arg(displayedId).arg(context));
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

    sortIfNotSorted();

	for (int i = 0; i < reservedArea_.size(); ++i)
	{
		auto area = reservedArea_.at(i);

		for (int j = i + 1; j < reservedArea_.size(); ++j)
		{
			auto nextArea = reservedArea_.at(j);

			if (area->id_ == nextArea->id_ && nextArea->start_->coord_ <= area->end_->coord_)
			{
				errors.append(QObject::tr("Multiple definitions of %1 %2 overlap within %3")
					.arg(itemIdentifier).arg(area->id_).arg(context));
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::sortIfNotSorted()
//-----------------------------------------------------------------------------
void MemoryReserve::sortIfNotSorted()
{
    if (!areaIsSorted_)
    {
        std::sort(reservedArea_.begin(), reservedArea_.end(), [](QSharedPointer<MemoryArea> areaA, QSharedPointer<MemoryArea> areaB)
            {
                return areaA->operator<(*areaB);
            });
        std::sort(endPoints_.begin(), endPoints_.end(), [](QSharedPointer<MemoryEndPoint> pointA, QSharedPointer<MemoryEndPoint> pointB)
            {
                return pointA->operator<(*pointB);
            });
        areaIsSorted_ = true;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryReserve::MemoryEndPoint::operator<()
//-----------------------------------------------------------------------------
bool MemoryReserve::MemoryEndPoint::operator<(MemoryEndPoint const& other) const
{
    if (coord_ != other.coord_)
        return coord_ < other.coord_;

    // Ensure that "begin" endpoints come before "end" endpoints
    if (isBegin_ != other.isBegin_)
        return isBegin_; // true < false

    // increase consistency in sorting by comparing id (name) and index of containing area
    if (parentArea_->id_ != other.parentArea_->id_)
        return parentArea_->id_ < other.parentArea_->id_;

    if (parentArea_->index_ != other.parentArea_->index_)
        return parentArea_->index_ < other.parentArea_->index_;

    // Final fallback: compare memory addresses to ensure strict ordering
    return this < &other;
}
