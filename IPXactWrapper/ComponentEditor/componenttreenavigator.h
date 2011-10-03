/* 
 *
 *  Created on: 1.2.2011
 *      Author: Antti Kamppi
 * 		filename: componenttreenavigator.h
 */

#ifndef COMPONENTTREENAVIGATOR_H
#define COMPONENTTREENAVIGATOR_H

#include "componenttreemodel.h"
#include "componenttreeview.h"
#include <models/component.h>

#include <QWidget>
#include <QGroupBox>
#include <QSize>
#include <QPushButton>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief ComponentTreeNavigator is used to select IP-Xact elements for editing.
 * 
 * Navigator contains a tree view and an add button and a remove button to 
 * add/remove elements in a component and it's subelements. 
 */
class ComponentTreeNavigator : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component that is being edited
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ComponentTreeNavigator(QSharedPointer<Component> component, 
		LibraryInterface* handler,
		QWidget *parent);

	//! \brief The destructor
	virtual ~ComponentTreeNavigator();

	/*! \brief Update the selected item's name through the model
	 *
	 * \param newName The new name for the specified item.
	 * \param index QModelIndex that specifies which item is changed
	 *
	*/
	void updateItemName(const QString newName, const QModelIndex& index);

	/*! \brief Tell model to update the given item.
	 *
	 * \param index QModelIndex that specifies the item to be updated
	 *
	*/
	void updateItem(const QModelIndex& index);

	/*! \brief Get the sizeHint of this widget
	 *
	 * This function is used by the layout managers to adjust the sizes of the 
	 * widgets.
	 * 
	 * \return QSize containing the desired size-information.
	*/
	virtual QSize sizeHint() const;

	/*! \brief Clear the selection in the tree.
	 *
	*/
	void clearSelection();

	/*! \brief Set the add and remove buttons to enabled/disabled.
	 *
	 * \param locked If true then buttons are disabled.
	 *
	*/
	void setLocked(bool locked);

public slots:

	//! \brief Called when user clicks "Add" button.
	void onAdd();

	//! \brief Called when user clicks "Remove" button.
	void onRemove();

	/*! \brief The handler for view's clicked-signal.
	 * 
	 * This function always emits the itemSelected signal but also 
	 * disables/enables the removeButton according to what kind of item was 
	 * selected.
	 *
	 * \param index QModelIndex that specifies which item was selected.
	 *
	*/
	void onItemSelected(const QModelIndex& index);

	//! \brief Select the specified item in the view.
	void selectItem(const QModelIndex& index);

signals:

	//! \brief Emitted when a user clicks an item in the tree view.
	void itemSelected(const QModelIndex& index);

	//! \brief Emitted when user clicks the remove button.
	void itemRemoved(const QModelIndex& index);

	//! \brief Emitted when an item is moved in the tree view
	void itemMoved();

private:

	//! No copying
	ComponentTreeNavigator(const ComponentTreeNavigator& other);

	//! No assignment
	ComponentTreeNavigator& operator=(const ComponentTreeNavigator& other);

	//! \brief The tree view to display the navigation tree.
	ComponentTreeView view_;

	//! \brief The tree model to manage the navigation tree.
	ComponentTreeModel model_;

	//! \brief The button to add a new item to the tree
	QPushButton addButton_;

	//! \brief The button to remove an item from the tree.
	QPushButton removeButton_;
	
	//! \brief Info on the locked status of the editor.
	bool locked_;
};

#endif // COMPONENTTREENAVIGATOR_H
