//-----------------------------------------------------------------------------
// File: IndirectInterfacesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2017
//
// Description:
// Editor for component Indirect Interfaces.
//-----------------------------------------------------------------------------

#ifndef INDIRECT_INTERFACES_EDITOR_H
#define INDIRECT_INTERFACES_EDITOR_H

#include "IndirectInterfacesModel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class IndirectInterfaceValidator;
class Component;
class LibraryInterface;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! Editor to add/remove/edit the bus interfaces of a component.
//-----------------------------------------------------------------------------
class IndirectInterfacesEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] library             The IP library.
	 *      @param [in] component           The component being edited.
	 *      @param [in] validator			The validator for indirect interfaces.
	 *      @param [in] parent              The owner of this editor.
	 */
	IndirectInterfacesEditor(LibraryInterface* library, QSharedPointer<Component> component,
        QSharedPointer<IndirectInterfaceValidator> validator,
		QWidget* parent = 0);
	
	//! The destructor
	virtual ~IndirectInterfacesEditor();

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying
	IndirectInterfacesEditor(const IndirectInterfacesEditor& other);
	IndirectInterfacesEditor& operator=(const IndirectInterfacesEditor& other);

	//! The view that displays the summary of the indirect interfaces.
	EditableTableView view_;

	//! The model that does the sorting of items.
	QSortFilterProxyModel proxy_;

	//! The model that manages the indirect interfaces being displayed.
	IndirectInterfacesModel model_;
};

#endif // INDIRECT_INTERFACES_EDITOR_H
