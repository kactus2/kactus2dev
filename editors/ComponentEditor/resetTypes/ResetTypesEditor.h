//-----------------------------------------------------------------------------
// File: ResetTypesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.01.2019
//
// Description:
// Editor to add/remove/edit the reset types-elements of a component.
//-----------------------------------------------------------------------------

#ifndef RESETTYPESEDITOR_H
#define RESETTYPESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/resetTypes/ResetTypesModel.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class ComponentValidator;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to add/remove/edit the reset types-elements of a component.
//-----------------------------------------------------------------------------
class ResetTypesEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] component   The component being edited.
	 *      @param [in] handler     The instance managing the library.
     *      @param [in] validator   The validator for components.
	 *      @param [in] parent      The parent of this editor.
     */
	ResetTypesEditor(QSharedPointer<Component> component, LibraryInterface* handler, 
        QSharedPointer<ComponentValidator> validator, QWidget* parent = 0);
	
	/*!
     *  The destructor.
     */
	virtual ~ResetTypesEditor() = default;

    //! No copying
    ResetTypesEditor(const ResetTypesEditor& other) = delete;
    ResetTypesEditor& operator=(const ResetTypesEditor& other) = delete;

	/*!
     *  Reload the information from the model to the editor.
     */
	void refresh() final;

protected:

	/*!
     *  Handler for widget's show event.
     */
	virtual void showEvent(QShowEvent* event);

private:

	//! The view to display the reset types.
	EditableTableView view_;

	//! The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! The model that manages the reset types of the editor.
    ResetTypesModel model_;
};

#endif // RESETTYPESEDITOR_H
