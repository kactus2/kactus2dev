//-----------------------------------------------------------------------------
// File: componenteditorcominterfacesitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The COM interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORCOMINTERFACESITEM_H
#define COMPONENTEDITORCOMINTERFACESITEM_H

#include "componenteditoritem.h"

class Component;
class ComInterface;
//-----------------------------------------------------------------------------
//! The COM interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorComInterfacesItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] model       The model that owns the items.
	 *      @param [in] libHandler  The instance that manages the library.
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The parent item.
	 *
	*/
	ComponentEditorComInterfacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorComInterfacesItem();

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if COM interfaces exist, otherwise not bold.
	*
	*      @return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

	/*! Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! Add a new child to the item.
	 * 
	 *      @param [in] index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

private:
	//! No copying
	ComponentEditorComInterfacesItem(const ComponentEditorComInterfacesItem& other);

	//! No assignment
	ComponentEditorComInterfacesItem& operator=(const ComponentEditorComInterfacesItem& other);
};

#endif // COMPONENTEDITORCOMINTERFACESITEM_H
