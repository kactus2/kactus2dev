//-----------------------------------------------------------------------------
// File: addressspaceseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.06.2012
//
// Description:
// The editor to add/remove/edit address spaces of component.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACESEDITOR_H
#define ADDRESSSPACESEDITOR_H

#include "addressspacesmodel.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <editors/ComponentEditor/itemeditor.h>

#include <QSortFilterProxyModel>

class LibraryInterface;
class Component;

class AddressSpaceValidator;
//-----------------------------------------------------------------------------
//! The editor to add/remove/edit address spaces of component.
//-----------------------------------------------------------------------------
class AddressSpacesEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance managing the library.
     *      @param [in] parameterFinder         Finder for parameter names.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        The expression parser to use.
     *      @param [in] addressSpaceValidator   Validator used for address spaces.
	 */
	AddressSpacesEditor(QSharedPointer<Component> component,
		 LibraryInterface* handler,
         QSharedPointer<ParameterFinder> parameterFinder,
         QSharedPointer<ExpressionFormatter> expressionFormatter,
         QSharedPointer<ExpressionParser> expressionParser,
         QSharedPointer<AddressSpaceValidator> addressSpaceValidator);
	
	//! The destructor
	~AddressSpacesEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

signals:

	//! Emitted when a bus interface editor should be opened.
	void selectBusInterface(const QString& interfaceName);

    /*!
     *  Emitted when address unit bits change on the given row.
     *
     *      @param [in] spaceIndex  The index of the address space where address unit bits were changed.
     */
    void aubChangedOnRow(int spaceIndex) const;

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

    //! Handler for view's doubleClicked-signal.
    virtual void onDoubleClick(const QModelIndex& index);

private:
	
	//! No copying.
	AddressSpacesEditor(const AddressSpacesEditor& other);
	AddressSpacesEditor& operator=(const AddressSpacesEditor& other);

	//! The view to display the address spaces.
	EditableTableView view_;

	//! The model that manages the file set objects for the view.
	AddressSpacesModel model_;
};

#endif // ADDRESSSPACESEDITOR_H
