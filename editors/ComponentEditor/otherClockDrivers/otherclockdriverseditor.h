//-----------------------------------------------------------------------------
// File: otherclockdriverseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// Editor for otherClockDrivers of a component.
//-----------------------------------------------------------------------------

#ifndef OTHERCLOCKDRIVERSEDITOR_H
#define OTHERCLOCKDRIVERSEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <common/views/EditableTableView/editabletableview.h>

#include "otherclockdriversmodel.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class LibraryInterface;
class OtherClockDriverValidator;

//-----------------------------------------------------------------------------
//! Editor for otherClockDrivers of a component.
//-----------------------------------------------------------------------------

class OtherClockDriversEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] component       The component being edited.
	 *      @param [in] handler         The instance managing the library.
     *      @param [in] clockValidator  Validator used for other clock drivers.
	 *      @param [in] parent          The owner of this widget.
	 *
	 */
	OtherClockDriversEditor(QSharedPointer<Component> component, LibraryInterface* handler,
        QSharedPointer<OtherClockDriverValidator> clockValidator, QWidget *parent = 0);
	
	//! The destructor.
	virtual ~OtherClockDriversEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:
	
	//! No copying.
	OtherClockDriversEditor(const OtherClockDriversEditor& other);
	OtherClockDriversEditor& operator=(const OtherClockDriversEditor& other);

	//! The view that displays the parameters.
	EditableTableView view_;

	//! The model that holds the data to be displayed to the user
	OtherClockDriversModel model_;

	//! The proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

#endif // OTHERCLOCKDRIVERSEDITOR_H
