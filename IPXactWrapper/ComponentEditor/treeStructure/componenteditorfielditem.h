/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfielditem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORFIELDITEM_H
#define COMPONENTEDITORFIELDITEM_H

#include "componenteditoritem.h"
#include <models/field.h>

#include <QFont>
#include <QSharedPointer>

class FieldEditor;

/*! \brief The item for single field in component editor's navigation tree.
 *
 */
class ComponentEditorFieldItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param field Pointer to the field being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorFieldItem(QSharedPointer<Field> field,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFieldItem();

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
	ComponentEditorFieldItem(const ComponentEditorFieldItem& other);

	//! \brief No assignment
	ComponentEditorFieldItem& operator=(const ComponentEditorFieldItem& other);

	//! \brief Pointer to the field being edited.
	QSharedPointer<Field> field_;

	//! \brief Pointer to the editor to edit the field
	FieldEditor* editor_;
};

#endif // COMPONENTEDITORFIELDITEM_H
