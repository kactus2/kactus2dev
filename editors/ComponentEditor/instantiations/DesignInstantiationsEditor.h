//-----------------------------------------------------------------------------
// File: DesignInstantiationsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Editor for design instantiations.
//-----------------------------------------------------------------------------

#ifndef DESIGNINSTANTIATIONSEDITOR_H
#define DESIGNINSTANTIATIONSEDITOR_H

#include "DesignInstantiationsModel.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

class EditableTableView;
class LibraryInterface;
class InstantiationsValidator;
//-----------------------------------------------------------------------------
//! Editor for design instantiations.
//-----------------------------------------------------------------------------
class DesignInstantiationsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component       The component being edited.
	 *      @param [in] handler         The instance managing the library.
     *      @param [in] validator       The validator used for design instantiations.
	 *      @param [in] parent          The parent of this editor.
	 */
	DesignInstantiationsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
        QSharedPointer<InstantiationsValidator> validator, QWidget* parent = 0);
	
	//! The destructor.
	virtual ~DesignInstantiationsEditor();

	//! Reload the information from the model to the editor.
	virtual void refresh();

public slots:

	/*!
	 *  Displays warning message when trying to drag and drop a busDef or absDef using different standard revision
	 *  than the containing component.
	 */
	void stdRevisionMismatchWarning();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying.
	DesignInstantiationsEditor(const DesignInstantiationsEditor& other);
	DesignInstantiationsEditor& operator=(const DesignInstantiationsEditor& other);

	//! The view to display the view info.
	EditableTableView* view_;

	//! The proxy that does the sorting.
	QSortFilterProxyModel* proxy_;

	//! The model to manage the design instantiations summary.
	DesignInstantiationsModel model_;
};

#endif // DESIGNINSTANTIATIONSEDITOR_H
