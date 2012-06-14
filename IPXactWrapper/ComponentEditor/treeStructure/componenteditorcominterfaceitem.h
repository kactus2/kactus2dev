/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcominterfaceitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORCOMINTERFACEITEM_H
#define COMPONENTEDITORCOMINTERFACEITEM_H

#include "componenteditoritem.h"
#include <models/ComInterface.h>
#include <IPXactWrapper/ComponentEditor/software/ComInterfaceEditor.h>

/*! \brief The item for a single COM interface in component editor's navigation tree.
 *
 */
class ComponentEditorComInterfaceItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param comInterface Pointer to the COM interface being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorComInterfaceItem(QSharedPointer<ComInterface> comInterface,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorComInterfaceItem();

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
	ComponentEditorComInterfaceItem(const ComponentEditorComInterfaceItem& other);

	//! \brief No assignment
	ComponentEditorComInterfaceItem& operator=(const ComponentEditorComInterfaceItem& other);

	//! \brief Pointer to the com interface being edited.
	QSharedPointer<ComInterface> interface_;

	//! \brief The editor to edit the com interface.
	ComInterfaceEditor editor_;
};

#endif // COMPONENTEDITORCOMINTERFACEITEM_H
