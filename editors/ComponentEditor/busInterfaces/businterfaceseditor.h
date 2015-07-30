/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfaceseditor.h
 *		Project: Kactus 2
 */

#ifndef BUSINTERFACESEDITOR_H
#define BUSINTERFACESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <editors/ComponentEditor/itemeditor.h>
#include "businterfacesmodel.h"
#include <IPXACTmodels/component.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class LibraryInterface;
class ParameterFinder;

/*! \brief Editor to add/remove/edit the bus interfaces of a component.
 *
 */
class BusInterfacesEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The width of the name column
	static const int NAME_COLUMN_WIDTH = 100;

	//! \brief The width of the bus definition column.
	static const int BUS_COLUMN_WIDTH = 200;

	//! \brief The width of the abs def column.
	static const int ABS_COLUMN_WIDTH = 200;

	//! \brief The width of the interface mode column.
	static const int IF_MODE_COLUMN_WIDTH = 100;

	/*!
	 *  The constructor.
	 *
	 *      @param [in] handler             Pointer to the instance that manages the library.
	 *      @param [in] component           Pointer to the component being edited.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
	 *      @param [in] parent              Pointer to the owner of this editor.
	 */
	BusInterfacesEditor(LibraryInterface* handler, QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder, QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~BusInterfacesEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! \brief No copying
	BusInterfacesEditor(const BusInterfacesEditor& other);

	//! \brief No assignment
	BusInterfacesEditor& operator=(const BusInterfacesEditor& other);

	//! \brief The view that displays the summary of the bus interfaces.
	EditableTableView view_;

	//! \brief The model that does the sorting of items.
	QSortFilterProxyModel proxy_;

	//! \brief The model that manages the bus interfaces being displayed.
	BusInterfacesModel model_;
};

#endif // BUSINTERFACESEDITOR_H
