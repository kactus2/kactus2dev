//-----------------------------------------------------------------------------
// File: businterfaceseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// Editor to add/remove/edit the bus interfaces of a component.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACESEDITOR_H
#define BUSINTERFACESEDITOR_H

#include "businterfacesmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class BusInterfaceValidator;
class Component;
class LibraryInterface;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! Editor to add/remove/edit the bus interfaces of a component.
//-----------------------------------------------------------------------------
class BusInterfacesEditor : public ItemEditor
{
	Q_OBJECT

public:

	//! The width of the name column
	static const int NAME_COLUMN_WIDTH = 100;

	//! The width of the bus definition column.
	static const int BUS_COLUMN_WIDTH = 200;

	//! The width of the abs def column.
	static const int ABS_COLUMN_WIDTH = 200;

	//! The width of the interface mode column.
	static const int IF_MODE_COLUMN_WIDTH = 100;

	/*!
	 *  The constructor.
	 *
	 *      @param [in] handler             The instance that manages the library.
	 *      @param [in] component           The component being edited.
	 *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] parent              The owner of this editor.
	 */
	BusInterfacesEditor(LibraryInterface* handler, QSharedPointer<Component> component,
        QSharedPointer<BusInterfaceValidator> validator,
        QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent = 0);
	
	//! The destructor
	virtual ~BusInterfacesEditor();

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying
	BusInterfacesEditor(const BusInterfacesEditor& other);
	BusInterfacesEditor& operator=(const BusInterfacesEditor& other);

	//! The view that displays the summary of the bus interfaces.
	EditableTableView view_;

	//! The model that does the sorting of items.
	QSortFilterProxyModel proxy_;

	//! The model that manages the bus interfaces being displayed.
	BusInterfacesModel model_;
};

#endif // BUSINTERFACESEDITOR_H
