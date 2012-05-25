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

/*! \brief The editor to add/remove/edit file sets of a component.
 *
 */
class FileSetsEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 *
	*/
	FileSetsEditor(QSharedPointer<Component> component);
	
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

private:

	//! \brief No copying
	FileSetsEditor(const FileSetsEditor& other);

	//! \brief No assignment
	FileSetsEditor& operator=(const FileSetsEditor& other);

	//! \brief The view to display the file sets.
	EditableTableView view_;

	//! \brief The model that manages the file set objects for the view.
	FileSetsModel model_;
};

#endif // FILESETSEDITOR_H
