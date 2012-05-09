/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORMEMMAPSITEM_H
#define COMPONENTEDITORMEMMAPSITEM_H

#include "componenteditoritem.h"

/*! \brief The Memory maps-item in the component navigation tree.
 *
 */
class ComponentEditorMemMapsItem : public ComponentEditorItem {
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
	ComponentEditorMemMapsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorMemMapsItem();

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
	ComponentEditorMemMapsItem(const ComponentEditorMemMapsItem& other);

	//! \brief No assignment
	ComponentEditorMemMapsItem& operator=(const ComponentEditorMemMapsItem& other);
};

#endif // COMPONENTEDITORMEMMAPSITEM_H
