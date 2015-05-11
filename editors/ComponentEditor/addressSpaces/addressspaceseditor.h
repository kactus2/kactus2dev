/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceseditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACESEDITOR_H
#define ADDRESSSPACESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <editors/ComponentEditor/itemeditor.h>
#include "addressspacesmodel.h"

#include <QSortFilterProxyModel>

class LibraryInterface;

/*! The editor to add/remove/edit address spaces of component.
 *
 */
class AddressSpacesEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance managing the library.
     *      @param [in] parameterFinder         Finder for parameter names.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        The expression parser to use.
	*/
	AddressSpacesEditor(QSharedPointer<Component> component,
		 LibraryInterface* handler,
         QSharedPointer<ParameterFinder> parameterFinder,
         QSharedPointer<ExpressionFormatter> expressionFormatter,
         QSharedPointer<ExpressionParser> expressionParser);
	
	//! The destructor
	~AddressSpacesEditor();

	/*! Check for the validity of the editor
	* 
	*       @return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

signals:

	//! Emitted when a bus interface editor should be opened.
	void selectBusInterface(const QString& interfaceName);

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

    //! Handler for view's doubleClicked-signal.
    virtual void onDoubleClick(const QModelIndex& index);

private:
	
	//! No copying
	AddressSpacesEditor(const AddressSpacesEditor& other);
	AddressSpacesEditor& operator=(const AddressSpacesEditor& other);

	//! The view to display the address spaces.
	EditableTableView view_;

	//! The model that manages the file set objects for the view.
	AddressSpacesModel model_;

	//! The proxy to do the sorting
	QSortFilterProxyModel proxy_;
};

#endif // ADDRESSSPACESEDITOR_H
