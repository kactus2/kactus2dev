/* 
 *  	Created on: 28.6.2012
 *      Author: Antti Kamppi
 * 		filename: cominterfaceseditor.h
 *		Project: Kactus 2
 */

#ifndef COMINTERFACESEDITOR_H
#define COMINTERFACESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <models/component.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include "cominterfacesmodel.h"

#include <QSharedPointer>
#include <QSortFilterProxyModel>

class LibraryInterface;

/*! \brief The editor to add/remove/edit the COM interfaces of a component.
 *
 */
class ComInterfacesEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The width of the name column.
	static const int NAME_COLUMN_WIDTH = 100;

	//! \brief The width of the com definition column.
	static const int COM_DEF_COLUMN_WIDTH = 200;

	//! \brief The width of the transfer type column.
	static const int TRANSFER_COLUMN_WIDTH = 100;

	//! \brief The width of the direction column.
	static const int DIRECTION_COLUMN_WIDTH = 100;

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	ComInterfacesEditor(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
				QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~ComInterfacesEditor();

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

private:
	
	//! \brief No copying
	ComInterfacesEditor(const ComInterfacesEditor& other);

	//! \brief No assignment
	ComInterfacesEditor& operator=(const ComInterfacesEditor& other);

	//! \brief The view to display the COM interfaces.
	EditableTableView view_;

	//! \brief The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! \brief The model that manages the COM interfaces.
	ComInterfacesModel model_;
};

#endif // COMINTERFACESEDITOR_H
