/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetseditor.h
 *		Project: Kactus 2
 */

#ifndef FILESETSEDITOR_H
#define FILESETSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <common/views/EditableTableView/editabletableview.h>
#include "filesetsmodel.h"

#include <QSortFilterProxyModel>

class LibraryInterface;

/*! \brief The editor to add/remove/edit file sets of a component.
 *
 */
class FileSetsEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The default width of the name column.
	static const int NAME_COLUMN_WIDTH = 200;

	//! \brief The default width of the description column.
	static const int DESC_COLUMN_WIDTH = 300;

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance managing the library.
	 *
	*/
	FileSetsEditor(QSharedPointer<Component> component,
		LibraryInterface* handler);
	
	//! \brief The destructor
	~FileSetsEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the editor to the IPXact model.
	 *
	*/
	virtual void makeChanges();

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! \brief No copying
	FileSetsEditor(const FileSetsEditor& other);

	//! \brief No assignment
	FileSetsEditor& operator=(const FileSetsEditor& other);

	//! \brief The view to display the file sets.
	EditableTableView view_;

	//! \brief The model that manages the file set objects for the view.
	FileSetsModel model_;

	//! \brief The proxy to do the sorting
	QSortFilterProxyModel proxy_;
};

#endif // FILESETSEDITOR_H
