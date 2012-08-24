/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorregisteritem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORREGISTERITEM_H
#define COMPONENTEDITORREGISTERITEM_H

#include "componenteditoritem.h"
#include <models/component.h>
#include <models/register.h>

#include <QSharedPointer>

class RegisterEditor;

/*! \brief The item for single register in component editor's navigation tree.
 *
 */
class ComponentEditorRegisterItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param reg Pointer to the register being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorRegisterItem(QSharedPointer<Register> reg, 
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorRegisterItem();

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

private:
	
	//! \brief No copying
	ComponentEditorRegisterItem(const ComponentEditorRegisterItem& other);

	//! \brief No assignment
	ComponentEditorRegisterItem& operator=(const ComponentEditorRegisterItem& other);

	//! \brief Pointer to the register being edited.
	QSharedPointer<Register> reg_;

	//! \brief Pointer to the editor to edit the register.
	RegisterEditor* editor_;
};

#endif // COMPONENTEDITORREGISTERITEM_H
