/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceseditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACESEDITOR_H
#define ADDRESSSPACESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include "addressspacesmodel.h"

#include <QSortFilterProxyModel>

/*! \brief The editor to add/remove/edit address spaces of component.
 *
 */
class AddressSpacesEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 *
	*/
	AddressSpacesEditor(QSharedPointer<Component> component);
	
	//! \brief The destructor
	~AddressSpacesEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private:
	
	//! \brief No copying
	AddressSpacesEditor(const AddressSpacesEditor& other);

	//! \brief No assignment
	AddressSpacesEditor& operator=(const AddressSpacesEditor& other);

	//! \brief The view to display the address spaces.
	EditableTableView view_;

	//! \brief The model that manages the file set objects for the view.
	AddressSpacesModel model_;

	//! \brief The proxy to do the sorting
	QSortFilterProxyModel proxy_;
};

#endif // ADDRESSSPACESEDITOR_H
