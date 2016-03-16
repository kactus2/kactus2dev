//-----------------------------------------------------------------------------
// File: componenteditorswpropertiesitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.5.2012
//
// Description:
// The Software Properties-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------


#ifndef COMPONENTEDITORSWPROPERTIESITEM_H
#define COMPONENTEDITORSWPROPERTIESITEM_H

#include "componenteditoritem.h"

#include <IPXACTmodels/kactusExtensions/ComProperty.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The Software Properties-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorSWPropertiesItem : public ComponentEditorItem
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
	ComponentEditorSWPropertiesItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
		QSharedPointer<Component> component, ComponentEditorItem* parent);

	//! The destructor
	virtual ~ComponentEditorSWPropertiesItem();

	/*! Get the font to be used for text of this item.
	*
	* The font is bold, if other SW properties exist, otherwise not bold.
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

	/*! Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

private:
	//! No copying
	ComponentEditorSWPropertiesItem(const ComponentEditorSWPropertiesItem& other);

	//! No assignment
	ComponentEditorSWPropertiesItem& operator=(const ComponentEditorSWPropertiesItem& other);

};

#endif // COMPONENTEDITORSWPROPERTIESITEM_H
