//-----------------------------------------------------------------------------
// File: componenteditorswviewsitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.05.2012
//
// Description:
// The Software Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORSWVIEWSITEM_H
#define COMPONENTEDITORSWVIEWSITEM_H

#include "componenteditoritem.h"

//-----------------------------------------------------------------------------
//! The Software Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorSWViewsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] model       The model that owns the items.
	 *      @param [in] libHandler  The instance that manages the library.
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      The parent item.
	 */
	ComponentEditorSWViewsItem(ComponentEditorTreeModel* model,	LibraryInterface* libHandler,
		QSharedPointer<Component> component, ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorSWViewsItem();

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if sw views exist, otherwise not bold.
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

	/*! Get pointer to the editor of this item.
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
	ComponentEditorSWViewsItem(const ComponentEditorSWViewsItem& other);

	//! No assignment
	ComponentEditorSWViewsItem& operator=(const ComponentEditorSWViewsItem& other);

};

#endif // COMPONENTEDITORSWVIEWSITEM_H
