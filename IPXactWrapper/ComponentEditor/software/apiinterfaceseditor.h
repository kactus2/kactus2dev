/* 
 *  	Created on: 27.6.2012
 *      Author: Antti Kamppi
 * 		filename: apiinterfaceseditor.h
 *		Project: Kactus 2
 */

#ifndef APIINTERFACESEDITOR_H
#define APIINTERFACESEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/component.h>
#include <common/views/EditableTableView/editabletableview.h>
#include "apiinterfacesmodel.h"

#include <QSharedPointer>
#include <QSortFilterProxyModel>

/*! \brief The editor to add/remove/edit the API interfaces of a component.
 *
 */
class ApiInterfacesEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The width of the name column
	static const int NAME_COLUMN_WIDTH = 100;

	//! \brief The width of the API definition column
	static const int API_DEF_COLUMN_WIDTH = 200;

	//! \brief The width of the dependency column.
	static const int DEPENDENCY_COLUMN_WIDTH = 100;

	//! \brief The width of the display name column.
	static const int DISPLAY_NAME_COLUMN_WIDTH = 100;

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	ApiInterfacesEditor(QSharedPointer<Component> component,
				QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~ApiInterfacesEditor();

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
	ApiInterfacesEditor(const ApiInterfacesEditor& other);

	//! \brief No assignment
	ApiInterfacesEditor& operator=(const ApiInterfacesEditor& other);

	//! \brief The view to display the API interface info.
	EditableTableView view_;

	//! \brief The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! \brief The model that manages the API interfaces.
	ApiInterfacesModel model_;
};

#endif // APIINTERFACESEDITOR_H
