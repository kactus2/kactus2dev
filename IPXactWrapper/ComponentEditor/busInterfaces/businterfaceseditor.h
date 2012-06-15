/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfaceseditor.h
 *		Project: Kactus 2
 */

#ifndef BUSINTERFACESEDITOR_H
#define BUSINTERFACESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include "businterfacesmodel.h"
#include <models/component.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief Editor to add/remove/edit the bus interfaces of a component.
 *
 */
class BusInterfacesEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	BusInterfacesEditor(LibraryInterface* handler,
		QSharedPointer<Component> component,
				QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~BusInterfacesEditor();

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
	BusInterfacesEditor(const BusInterfacesEditor& other);

	//! \brief No assignment
	BusInterfacesEditor& operator=(const BusInterfacesEditor& other);

	//! \brief The view that displays the summary of the bus interfaces.
	EditableTableView view_;

	//! \brief The model that does the sorting of items.
	QSortFilterProxyModel proxy_;

	//! \brief The model that manages the bus interfaces being displayed.
	BusInterfacesModel model_;
};

#endif // BUSINTERFACESEDITOR_H
