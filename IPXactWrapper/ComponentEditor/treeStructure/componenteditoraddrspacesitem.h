/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspacesitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORADDRSPACESITEM_H
#define COMPONENTEDITORADDRSPACESITEM_H

#include "componenteditoritem.h"

/*! \brief The Address spaces-item in the component editor navigation tree.
*
*/
class ComponentEditorAddrSpacesItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorAddrSpacesItem();

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
	ComponentEditorAddrSpacesItem(const ComponentEditorAddrSpacesItem& other);

	//! \brief No assignment
	ComponentEditorAddrSpacesItem& operator=(const ComponentEditorAddrSpacesItem& other);
};

#endif // COMPONENTEDITORADDRSPACESITEM_H
