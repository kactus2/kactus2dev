//-----------------------------------------------------------------------------
// File: SystemViewsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The editor to add/remove/edit the system views of a component.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEWSEDITOR_H
#define SYSTEMVIEWSEDITOR_H

#include "SystemViewsModel.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! The editor to add/remove/edit the system views of a component.
//-----------------------------------------------------------------------------
class SystemViewsEditor : public ItemEditor
{
	Q_OBJECT

public:

	//! The width of the name column
	static const int NAME_COLUMN_WIDTH = 100;

	//! The width of the bus definition column.
	static const int HIER_REF_COLUMN_WIDTH = 200;

	/*! The constructor
	 *
	 *      @param [in] component The component being edited.
	 *      @param [in] handler The instance managing library.
	 *      @param [in] parent The parent of this editor.
	*/
	SystemViewsEditor(QSharedPointer<Component> component, LibraryInterface* handler, QWidget* parent = 0);
	
	//! The destructor
	virtual ~SystemViewsEditor();

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
	SystemViewsEditor(const SystemViewsEditor& other);

	//! No assignment
	SystemViewsEditor& operator=(const SystemViewsEditor& other);

	//! The view that displays the summary of the bus interfaces.
	EditableTableView view_;

	//! The model that does the sorting of items.
	QSortFilterProxyModel proxy_;

	//! The model that manages the system views.
	SystemViewsModel model_;
};

#endif // SYSTEMVIEWSEDITOR_H
