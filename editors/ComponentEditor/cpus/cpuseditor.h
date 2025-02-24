//-----------------------------------------------------------------------------
// File: cpuseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// Editor to add/remove/edit the cpu-elements of a component.
//-----------------------------------------------------------------------------

#ifndef CPUSEDITOR_H
#define CPUSEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include "cpusmodel.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class CPUValidator;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor to add/remove/edit the cpu-elements of a component.
//-----------------------------------------------------------------------------
class CpusEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *    @param [in] component   The component being edited.
	 *    @param [in] handler     The instance managing the library.
     *    @param [in] validator   The validator for cpus.
     *    @param [in] expressions	The collection of objects for expression handling.
	 *    @param [in] parent      The parent of this editor.
	*/
	CpusEditor(QSharedPointer<Component> component, LibraryInterface* handler, 
        QSharedPointer<CPUValidator> validator,
		ExpressionSet expressions,
		QWidget* parent = 0);
	
	//! The destructor
	virtual ~CpusEditor() = default;

    //! No copying
    CpusEditor(const CpusEditor& other) = delete;
    CpusEditor& operator=(const CpusEditor& other) = delete;

	//! Reload the information from the model to the editor.	
	void refresh() final;

protected:

	//! Handler for widget's show event
	void showEvent(QShowEvent* event) final;

private:

	//! The view to display the cpus
	EditableTableView view_;

	//! The model that does the sorting.
	QSortFilterProxyModel proxy_;

	//! The model that manages the cpus of the editor.
	CpusModel model_;
};

#endif // CPUSEDITOR_H
