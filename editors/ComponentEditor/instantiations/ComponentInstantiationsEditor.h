//-----------------------------------------------------------------------------
// File: ComponentInstantiationsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Editor for component instantiations.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATIONSEDITOR_H
#define COMPONENTINSTANTIATIONSEDITOR_H

#include "ComponentInstantiationsModel.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

class EditableTableView;
class LibraryInterface;
class InstantiationsValidator;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! Editor for component instantiations.
//-----------------------------------------------------------------------------
class ComponentInstantiationsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component           The component being edited.
	 *      @param [in] handler             The instance managing the library.
     *      @param [in] parameterFinder     Finder used to identify parameters.
     *      @param [in] validator           The validator used for component instantiations.
	 *      @param [in] parent              The parent of this editor.
	 */
	ComponentInstantiationsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<InstantiationsValidator> validator,
        QWidget* parent = 0);
	
	//! The destructor.
	virtual ~ComponentInstantiationsEditor();

	//! Reload the information from the model to the editor.
	virtual void refresh();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying.
	ComponentInstantiationsEditor(const ComponentInstantiationsEditor& other);
	ComponentInstantiationsEditor& operator=(const ComponentInstantiationsEditor& other);

	//! The view to display the view info.
	EditableTableView* view_;

	//! The proxy that does the sorting.
	QSortFilterProxyModel* proxy_;

	//! The model to manage the component instantiations summary.
	ComponentInstantiationsModel model_;
};

#endif // COMPONENTINSTANTIATIONSEDITOR_H
