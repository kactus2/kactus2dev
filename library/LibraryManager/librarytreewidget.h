//-----------------------------------------------------------------------------
// File: librarytreewidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// LibraryTreeWidget is the widget that displays the library content in a tree.
//-----------------------------------------------------------------------------

#ifndef LIBRARYTREEWIDGET_H
#define LIBRARYTREEWIDGET_H

#include "librarytreeview.h"

#include <IPXACTmodels/common/VLNV.h>

#include <QWidget>

class LibraryTreeModel;
class LibraryInterface;
class LibraryTreeFilter;

//-----------------------------------------------------------------------------
//! LibraryTreeWidget is the widget that displays the library content in a tree.
//-----------------------------------------------------------------------------
class LibraryTreeWidget : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *  Constructs the library tree view and the filter model for the tree view
	 *  and connects the view and filter model to the dataModel.
	 * 
	 *       @param [in] handler    The instance that manages the library.
	 *       @param [in] dataModel  The dataModel instance that holds the data to be displayed in the view.
	 *       @param [in] parent     The owner of this widget.
	 */
    LibraryTreeWidget(LibraryInterface* handler, LibraryTreeModel* dataModel, QWidget* parent);

    //! The destructor
    virtual ~LibraryTreeWidget();

	/*! Select the library item that is identified by the vlnv.
	 *
	 *      @param [in] vlnv The vlnv that identifies the libraryItem to select.
	*/
	void selectItem(VLNV const& vlnv);

    /*!
     *  Gets the filter for the tree model.
     *
     *      @return The filter in use.
     */
    LibraryFilter* getFilter() const;

signals:

	//! Print a notification to the user
	void noticeMessage(const QString& msg);

	//! Print an error message to the user
	void errorMessage(const QString& msg);

	//! Emitted when an item identifying unique vlnv is selected.
	void itemSelected(const VLNV& vlnv);

private:

	//! No copying.
	LibraryTreeWidget(const LibraryTreeWidget& other);

	//! No assignment.
	LibraryTreeWidget& operator=(const LibraryTreeWidget& other);

	//! Set up the connections between child widgets.
	void setupConnections(LibraryTreeModel* dataModel);
   
    //! The filter between the view and data model.
    LibraryTreeFilter* filter_;

	//! The table view to display the search results.
	LibraryTreeView view_;

	//! The model for the filter.
	LibraryTreeModel* dataModel_;

};

#endif // LIBRARYTREEWIDGET_H
