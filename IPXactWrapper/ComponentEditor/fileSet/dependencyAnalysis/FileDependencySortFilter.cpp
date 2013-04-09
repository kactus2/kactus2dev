//-----------------------------------------------------------------------------
// File: FileDependencySortFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 22.03.2013
//
// Description:
// Sort filter for the file dependency view.
//-----------------------------------------------------------------------------

#include "FileDependencySortFilter.h"

#include "FileDependencyItem.h"

//-----------------------------------------------------------------------------
// Function: FileDependencySortFilter::FileDependencySortFilter()
//-----------------------------------------------------------------------------
FileDependencySortFilter::FileDependencySortFilter(QObject* parent /*= 0*/)
    : QSortFilterProxyModel(parent),
      filters_(FileDependencyGraphView::FILTER_DEFAULT)
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencySortFilter::~FileDependencySortFilter()
//-----------------------------------------------------------------------------
FileDependencySortFilter::~FileDependencySortFilter()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencySortFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool FileDependencySortFilter::filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    // Root item is always ok.
    if (!index.isValid())
    {
        return true;
    }

    FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());
    Q_ASSERT(item != 0);

    FileDependencyStatus status = item->getStatus();

    return (status == FILE_DEPENDENCY_STATUS_UNKNOWN) ||
           ((filters_ & FileDependencyGraphView::FILTER_GREEN) && status == FILE_DEPENDENCY_STATUS_OK) ||
           ((filters_ & FileDependencyGraphView::FILTER_YELLOW) && status == FILE_DEPENDENCY_STATUS_CHANGED) ||
           ((filters_ & FileDependencyGraphView::FILTER_RED) && status == FILE_DEPENDENCY_STATUS_CHANGED2);
}

//-----------------------------------------------------------------------------
// Function: FileDependencySortFilter::setFilters()
//-----------------------------------------------------------------------------
void FileDependencySortFilter::setFilters(FileDependencyGraphView::DependencyFilters filters)
{
    filters_ = filters;
    invalidateFilter();
}
