/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorrootitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORROOTITEM_H
#define COMPONENTEDITORROOTITEM_H

#include "componenteditoritem.h"
#include <models/component.h>

#include <QSharedPointer>
#include <QString>
#include <QWidget>

class ItemEditor;
class ComponentEditorTreeModel;
class LibraryInterface;

/*! \brief The root item in the component editor's navigation tree.
 *
 */
class ComponentEditorRootItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner model of this root item.
	 *
	*/
	ComponentEditorRootItem(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorTreeModel* parent,
        QWidget* parentWnd);
	
	//! \brief The destructor
	virtual ~ComponentEditorRootItem();

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

	/*! \brief Get the tree item that holds the editor for given bus interface.
	 *
	 * \param interfaceName The name of the bus interface to get the item for.
	 *
	 * \return Pointer to the tree item.
	*/
	QSharedPointer<ComponentEditorItem> getBusInterfaceItem(const QString& interfaceName) const;

private:
	//! \brief No copying
	ComponentEditorRootItem(const ComponentEditorRootItem& other);

	//! \brief No assignment
	ComponentEditorRootItem& operator=(const ComponentEditorRootItem& other);
};

#endif // COMPONENTEDITORROOTITEM_H
