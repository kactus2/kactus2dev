/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYSEARCHFILTER_H
#define LIBRARYSEARCHFILTER_H

#include "librarydata.h"
#include "typebox.h"
#include "vlnvbox.h"

#include <QSortFilterProxyModel>
#include <QModelIndex>

/*! \brief The class that does the filtering and sorting of items for search view.
 *
 */
class LibrarySearchFilter : public QSortFilterProxyModel {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this class
	 * 
	*/
	LibrarySearchFilter(QObject *parent = 0);

	//! \brief The destructor
	virtual ~LibrarySearchFilter();

signals:

public slots:

	/*! \brief The handler when user changes the show/hide hidden files status.
	 *
	 * \param state Specifies the state of the check box.
	 *
	*/
	void onShowStatusChange(int state);

	/*! \brief The handler when the search word changes
	 *
	 * \param newWord The new search word used to filter the library items
	 *
	*/
	void onSearchWordChanged(const QString& newWord);

	/*! \brief The handler when user changes the vlnv types to search for
	 *
	 * \param newSettings Contains info on which items shuold be included in search.
	 *
	*/
	void onTypeStateChanged(const TypeBox::TypeSearchSettings* newSettings);

	/*! \brief The handler when user selects which fields to search in VLNV tags.
	 *
	 * \param newSettings Contains info on which vlnv tags are included in the search.
	 *
	*/
	void onVLNVStateChanged(const VLNVBox::VLNVSearchSettings* newSettings);

protected:

	/*! \brief The function that specified if the item should be shown in view.
	 *
	 * \param sourceRow The row of the item.
	 * \param sourceParent The model index of the parent of the item
	 *
	 * \return True if the item should be visible
	*/
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;

private:

	//! \brief No copying
	LibrarySearchFilter(const LibrarySearchFilter& other);

	//! \brief No assignment
	LibrarySearchFilter& operator=(const LibrarySearchFilter& other);

	/*! \brief Checks if the searchWord matches the item
	 *
	 * \param item Pointer to the table item that is checked
	 *
	 * \return True if the search is a match, otherwise false.
	 */
	bool searchWordMatch(VLNV* vlnv) const;

	/*! \brief Checks the document type if it matches the search settings
	 *
	 * \param item Pointer to the table item that is checked
	 *
	 * \return True if the type is included in the search, otherwise false.
	 */
	bool checkTypeMatch(VLNV* vlnv) const;

	//! \brief Holds information on should the hidden items be visible or not.
	bool showHidden_;

	//! \brief The string that is used in the search
	QString searchWord_;

	//! \brief Contains the settings to select document types.
	TypeBox::TypeSearchSettings typeSettings_;

	//! \brief Contains the settings to select vlnv tags.
	VLNVBox::VLNVSearchSettings vlnvSettings_;

};

#endif // LIBRARYSEARCHFILTER_H
