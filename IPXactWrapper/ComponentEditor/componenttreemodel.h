/* 
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#ifndef COMPONENTTREEMODEL_H
#define COMPONENTTREEMODEL_H

#include <models/component.h>

#include <QAbstractItemModel>
#include <QVariant>
#include <QModelIndex>
#include <QSharedPointer>

class IPXactWrapperItemInterface;
class ComponentTreeItem;
class LibraryInterface;

/*! \brief ComponentTreeModel is a model-class for ComponentTreeView.
 * 
 * This model contains the ComponentTreeItems that represent IP-Xact elements
 * to edit in a component.
 */
class ComponentTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:

    /*! \brief The constructor
     *
     * \param component Pointer to the component instance that is edited.
     * \param handler Pointer to the instance that manages the library.
     * \param parent Pointer to the owner of this model.
     */
    ComponentTreeModel(QSharedPointer<Component> component,
		LibraryInterface* handler,
		QObject *parent = 0);

	//! \brief The destructor
    virtual ~ComponentTreeModel();

    /*! \brief Create a model index for given item.
     *
     * \param row Row of the item
     * \param column Column of the item
     * \param parent Model index of the parent of the item
     *
     * \return QModelIndex that can be used to identify the item in the tree.
    */
    QModelIndex index(int row, int column,
    		const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the model index of the general-editor.
	 *
	 * \return QModelIndex The index of the general editor.
	*/
	QModelIndex generalEditorIndex() const;

    /*! \brief Get the model index of the parent of specified item.
     *
     * \param index The model index of the item that's parent is requested.
     *
     * \return QModelIndex that identifies the item's parent.
    */
    QModelIndex parent(const QModelIndex& index) const;

    /*! \brief Get the number of rows the specified item has.
     *
     * \param parent Model index of the item that's row count is requested.
     *
     * \return The number of rows that the item has. This is the number of
     * item's child items.
    */
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*! \brief Get the number of columns the item has
     * 
     * This function always returns 1.
     * 
     * \param parent Model index of the item that's column count is requested.
     *
     * \return Always returns 1
    */
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /*! \brief Get the data to be displayed for specified item
     *
     * \param index Model index of the item that's data is requested.
     * \param role Specifies the type of data that is requested.
     *
     * \return QVariant containing the data.
    */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /*! \brief Does the specified item have any child items
     *
     * \param parent Model index of the item.
     *
     * \return True if the item has any child items. This function is implemented
     * for performance reasons.
    */
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

    /*! \brief Get info on how the items can be handled.
     *
     * \param index The model index of the item that's flags are requested.
     *
     * \return Qt::ItemFlags that define how item can be handled.
    */
    Qt::ItemFlags flags(const QModelIndex& index) const;

    /*! \brief Get the headers for a given column
     *
     * \param section The column that's header is requested
	 * \param orientation The orientation of the header, only Qt::Horizontal is
	 * supported
     * \param role The role that specifies the type of data that is requested.
     *
     * \return QVariant that contains the data for given header.
    */
    QVariant headerData(int section, Qt::Orientation orientation,
    		int role = Qt::DisplayRole) const;

	/*! \brief Set a new name for a specified item in the model.
	*
	* \param newName The new name for the item.
	* \param index The model index that specifies the item to change.
	*/
	void setItemName(const QString newName, const QModelIndex& index);

	/*! \brief Tell view to update the specified model index.
	 *
	 * \param index QModelIndex that specifies which item to update.
	 *
	*/
	void updateItem(const QModelIndex& index);

	/*! \brief Remove the specified item from the tree.
	 *
	 * \param index QModelIndex that specifies which item is to be removed.
	 * All the item's child items are also removed.
	 *
	 * \return void
	*/
	void removeItem(const QModelIndex& index);

	/*! \brief Create a new child for the specified parent.
	 *
	 * \param index Model index of the item to who new child is created for.
	 *
	 * \return Model index of the created child item.
	*/
	QModelIndex createChild(const QModelIndex& parent);

public slots:

	//! \brief Move an item from one place to another.
	void moveItem(const QModelIndex& source, const QModelIndex& target);

signals:

	//! \brief Emitted when an item is moved
	void itemMoved();

private:

	//! No copying
	ComponentTreeModel(const ComponentTreeModel& other);

	//! No assignment
	ComponentTreeModel& operator=(const ComponentTreeModel& other);

	//! \brief Pointer to the root item of the model
	ComponentTreeItem* rootItem_;

	//! \brief Pointer to the component that is being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

};

#endif // COMPONENTTREEMODEL_H
