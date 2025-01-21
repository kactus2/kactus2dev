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
     *    @param [in]  handler    The instance that manages the library.
     *    @param [in]  parent     The owner of this widget.
     *
    */
    LibraryTreeFilter(LibraryInterface* handler, QObject *parent = 0);

	//! The destructor
    virtual ~LibraryTreeFilter() = default;

    //! No copying
    LibraryTreeFilter(const LibraryTreeFilter& other) = delete;

    //! No assignment
    LibraryTreeFilter& operator=(const LibraryTreeFilter& other) = delete;

protected:

	/*! Checks if the row should be shown in the view or not.
	 *
	 *    @param [in]  sourceRow      Specifies the row number of the item
	 *    @param [in]  sourceParent   Specifies the parent of the item.
	 *
	 *    @return True if item is shown, otherwise false.
	*/
	virtual bool filterAcceptsRow(int sourceRow, QModelIndex const& sourceParent) const override final;

private:

	/*!
	 *  Checks if the VLNV should be shown in the view or not based on filters.
	 *
	 *    @param [in] vlnv		The VLNV to check.
	 *
	 *    @return True, if the VLNV should be shown, otherwise false.
	 */
	bool checkType(VLNV const& vlnv) const;

};

#endif // LIBRARYTREEFILTER_H
