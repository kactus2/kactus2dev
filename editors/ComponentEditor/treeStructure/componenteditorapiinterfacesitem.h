//-----------------------------------------------------------------------------
// File: componenteditorapiinterfacesitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.5.2012
//
// Description:
// The API interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORAPIINTERFACESITEM_H
#define COMPONENTEDITORAPIINTERFACESITEM_H

#include "componenteditoritem.h"

class ApiInterface;
//-----------------------------------------------------------------------------
//! The API interfaces-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorAPIInterfacesItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] model       Pointer to the model that owns the items.
	 *      @param [in] libHandler  Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] parent      Pointer to the parent item.
	 */
	ComponentEditorAPIInterfacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorAPIInterfacesItem();

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if API interfaces exist, otherwise not bold.
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
	 *      @return Pointer to the editor to use for this item.
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
	ComponentEditorAPIInterfacesItem(const ComponentEditorAPIInterfacesItem& other);

	//! No assignment
	ComponentEditorAPIInterfacesItem& operator=(const ComponentEditorAPIInterfacesItem& other);

	//! Contains the api interfaces to edit.
	QList<QSharedPointer<ApiInterface> > apiInterfaces_;
};

#endif // COMPONENTEDITORAPIINTERFACESITEM_H
