//-----------------------------------------------------------------------------
// File: PowerDomainsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 5.10.2023
//
// Description:
// Editor for power domains of a component.
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINSEDITOR_H
#define POWERDOMAINSEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/common/ExpressionSet.h>

#include <common/views/EditableTableView/editabletableview.h>

#include "PowerDomainsModel.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class LibraryInterface;
class OtherClockDriverValidator;

//-----------------------------------------------------------------------------
//! Editor for power domains of a component.
//-----------------------------------------------------------------------------
class PowerDomainsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] component       The component being edited.
	 *      @param [in] handler         The instance managing the library.
     *      @param [in] clockValidator  Validator used for other clock drivers.
     *      @param [in] expressions		The collection of objects for expression handling.
	 *      @param [in] parent          The owner of this widget.
	 *
	 */
	PowerDomainsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
        //QSharedPointer<OtherClockDriverValidator> clockValidator, 
		ExpressionSet expressions,
		QWidget *parent = 0);
	
	//! The destructor.
	virtual ~PowerDomainsEditor() = default;

    //! No copying.
    PowerDomainsEditor(const PowerDomainsEditor& other) = delete;
	PowerDomainsEditor& operator=(const PowerDomainsEditor& other) = delete;

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();


protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:
	
	//! Setup the widget layout.
    void setupLayout();

	//! The view that displays the parameters.
	EditableTableView view_;

	//! The model that holds the data to be displayed to the user
	PowerDomainsModel model_;

	//! The proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

#endif // POWERDOMAINSEDITOR_H
