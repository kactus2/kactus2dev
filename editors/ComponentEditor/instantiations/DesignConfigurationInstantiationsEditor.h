//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 29.01.2016
//
// Description:
// Editor for DesignConfiguration instantiations.
//-----------------------------------------------------------------------------

#ifndef DESIGNCONFIGURATIONINSTANTIATIONSEDITOR_H
#define DESIGNCONFIGURATIONINSTANTIATIONSEDITOR_H

#include "DesignConfigurationInstantiationsModel.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

class EditableTableView;
class LibraryInterface;
class InstantiationsValidator;
//-----------------------------------------------------------------------------
//! Editor for DesignConfiguration instantiations.
//-----------------------------------------------------------------------------
class DesignConfigurationInstantiationsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component       The component being edited.
	 *      @param [in] handler         The instance managing the library.
     *      @param [in] validator       The validator used for design configuration instantiations.
	 *      @param [in] parent          The parent of this editor.
	 */
	DesignConfigurationInstantiationsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
        QSharedPointer<InstantiationsValidator> validator, QWidget* parent = 0);
	
	//! The destructor.
	virtual ~DesignConfigurationInstantiationsEditor();

	//! Reload the information from the model to the editor.
	virtual void refresh();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying.
	DesignConfigurationInstantiationsEditor(const DesignConfigurationInstantiationsEditor& other);
	DesignConfigurationInstantiationsEditor& operator=(const DesignConfigurationInstantiationsEditor& other);

	//! The view to display the view info.
	EditableTableView* view_;

	//! The proxy that does the sorting.
	QSortFilterProxyModel* proxy_;

	//! The model to manage the design configuration instantiations summary.
	DesignConfigurationInstantiationsModel model_;
};

#endif // DESIGNCONFIGURATIONINSTANTIATIONSEDITOR_H
