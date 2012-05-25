/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswpropertiesitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORSWPROPERTIESITEM_H
#define COMPONENTEDITORSWPROPERTIESITEM_H

#include "componenteditoritem.h"
#include <IPXactWrapper/ComponentEditor/software/SWPropertiesEditor.h>
#include <models/ComProperty.h>

/*! \brief The Software Properties-item in the component editor's navigation tree.
 *
 */
class ComponentEditorSWPropertiesItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorSWPropertiesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorSWPropertiesItem();

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

private:
	//! \brief No copying
	ComponentEditorSWPropertiesItem(const ComponentEditorSWPropertiesItem& other);

	//! \brief No assignment
	ComponentEditorSWPropertiesItem& operator=(const ComponentEditorSWPropertiesItem& other);

	//! \brief Contains the properties to edit
	QList< QSharedPointer<ComProperty> >& swProperties_;

	//! \brief The editor to edit the properties
	SWPropertiesEditor editor_;
};

#endif // COMPONENTEDITORSWPROPERTIESITEM_H
