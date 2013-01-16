/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockeditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSBLOCKEDITOR_H
#define ADDRESSBLOCKEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/addressblock.h>
#include <models/component.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class EditableTableView;
class AddressBlockModel;
class AddressBlockProxy;

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

	/*! \brief The constructor
	 *
	 * \param addressBlock Pointer to the address block being edited.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	 */
	AddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
		QSharedPointer<Component> component,
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
