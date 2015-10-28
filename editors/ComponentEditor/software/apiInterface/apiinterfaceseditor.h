//-----------------------------------------------------------------------------
// File: apiinterfaceseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The editor to add/remove/edit the API interfaces of a component.
//-----------------------------------------------------------------------------

#ifndef APIINTERFACESEDITOR_H
#define APIINTERFACESEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include "apiinterfacesmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! The editor to add/remove/edit the API interfaces of a component.
//-----------------------------------------------------------------------------
class ApiInterfacesEditor : public ItemEditor
{
	Q_OBJECT

public:

	//! The width of the name column
	static const int NAME_COLUMN_WIDTH = 100;

	//! The width of the API definition column
	static const int API_DEF_COLUMN_WIDTH = 200;

	//! The width of the dependency column.
	static const int DEPENDENCY_COLUMN_WIDTH = 100;

	//! The width of the display name column.
	static const int DISPLAY_NAME_COLUMN_WIDTH = 100;

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] handler     The instance managing the library.
	 *      @param [in] parent  The parent of this editor.
	 */
	ApiInterfacesEditor(QSharedPointer<Component> component, LibraryInterface* handler,	QWidget* parent = 0);
	
	//! The destructor
	virtual ~ApiInterfacesEditor();

	/*! Check for the validity of the editor
	* 
	*      @return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:
	
	//! No copying
	ApiInterfacesEditor(const ApiInterfacesEditor& other);

	//! No assignment
	ApiInterfacesEditor& operator=(const ApiInterfacesEditor& other);

	//! The view to display the API interface info.
	EditableTableView view_;

	//! The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! The model that manages the API interfaces.
	ApiInterfacesModel model_;
};

#endif // APIINTERFACESEDITOR_H
