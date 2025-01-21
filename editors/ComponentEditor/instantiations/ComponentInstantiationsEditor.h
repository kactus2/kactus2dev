//-----------------------------------------------------------------------------
// File: ComponentInstantiationsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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
class ComponentInstantiationInterface;

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
	 *    @param [in] component               The component being edited.
	 *    @param [in] handler                 The instance managing the library.
     *    @param [in] parameterFinder         Finder used to identify parameters.
     *    @param [in] instantiationInterface  Interface for accessing component instantiations.
	 *    @param [in] parent                  The parent of this editor.
	 */
    ComponentInstantiationsEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        ComponentInstantiationInterface* instantiationInterface,
        QWidget* parent = 0);

	//! The destructor.
	virtual ~ComponentInstantiationsEditor() = default;

    //! No copying.
    ComponentInstantiationsEditor(const ComponentInstantiationsEditor& other) = delete;
    ComponentInstantiationsEditor& operator=(const ComponentInstantiationsEditor& other) = delete;

	//! Reload the information from the model to the editor.
	virtual void refresh();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:

	//! The view to display the view info.
	EditableTableView* view_;

	//! The proxy that does the sorting.
	QSortFilterProxyModel* proxy_;

	//! The model to manage the component instantiations summary.
    ComponentInstantiationsModel* model_;

    //! Interface for accessing component instantiations.
    ComponentInstantiationInterface* instantiationInterface_;

    //! The currently available component instantiations.
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > availableInstantiations_;
};

#endif // COMPONENTINSTANTIATIONSEDITOR_H
