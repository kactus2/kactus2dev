//-----------------------------------------------------------------------------
// File: librarytreefilter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// The class that does the filtering and sorting of items for the library tree view.
//-----------------------------------------------------------------------------

#ifndef LIBRARYTREEFILTER_H
#define LIBRARYTREEFILTER_H

#include "LibraryFilter.h"

class LibraryInterface;

//-----------------------------------------------------------------------------
//! The class that does the filtering and sorting of items for the library tree view.
//-----------------------------------------------------------------------------
class LibraryTreeFilter : public LibraryFilter
{
	Q_OBJECT

public:

    /*! The constructor
     *
     *      @param [in]  handler    Pointer to the instance that manages the library.
     *      @param [in]  parent     Pointer to the owner of this widget.
     *
    */
    LibraryTreeFilter(LibraryInterface* handler, QObject *parent = 0);

	//! The destructor
    virtual ~LibraryTreeFilter();

protected:

	/*! Checks if the row should be shown in the view or not.
	 *
	 *      @param [in]  sourceRow Specifies the row number of the item
	 *      @param [in]  sourceParent Specifies the parent of the item.
	 *
	 * \return True if item is shown.
	*/
	virtual bool filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const;

private:

	//! No copying
	LibraryTreeFilter(const LibraryTreeFilter& other);

	//! No assignment
	LibraryTreeFilter& operator=(const LibraryTreeFilter& other);

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

};

#endif // LIBRARYTREEFILTER_H
