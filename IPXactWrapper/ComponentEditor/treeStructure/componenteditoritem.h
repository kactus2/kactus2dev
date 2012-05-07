/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoritem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORITEM_H
#define COMPONENTEDITORITEM_H

#include <LibraryManager/libraryinterface.h>
#include <models/component.h>

#include <QObject>
#include <QSharedPointer>
#include <QFont>
#include <QList>

class LibraryInterface;
class ItemEditor;
class ComponentEditorTreeModel;

/*! \brief ComponentEditorItem is one item in the navigation tree in component editor.
 *
 */
class ComponentEditorItem : public QObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorItem(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);
	
	/*! \brief The constructor for root item.
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent object.
	 *
	*/
	ComponentEditorItem(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorTreeModel* parent);

	//! \brief The destructor
	virtual ~ComponentEditorItem();

	/*! \brief Get the row of this item under the parent.
	 *
	 * \return The row number of this item.
	*/
	int row() const;

	/*! \brief Get the index of the child item under the parent.
	 *
	 * \param child Pointer to the child that's row is wanted.
	 *
	 * \return The index of the child item. Returns -1 if child is not found.
	*/
	int getIndexOf(const ComponentEditorItem* child) const;

	/*! \brief Get number of children the item contains.
	 *
	 * \return The number of child items this item contains.
	*/
	int rowCount() const;

	/*! \brief Returns true if item contains child items.
	 *
	 * \return True if child items exist.
	*/
	bool hasChildren() const;

	/*! \brief Get pointer to the child in given index.
	 *
	 * \param index The index identifying the child.
	 *
	 * \return Pointer to the indexed child. Null pointer is returned if index is invalid.
	*/
	ComponentEditorItem* child(const int index);

	/*! \brief Get pointer to the parent of this item.
	 *
	 * \return Pointer to the parent item.
	*/
	ComponentEditorItem* parent();

	/*! \brief Get the font to be used for text of this item.
	*
	* The default implementation is for the base elements that can not be removed.
	*
	* \return QFont instance that defines the font to be used.
	*/
	QFont getFont() const;

	/*! \brief Add a new child to the item.
	 *
	 * \param child Pointer to the child to add.
	 *
	*/
	void addChild(ComponentEditorItem* child);

	/*! \brief Remove a child from the item.
	 *
	 * \param child Pointer to the child to remove.
	 *
	*/
	void removeChild(ComponentEditorItem* child);

	/*! \brief Move child from source index to target index under the same parent.
	 *
	 * \param sourceIndex The index identifying which item to move. If not valid then nothing is done.
	 * \param targetIndex The index identifying the target to move to. If under 0 then item is
	 * prepended to the list. If greater than number of children then item is appended to the list.
	 *
	*/
	void moveChild(const int sourceIndex, int targetIndex);

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
	 * The default implementation returns null pointer. 
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

signals:

	//! \brief This signal is emitted when the item should be updated in view.
	void contentChanged(ComponentEditorItem* item);

protected:

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Contains the children of this item.
	QList<ComponentEditorItem*> childItems_;

private:
	//! \brief No copying
	ComponentEditorItem(const ComponentEditorItem& other);

	//! \brief No assignment
	ComponentEditorItem& operator=(const ComponentEditorItem& other);

	//! \brief Pointer to the parent item.
	ComponentEditorItem* parent_;
};

#endif // COMPONENTEDITORITEM_H
