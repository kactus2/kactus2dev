/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYTREEWIDGET_H
#define LIBRARYTREEWIDGET_H

#include "librarytreeview.h"
#include "librarytreefilter.h"

#include <library/LibraryManager/vlnv.h>

#include <QWidget>

class LibraryTreeModel;
class LibraryInterface;
class VLNVDialer;

/*! \brief LibraryTreeWidget is the widget that displays the library in a tree.
 *
 * Widget contains an add-button to select items from the library and check
 * box to hide/show hidden library items.
 */
class LibraryTreeWidget : public QWidget {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 *  Constructs the library tree view and the filter model for the tree view
	 *  and connects the view and filter model to the dataModel.
	 * 
	 *  \param dialer Pointer to the dialer that provides search options.
	 *  \param handler Pointer to the instance that manages the library.
	 *  \param dataModel Pointer to the dataModel instance that holds the data
	 *  to be displayed in the view.
	 *  \param parent Pointer to the owner of this widget.
	 */
    LibraryTreeWidget(VLNVDialer* dialer,
		LibraryInterface* handler, 
		LibraryTreeModel* dataModel,
		QWidget* parent);

    //! \brief The destructor
    virtual ~LibraryTreeWidget();

	/*! \brief Select the library item that is identified by the vlnv
	 *
	 * \param vlnv Reference to the vlnv that identifies the libraryItem to select
	 *
	*/
	void selectItem(const VLNV& vlnv);

signals:

	//! \brief Print a notification to the user
	void noticeMessage(const QString& msg);

	//! \brief Print an error message to the user
	void errorMessage(const QString& msg);

	//! \brief Emitted when an item identifying unique vlnv is selected.
	void itemSelected(const VLNV& vlnv);

private:

	//! \brief No copying
	LibraryTreeWidget(const LibraryTreeWidget& other);

	//! \brief No assignment
	LibraryTreeWidget& operator=(const LibraryTreeWidget& other);

	//! \brief Set up the connections between child widgets
	void setupConnections(LibraryTreeModel* dataModel);

	//! \brief The table view to display the search results
	LibraryTreeView view_;

	//! \brief The filter between the view and data model.
	LibraryTreeFilter filter_;

	//! \brief The model for the filter
	LibraryTreeModel* dataModel_;

};

#endif // LIBRARYTREEWIDGET_H
