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

/*! \brief The editor to add/remove/edit the COM interfaces of a component.
 *
 */
class ComInterfacesEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	ComInterfacesEditor(QSharedPointer<Component> component,
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
