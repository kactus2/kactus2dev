/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYSEARCHWIDGET_H
#define LIBRARYSEARCHWIDGET_H

#include "typebox.h"
#include "vlnvbox.h"
#include "librarysearchfilter.h"
#include "librarytableview.h"

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>

class LibraryData;
class LibraryHandler;

/*! brief Contains the GUI-items to be displayed in the search tab.
*
*/
class LibrarySearchWidget : public QWidget {

	Q_OBJECT

public:

    /*! \brief The constructor
     *
     * \param parent Pointer to the owner of this widget.
     * \param dataModel Pointer to the data model that is used as base model for
     * the filter.
     *
    */
    LibrarySearchWidget(LibraryHandler *parent, LibraryData* dataModel);

	//! \brief The destructor
    virtual ~LibrarySearchWidget();

private:

	//! \brief No copying
	LibrarySearchWidget(const LibrarySearchWidget& other);

	//! \brief No assignment
	LibrarySearchWidget& operator=(LibrarySearchWidget& other);

	//! \brief Set up the layout for the widget
	void setupLayout();

	//! \brief Set up the connections between the child widgets of this widget
	void setupConnections();

	//! \brief Connect the signals from dataModel
	void connectDataModel(LibraryData* dataModel);

	//! \brief Label to display info for searchWordEdit
	QLabel searchLabel_;

	//! \brief The line edit to set the search word
	QLineEdit searchWordEdit_;

	//! \brief The QGroupBox that contains the search settings for vlnv type.
	TypeBox typeBox_;

	//! \brief The QGroupBox that contains the search settings for vlnv fields.
	VLNVBox vlnvBox_;

	//! \brief The check box to hide/show hidden library items
	QCheckBox hideShowCheckBox_;

	//! \brief The filter instance that controls which items to display in view.
	LibrarySearchFilter filter_;

	//! \brief The table view to display the library search results.
	LibraryTableView view_;
};

#endif // LIBRARYSEARCHWIDGET_H
