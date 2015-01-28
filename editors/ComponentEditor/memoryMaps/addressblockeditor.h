/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockeditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSBLOCKEDITOR_H
#define ADDRESSBLOCKEDITOR_H

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/itemeditor.h>
#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/component.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class EditableTableView;
class AddressBlockModel;
class AddressBlockProxy;
class LibraryInterface;

/*! \brief The editor to edit the details of an address block within component editor.
 *
 */
class AddressBlockEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The default height and width of the editor.
	enum Sizes {
		HEIGHT = 300,
		WIDTH = 700
	};

	/*!
	 *  The constructor.
	 *
	 *      @param [in] addressBlock            Pointer to the address block being edited.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent of this editor.
	 */
	AddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
		QSharedPointer<Component> component,
		LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		QWidget* parent = 0);

	//! \brief The destructor
	virtual ~AddressBlockEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

	/*! \brief The size hint for the editor.
	 *
	 * \return QSize contains the size hint.
	*/
	virtual QSize sizeHint() const;

signals:

    /*!
     *  Change the value for address unit bits in the model.
     *
     *      @param [in] newAddressUnitBits  The new address unit bits value.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:
	
	//! \brief No copying
	AddressBlockEditor(const AddressBlockEditor& other);

	//! \brief No assignment
	AddressBlockEditor& operator=(const AddressBlockEditor& other);

	//! \brief The view to display the items.
	EditableTableView* view_;

	//! \brief The proxy that does the sorting of items.
	AddressBlockProxy* proxy_;

	//! \brief Pointer to the model that manages the details of address block.
	AddressBlockModel* model_;
};

#endif // ADDRESSBLOCKEDITOR_H
