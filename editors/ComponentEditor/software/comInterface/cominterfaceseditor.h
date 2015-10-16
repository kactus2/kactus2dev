//-----------------------------------------------------------------------------
// File: cominterfaceseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.06.2012
//
// Description:
// The editor to add/remove/edit the COM interfaces of a component.
//-----------------------------------------------------------------------------

#ifndef COMINTERFACESEDITOR_H
#define COMINTERFACESEDITOR_H

#include "cominterfacesmodel.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! The editor to add/remove/edit the COM interfaces of a component.
//-----------------------------------------------------------------------------
class ComInterfacesEditor : public ItemEditor
{
	Q_OBJECT

public:

	//! The width of the name column.
	static const int NAME_COLUMN_WIDTH = 100;

	//! The width of the com definition column.
	static const int COM_DEF_COLUMN_WIDTH = 200;

	//! The width of the transfer type column.
	static const int TRANSFER_COLUMN_WIDTH = 100;

	//! The width of the direction column.
	static const int DIRECTION_COLUMN_WIDTH = 100;

	/*! The constructor
	 *
	 *      @param [in] libHandler The instance that manages the library.
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The parent of this editor.
	 *
	*/
	ComInterfacesEditor(LibraryInterface* libHandler, QSharedPointer<Component> component, QWidget* parent = 0);
	
	//! The destructor
	virtual ~ComInterfacesEditor();

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
	ComInterfacesEditor(const ComInterfacesEditor& other);

	//! No assignment
	ComInterfacesEditor& operator=(const ComInterfacesEditor& other);

	//! The view to display the COM interfaces.
	EditableTableView view_;

	//! The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! The model that manages the COM interfaces.
	ComInterfacesModel model_;
};

#endif // COMINTERFACESEDITOR_H
