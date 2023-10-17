//-----------------------------------------------------------------------------
// File: FieldSliceEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 08.08.2023
//
// Description:
// Editor to add/remove/edit the mode-elements of a component.
//-----------------------------------------------------------------------------

#ifndef FIELDSLICE_EDITOR_H
#define FIELDSLICE_EDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include "FieldSliceModel.h"
//#include "FieldSliceDelegate.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class ModeValidator;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to add/remove/edit the Mode-elements of a component.
//-----------------------------------------------------------------------------
class FieldSliceEditor : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *		@param [in] mode		The mode being edited.
     *      @param [in] validator   The validator for FieldSlice.
     *      @param [in] expressions	The collection of objects for expression handling.
	 *      @param [in] handler     The instance managing the library.
	 *      @param [in] parent      The parent of this editor.
	*/
	FieldSliceEditor(QSharedPointer<Component> component, 
        QSharedPointer<Mode> mode,
        QSharedPointer<FieldSliceValidator> validator,
		ExpressionSet expressions,
		LibraryInterface* handler,
		QWidget* parent = nullptr);
	
	//! The destructor
	virtual ~FieldSliceEditor() = default;

    //! No copying
    FieldSliceEditor(const FieldSliceEditor& other) = delete;
    FieldSliceEditor& operator=(const FieldSliceEditor& other) = delete;

	//! Reload the information from the model to the editor.	
	void refresh();

signals:
	
	/*!
	 *  Increase the amount of references to a parameter corresponding to the id.
	 *
	 *      @param [in] parameterId     The id of the parameter being referenced.
	 */
	void increaseReferences(QString const& parameterId);

	/*!
	 *  Decrease the amount of references to a parameter corresponding to the id.
	 *
	 *      @param [in] parameterId     The id of the parameter being referenced.
	 */
	void decreaseReferences(QString const& parameterId);

	/*!
	 *  Emitted when the contents of the model change.
	 */
	void contentChanged();

private:

	//! The component containing the modes.
	QSharedPointer<Component> component_;

	//! The view to display the FieldSlice
	EditableTableView view_;

	//! The delegate for the view.
	//FieldSliceDelegate* delegate_ = nullptr;

	//! The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! The model that manages the FieldSlice of the editor.
	FieldSliceModel model_;
};

#endif // FIELDSLICE_EDITOR_H
