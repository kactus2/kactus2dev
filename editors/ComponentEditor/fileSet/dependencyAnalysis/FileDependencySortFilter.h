//-----------------------------------------------------------------------------
// File: FileDependencySortFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.03.2013
//
// Description:
// Sort filter for the file dependency view.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYSORTFILTER_H
#define FILEDEPENDENCYSORTFILTER_H

#include "FileDependencyGraphView.h"

#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
//! Sort filter for the file dependency view.
//-----------------------------------------------------------------------------
class FileDependencySortFilter : public QSortFilterProxyModel
{
public:
    /*!
     *  Constructor.
     */
    FileDependencySortFilter(QObject* parent = 0);

    /*!
     *  Destructor.
     */
    ~FileDependencySortFilter();

    /*!
     *  Sets the filters.
     */
    void setFilters(FileDependencyGraphView::DependencyFilters filters);
    
protected:
    virtual bool filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const;

private:
    // Disable copying.
    FileDependencySortFilter(FileDependencySortFilter const& rhs);
    FileDependencySortFilter& operator=(FileDependencySortFilter const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The dependency graph filters.
    FileDependencyGraphView::DependencyFilters filters_;
};

#endif // FILEDEPENDENCYSORTFILTER_H
