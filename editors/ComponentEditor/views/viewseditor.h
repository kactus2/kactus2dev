//-----------------------------------------------------------------------------
// File: viewseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 12.06.2012
//
// Description:
// The editor to add/remove/edit views of a component.
//-----------------------------------------------------------------------------

#ifndef VIEWSEDITOR_H
#define VIEWSEDITOR_H

#include "viewsmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

class LibraryInterface;
class ViewValidator;
//-----------------------------------------------------------------------------
//! The editor to add/remove/edit views of a component.
//-----------------------------------------------------------------------------
class ViewsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component       Pointer to the component being edited.
	 *      @param [in] handler         Pointer to the instance managing the library.
     *      @param [in] viewValidator   The validator used for views.
	 *      @param [in] parent          Pointer to the parent of this editor.
	 */
	ViewsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
        QSharedPointer<ViewValidator> viewValidator, QWidget* parent = 0);
	
	//! The destructor.
	virtual ~ViewsEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

public slots:

	//! Called to update the view sorting when the model changes.
    virtual void onItemChanged();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying.
	ViewsEditor(const ViewsEditor& other);

	//! No assignment.
	ViewsEditor& operator=(const ViewsEditor& other);

	//! The view to display the view info.
	EditableTableView view_;

	//! The proxy that does the sorting.
	QSortFilterProxyModel proxy_;

	//! The model to manage the views summary.
	ViewsModel model_;
};

#endif // VIEWSEDITOR_H
