//-----------------------------------------------------------------------------
// File: hierarchyfilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 1.7.2011
//
// Description:
// The class that does the filtering and sorting of items for the library hierarchy view.
//-----------------------------------------------------------------------------

#ifndef HIERARCHYFILTER_H
#define HIERARCHYFILTER_H

#include "../LibraryFilter.h"

#include <KactusAPI/include/utils.h>

#include <QSharedPointer>
#include <QString>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! The class that does the filtering and sorting of items for the library hierarchy view.
//-----------------------------------------------------------------------------
class HierarchyFilter : public LibraryFilter 
{
	Q_OBJECT

public:

	/*! The constructor
	 *
     *      @param [in] libraryAccess   Access interface to the library.
	 *      @param [in] parent          Pointer to the owner of this filter.
     */
    HierarchyFilter(LibraryInterface* libraryAccess, QObject *parent);

	//! The destructor
	virtual ~HierarchyFilter() = default;

    //! No copying
    HierarchyFilter(const HierarchyFilter& other) = delete;

    //! No assignment
    HierarchyFilter& operator=(const HierarchyFilter& other) = delete;

protected:

	/*! Validate the given row on a parent and check if item should be shown.
	 *
	 *      @param [in]  source_row     The row number of the child on the parent.
	 *      @param [in]  source_parent  Model index that identifies the parent object.
	 *
	 *      @return bool True if object should be displayed.
	*/
	virtual bool filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const override final;

};

#endif // HIERARCHYFILTER_H
