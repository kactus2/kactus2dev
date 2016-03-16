//-----------------------------------------------------------------------------
// File: componenteditorapiinterfaceitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.05.2012
//
// Description:
// The item for single API-interface in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORAPIINTERFACEITEM_H
#define COMPONENTEDITORAPIINTERFACEITEM_H

#include "componenteditoritem.h"

class ApiInterface;

//-----------------------------------------------------------------------------
//! The item for single API-interface in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorAPIInterfaceItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] APIInterface Pointer to the API interface to edit.
	 *      @param [in] model Pointer to the model that owns the items.
	 *      @param [in] libHandler Pointer to the instance that manages the library.
	 *      @param [in] component Pointer to the component being edited.
	 *      @param [in] parent Pointer to the parent item.
	 */
	ComponentEditorAPIInterfaceItem(QSharedPointer<ApiInterface> APIInterface,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorAPIInterfaceItem();

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

	/*! Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! Get pointer to the editor of this item.
	 *
	 *      @return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

private:
	
	//! No copying
	ComponentEditorAPIInterfaceItem(const ComponentEditorAPIInterfaceItem& other);

	//! No assignment
	ComponentEditorAPIInterfaceItem& operator=(const ComponentEditorAPIInterfaceItem& other);

	//! Pointer to the interface being edited.
	QSharedPointer<ApiInterface> apiIf_;
};

#endif // COMPONENTEDITORAPIINTERFACEITEM_H
