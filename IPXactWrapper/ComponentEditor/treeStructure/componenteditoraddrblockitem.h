/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrblockitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORADDRBLOCKITEM_H
#define COMPONENTEDITORADDRBLOCKITEM_H

#include <IPXactWrapper/ComponentEditor/treeStructure/componenteditoritem.h>
#include <models/addressblock.h>
#include <models/registermodel.h>

#include <QFont>
#include <QSharedPointer>

class AddressBlockEditor;

/*! \brief The item for a single address block in component editor's navigation tree.
 *
 */
class ComponentEditorAddrBlockItem : public ComponentEditorItem {
	Q_OBJECT

public:


	/*! \brief The constructor
	 *
	 * \param addrBlock Pointer to the address block being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorAddrBlockItem(QSharedPointer<AddressBlock> addrBlock,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorAddrBlockItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* \return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual const ItemEditor* editor() const;

	/*! \brief Add a new child to the item.
	 * 
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

protected slots:

	/*! \brief Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

private:
	
	//! \brief No copying
	ComponentEditorAddrBlockItem(const ComponentEditorAddrBlockItem& other);

	//! \brief No assignment
	ComponentEditorAddrBlockItem& operator=(const ComponentEditorAddrBlockItem& other);

	//! \brief Pointer to the address block being edited.
	QSharedPointer<AddressBlock> addrBlock_;

	//! \brief Contains the registers that belong to this address block
	QList<QSharedPointer<RegisterModel> >& regItems_;

	//! \brief Pointer to the editor to edit the address block.
	AddressBlockEditor* editor_;
};

#endif // COMPONENTEDITORADDRBLOCKITEM_H
