//-----------------------------------------------------------------------------
// File: swviewseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The editor to add/remove/edit the software views of a component.
//-----------------------------------------------------------------------------

#ifndef SWVIEWSEDITOR_H
#define SWVIEWSEDITOR_H

#include "swviewsmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

class LibraryInterface;
//-----------------------------------------------------------------------------
//! The editor to add/remove/edit the software views of a component.
//-----------------------------------------------------------------------------
class SWViewsEditor : public ItemEditor
{
	Q_OBJECT

public:

	//! The width of the name column
	static const int NAME_COLUMN_WIDTH = 100;

	//! The width of the bus definition column.
	static const int HIER_REF_COLUMN_WIDTH = 200;

	/*! The constructor
	 *
	 *      @param [in] component Pointer to the component being edited.
	 *      @param [in] handler Pointer to the instance managing the library.
	 *      @param [in] parent Pointer to the parent of this editor.
	*/
	SWViewsEditor(QSharedPointer<Component> component, LibraryInterface* handler, QWidget* parent = 0);
	
	//! The destructor
	virtual ~SWViewsEditor();

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
	SWViewsEditor(const SWViewsEditor& other);

	//! No assignment
	SWViewsEditor& operator=(const SWViewsEditor& other);

	//! The view that displays the summary of the bus interfaces.
	EditableTableView view_;

	//! The model that does the sorting of items.
	QSortFilterProxyModel proxy_;

	//! The model that manages the software views.
	SWViewsModel model_;
};

#endif // SWVIEWSEDITOR_H
