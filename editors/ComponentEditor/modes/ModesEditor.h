//-----------------------------------------------------------------------------
// File: Modeseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// Editor to add/remove/edit the mode-elements of a component.
//-----------------------------------------------------------------------------

#ifndef MODESEDITOR_H
#define MODESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include "ModesModel.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class ModeValidator;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to add/remove/edit the Mode-elements of a component.
//-----------------------------------------------------------------------------
class ModesEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *    @param [in] component   The component being edited.
	 *    @param [in] handler     The instance managing the library.
     *    @param [in] validator   The validator for Modes.
	 *    @param [in] parent      The parent of this editor.
	*/
	ModesEditor(QSharedPointer<Component> component, LibraryInterface* handler, 
		QWidget* parent = 0);
	
	//! The destructor
	virtual ~ModesEditor() = default;

    //! No copying
    ModesEditor(const ModesEditor& other) = delete;
    ModesEditor& operator=(const ModesEditor& other) = delete;

	//! Reload the information from the model to the editor.	
	void refresh() final;

protected:

	//! Handler for widget's show event
	void showEvent(QShowEvent* event) final;

private:

	//! The view to display the Modes
	EditableTableView view_;

	//! The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! The model that manages the Modes of the editor.
	ModesModel model_;
};

#endif // MODESEDITOR_H
