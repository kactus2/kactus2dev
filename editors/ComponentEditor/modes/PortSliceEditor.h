//-----------------------------------------------------------------------------
// File: PortSliceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// Editor to add/remove/edit the mode-elements of a component.
//-----------------------------------------------------------------------------

#ifndef PORTSLICE_EDITOR_H
#define PORTSLICE_EDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include "PortSliceModel.h"
#include "PortSliceDelegate.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class ModeValidator;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to add/remove/edit the Mode-elements of a component.
//-----------------------------------------------------------------------------
class PortSliceEditor : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] handler     The instance managing the library.
     *      @param [in] validator   The validator for PortSlice.
     *      @param [in] expressions	The collection of objects for expression handling.
	 *      @param [in] parent      The parent of this editor.
	*/
	PortSliceEditor(QSharedPointer<Component> component, 
        QSharedPointer<Mode> mode,
        QSharedPointer<PortSliceValidator> validator,
		LibraryInterface* handler,
		ExpressionSet expressions,
		QWidget* parent = 0);
	
	//! The destructor
	virtual ~PortSliceEditor() = default;

    //! No copying
    PortSliceEditor(const PortSliceEditor& other) = delete;
    PortSliceEditor& operator=(const PortSliceEditor& other) = delete;

	//! Reload the information from the model to the editor.	
	void refresh();

signals:

	/*!
	 *  Emitted when the contents of the model change.
	 */
	void contentChanged();

private:

	//! The component containing the modes.
	QSharedPointer<Component> component_;

	//! The view to display the PortSlice
	EditableTableView view_;

	//! The delegate for the view.
	PortSliceDelegate* delegate_ = nullptr;

	//! The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! The model that manages the PortSlice of the editor.
	PortSliceModel model_;
};

#endif // PORTSLICE_EDITOR_H
