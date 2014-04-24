/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorapiinterfaceitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORAPIINTERFACEITEM_H
#define COMPONENTEDITORAPIINTERFACEITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/ApiInterface.h>

/*! \brief The item for single API-interface in component editor's navigation tree.
 *
 */
class ComponentEditorAPIInterfaceItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param APIInterface Pointer to the API interface to edit.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorAPIInterfaceItem(QSharedPointer<ApiInterface> APIInterface,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorAPIInterfaceItem();

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the font to be used for text of this item.
	*
	* \return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

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
	ComponentEditorAPIInterfaceItem(const ComponentEditorAPIInterfaceItem& other);

	//! \brief No assignment
	ComponentEditorAPIInterfaceItem& operator=(const ComponentEditorAPIInterfaceItem& other);

	//! \brief Pointer to the interface being edited.
	QSharedPointer<ApiInterface> apiIf_;
};

#endif // COMPONENTEDITORAPIINTERFACEITEM_H
